#pragma once

#include <sockets/Socket.h>

/**
 * @class ClientSocket
 * @brief Allows easy creation of a client socket
 * @author Kyle Wagler
 * @date 2026-03-20
 */
class ClientSocket : public Socket {
public:
    // ----- Creation ----- Destruction -----

    /**
     * @brief Creates a specified client.
     * @param cType The <code>CONNECTION_TYPE</code> to use.
     * @param ip The IP of the server to connect to.
     * @param port The port of the server to connect to.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    ClientSocket(CONNECTION_TYPE cType, const std::string& ip = DEFAULT_IP, uint16_t port = DEFAULT_PORT);
    /**
     * @brief Deconstructs the client
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    ~ClientSocket();

    bool send(const Packet& packet);
    Packet receive(int millis = 0, int maxSize = MAX_BUF_SIZE);

private:
    std::string mIp;
    sockaddr_in mSvrAddr;
};

