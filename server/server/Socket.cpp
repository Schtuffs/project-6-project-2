#include "Socket.h"

#include <cstring>
#include <print>

// ----- Creation ----- Destruction -----

Socket::Socket(CONNECTION_TYPE cType, uint16_t port) : mType(cType), mPort(port), mSocket(INVALID_SOCKET) {
    mIsSetup = true;
    #ifdef _WIN32
    if (WSAStartup(MAKEWORD(2,2), &mWsaData) != 0) {
        std::print("Failed to start WSA\n");
        mIsSetup = false;
        return;
    }
    #endif
    
    // Create the socket
    if (mType == CONNECTION_TYPE::TCP) {
        mSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else if (mType == CONNECTION_TYPE::UDP) {
        mSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    
    if (mSocket == INVALID_SOCKET) {
        std::print("ERROR: Failed to create socket\n");
        mIsSetup = false;
        return;
    }
}

Socket::~Socket() {
    #ifdef _WIN32
        WSACleanup();
    #endif

    // Exile the sockets
    CLOSE(mSocket);
}

bool Socket::isSetup() {
    return mIsSetup;
}



// ----- Read -----

CONNECTION_TYPE Socket::type() {
    return mType;
}

bool Socket::poll(SOCKET socket, int millis) {
    pollfd fds[1];
    fds[0].fd = socket;
    fds[0].events = POLLIN;
#ifdef _WIN32
    return (::POLL(fds, 1, millis) > 0);
#else
    int pollValue = ::POLL(fds, 1, millis);
    return (pollValue & POLLIN);
#endif
}

bool Socket::send(const Packet& packet, CLIENT& client) const noexcept {
    if (mType == CONNECTION_TYPE::TCP) {
        // Prepare buffer sized
        uint32_t size = static_cast<uint32_t>(packet.size());
        uint32_t sendSize = htonl(size);
        Packet::BUFFER* buffer = new Packet::BUFFER[size + sizeof(sendSize)];

        // Copy datapoints into buffer
        std::memcpy(buffer, &sendSize, sizeof(sendSize));
        std::memcpy(buffer + sizeof(sendSize), packet.data(), packet.size());

        // Add lengths
        int returnCode = ::send(client.socket, buffer, size + sizeof(sendSize), 0);
        delete[] buffer;
        return returnCode != SOCKET_ERROR;
    }
    else if (mType == CONNECTION_TYPE::UDP) {
        // define correct variable type for sender address length
        sockaddr_in addr;
        const auto addrLen = sizeof(addr);
        
        // Setup the address
        addr.sin_family = AF_INET;
        addr.sin_port = client.port;
        addr.sin_addr = client.ip;

        return (::sendto(client.socket, packet.data(), packet.size(), 0, (sockaddr*)&addr, addrLen) != SOCKET_ERROR);
    }
    return false;
}

Packet Socket::_receive(CLIENT& client, int millis, int maxSize) {
    // Check if receive possible
    if (!this->poll(client.socket, millis)) {
        return Packet();
    }
    
    // Receive the head first
    if (mType == CONNECTION_TYPE::TCP) {
        // Read size byte
        uint32_t size = 0;
        Packet::BUFFER sizeBuf[sizeof(size)];
        int received = ::recv(client.socket, sizeBuf, sizeof(size), 0);
        if (received != (static_cast<int>(sizeof(size)))) {
            if (received < 0) {
                std::println("ERROR: failed to receive data from socket.");
            }
            return Packet();
        }
        std::memcpy(&size, sizeBuf, sizeof(size));
        size = ntohl(size);
        
        // Read until size satisfied
        uint64_t total = 0;
        Packet::BUFFER* buf = new Packet::BUFFER[size];
        Packet::BUFFER* data = new Packet::BUFFER[size];
        while (total != size) {
            int received = ::recv(client.socket, buf, size - total, 0);
            if (received <= 0) {
                if (received < 0) {
                    std::println("ERROR: Failed to receive data from socket");
                }
                delete[] data;
                delete[] buf;
                return Packet();
            }
            std::memcpy(data + total, buf, received);
            total += received;
        }

        Packet packet(data, size);
        delete[] data;
        delete[] buf;
        return packet;
    }
    
    if (mType == CONNECTION_TYPE::UDP) {
        sockaddr_in udpClient;
        socklen_t length = sizeof(mUdpClient);

        Packet::BUFFER* buf = new Packet::BUFFER[maxSize];
        int received = ::recvfrom(client.socket, buf, maxSize, 0, (sockaddr*)&udpClient, &length);
        if (received <= 0) {
            if (received < 0) {
                std::println("ERROR: failed to receive from socket.");
            }
            delete[] buf;
            return Packet();
        }
        // Save client information
        client.ip = udpClient.sin_addr;
        client.port = udpClient.sin_port;
        
        Packet packet(buf, received);
        delete[] buf;
        return packet;
    }

    std::println(stderr, "ERROR: Failed to receive on invalid packet type.");
    return Packet();
}

