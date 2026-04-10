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
     * @param cType The `CONNECTION_TYPE` to use.
     * @param ip The IP of the server to connect to in xxx.xxx.xxx.xxx format.
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

    /**
     * @brief Sends a packet to the server.
     * @param packet The `Packet` to send to the server.
     * @return `true` on successful send, `false` on failure to send
     * @author Kyle Wagler
     * @date 2026-04-09
     */
    bool send(const Packet& packet);
    
    /**
     * @brief Receives a packet from the server.
     * @param millis The time in milliseconds to check for `Packet`'s received. 0 for a non-blocking call.
     * @param maxSize The maximum size for a UDP packet. Not necessary for TCP.
     * @return The `Packet` data from the server. Returns an invalid `Packet` on failure.
     * @author Kyle Wagler
     * @date 2026-04-09
     */
    Packet receive(int millis = 0, int maxSize = MAX_BUF_SIZE);

private:
    std::string mIp;
    sockaddr_in mSvrAddr;
};

