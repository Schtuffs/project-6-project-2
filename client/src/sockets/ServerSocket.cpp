#define _SERVER_SOCKET_IMPL
#include <sockets/ServerSocket.h>

#include <print>

#include <boost/asio/post.hpp>

#include <sockets/Packet.h>

static bool s_isRunning = true;

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
    BOOL WINAPI EventHandler(DWORD dwType) {
        switch (dwType) {
            case CTRL_C_EVENT:
                std::print("Shutting down server\n");
                s_isRunning = false;

                // Remove event handler incase of freeze
                if (!SetConsoleCtrlHandler(EventHandler, false)) {
                    std::print("Failed to remove Ctrl+C event handler\n");
                }
                return TRUE;
            default:
                break;
        }
        return FALSE;
    }
#endif

// ----- Creation ----- Destruction -----

ServerSocket::ServerSocket(CONNECTION_TYPE cType, uint16_t port) : Socket(cType, port) {
    // Check socket creation
    if (!mIsSetup) {
        return;
    }
    
    // Mount event handler
    #ifdef _WIN32
        if (!SetConsoleCtrlHandler(EventHandler, true)) {
            std::print("Failed to set Ctrl+C event handler\n");
        }
    #endif

    // Setup server information
    m_svrAddr.sin_family = AF_INET;
    m_svrAddr.sin_addr.s_addr = INADDR_ANY;
    m_svrAddr.sin_port = htons(mPort);

    // Bind the socket
    if (bind(mSocket, (sockaddr*)&m_svrAddr, sizeof(m_svrAddr)) != 0) {
        std::print("ERROR: Failed to bind socket\n");
        mIsSetup = false;
        return;
    }
    
    // TCP server
    if (mType == CONNECTION_TYPE::TCP) {
        // Listen property for socket
        if (listen(mSocket, 10) != 0) {
            std::print("ERROR: Listen failed, could not configure socket\n");
            mIsSetup = false;
            return;
        }
    }

    m_isRunning = false;
    s_isRunning = true;

    std::println("{} server bound to port {}", (mType == CONNECTION_TYPE::TCP ? "TCP" : "UDP"), mPort);
}

ServerSocket::~ServerSocket() {
    m_isRunning = false;

    // Unmount event handler
    #ifdef _WIN32
        SetConsoleCtrlHandler(EventHandler, false);
    #endif

    // If threaded
    if (m_packetPool != nullptr) {
        m_packetPool->stop();
        m_packetPool->join();
        delete m_packetPool;
        m_packetPool = nullptr;
    }
    
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }
    std::println("{} server closed on port {}", (mType == CONNECTION_TYPE::TCP ? "TCP" : "UDP"), mPort);
}



// ----- Read -----

bool ServerSocket::accept(int millis) {
    // Check socket
    if (!poll(mSocket, millis)) {
        // Nothing found
        return false;
    }

#ifdef _WIN32
    int length = sizeof(m_svrAddr);
#else
    socklen_t length = sizeof(m_svrAddr);
#endif
    SOCKET client;
    if ((client = ::accept(mSocket, (sockaddr*)&m_svrAddr, &length)) != INVALID_SOCKET) {
        if (client == INVALID_SOCKET) {
            std::print("ERROR: Could not accept client\n");
            return false;
        }

        // Detach client thread
        std::thread t(&ServerSocket::clientLoop, this, client);
        t.detach();
        return true;
    }
    return false;
}

bool ServerSocket::isRunning() {
    return m_isRunning;
}

bool ServerSocket::send(CLIENT& client, const Packet& packet) const noexcept {
    return Socket::send(packet, client);
}

int ServerSocket::receive(SOCKET socket, int millis, int maxSize) {
    // Poll
    if (!Socket::poll(socket, millis)) {
        return 0;
    }
    
    // Get packet
    CLIENT client;
    client.socket = socket;
    Packet p = Socket::_receive(client, millis, maxSize);
    if (!p.isValid()) {
        return -1;
    }
    
    // Send packet to user space to determine what to do with it
    try {
        // TCP already on its own thread, handle the packet
        if (mType == CONNECTION_TYPE::TCP) {
            m_packetLambda(client, p);
        }
        // UDP on one thread, send to threadpool
        else if (mType == CONNECTION_TYPE::UDP) {
            boost::asio::post(*m_packetPool, [this, client, p]() {
                m_packetLambda(client, p);
            });
        }
    } catch (...) {
        std::println("ERROR: uncaught exception in packet lambda.");
        return -1;
    }

    return 1;
}

bool ServerSocket::check(int millis, int maxSize) {
    // Return if TCP
    if (mType == CONNECTION_TYPE::TCP) {
        // Check for new connections
        return ServerSocket::accept(millis);
    }
    else if (mType == CONNECTION_TYPE::UDP) {
        return (ServerSocket::receive(mSocket, millis, maxSize) == 1);
    }
    return false;
}



// ----- Update -----

void ServerSocket::addReceive(std::function<void(CLIENT, Packet)> function) {
    m_packetLambda = function;
}

void ServerSocket::detach(uint64_t threads) {
    if (threads == 0) {
        m_packetPool = new boost::asio::thread_pool(std::thread::hardware_concurrency());
    }
    else {
        m_packetPool = new boost::asio::thread_pool(threads);
    }
    m_isRunning = true;
    m_serverThread = std::thread(&ServerSocket::serverLoop, this);
}

void ServerSocket::serverLoop() {
    std::print("Server running on thread {}\n", std::this_thread::get_id());
    
    // Main loop
    while (s_isRunning && m_isRunning) {
        check(100);
    }
    
    m_isRunning = false;
    std::print("Server closing on thread {}\n", std::this_thread::get_id());
}

void ServerSocket::clientLoop(SOCKET client) {
    std::print("Client added on thread {}\n", std::this_thread::get_id());
    
    // All threads are bound to main object lifetime or Ctrl+C
    while (s_isRunning && m_isRunning) {
        // Get messages until bad packet received
        if (receive(client, 100) == -1) {
            break;
        }
    }
    
    std::print("Client ending on thread {}\n", std::this_thread::get_id());
    CLOSE(client);
}

void ServerSocket::stop() {
    std::print("Shutting down server\n");
    m_isRunning = false;
}

