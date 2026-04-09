#pragma once

#include <functional>
#include <thread>

#ifdef _SERVER_SOCKET_IMPL
#include <boost/asio/thread_pool.hpp>
#endif

#include <sockets/CLIENT.h>
#include <sockets/Packet.h>
#include <sockets/Socket.h>

/**
 * @class ServerSocket
 * @brief Creates a simple server socket, TCP or UDP.
 * @author Kyle Wagler
 * @date 2026-03-20
 */
class ServerSocket : public Socket {
    public:
    // ----- Creation ----- Destruction -----
    
    /**
     * @brief Creates a specified server.
     * @param cType The `CONNECTION_TYPE` to use.
     * @param port The port to bind to.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    ServerSocket(CONNECTION_TYPE cType, uint16_t port = DEFAULT_PORT);
    /**
     * @brief Free object memory usage.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    ~ServerSocket();
    
    // ----- Read -----
    
    /**
     * @brief Check if the server is still running
     * @return `true` if running
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool isRunning();
    
    /**
     * @brief Send data from server back to client.
     * @param client The `CLIENT` to send to.
     * @param packet The `Packet` to send.
     * @return `true` on successful send, `false` on failure to send.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool send(CLIENT& client, const Packet& packet) const noexcept;
    
    // ----- Update -----
    
    /**
     * @brief Add a lambda for handling packets.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    void addReceive(std::function<void(CLIENT, Packet)> function);
    
    /**
     * @brief Runs the server off the main thread and multithreads clients.
     * @param threads The number of threads to run the server with. 0 - auto detect system threads.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    void detach(uint64_t threads = 0);
    
    /**
     * @brief Stops the server.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    void stop();
    
private:
    // Pointer to a function that the client will call on packet receive
    sockaddr_in m_svrAddr;

    bool m_isRunning;
    std::thread m_serverThread;
    std::function<void(CLIENT, Packet)> m_packetLambda;
#ifdef _SERVER_SOCKET_IMPL
    boost::asio::thread_pool* m_packetPool;
#endif
    
    /**
     * @brief Have the server check for network updates.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool check(int millis = 0, int maxSize = MAX_BUF_SIZE);
    
    /**
     * @brief Accepts a new client on TCP.
     * @return `true` on new client.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool accept(int millis = 0);
    /**
     * @brief Receives a packet and performs packet lambda.
     * @return 1 - success. 0 - poll timeout. -1 - error.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    int receive(SOCKET socket, int millis = 0, int maxSize = MAX_BUF_SIZE);
    
    /**
     * @brief The main server loop.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    void serverLoop();
    /**
     * @brief The TCP client loop.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    void clientLoop(SOCKET client);
};

