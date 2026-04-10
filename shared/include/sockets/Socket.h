#pragma once

#include <iostream>

#include <sockets/CLIENT.h>
#include <sockets/Packet.h>

enum class CONNECTION_TYPE  { TCP, UDP };
constexpr const char* SOCKETS_VERSION   = "3.0.0";
constexpr const char* DEFAULT_IP        = "127.0.0.1";
constexpr uint16_t DEFAULT_PORT         = 34567;

/**
 * @class Socket
 * @brief The base socket class to derive from.
 * @author Kyle Wagler
 * @date 2026-03-20
 */
class Socket {
    public:
    static constexpr uint32_t MAX_BUF_SIZE  = 0xff'ff;
    
    // ----- Read -----
    
    /**
     * @brief Check that socket is properly setup before use.
     * @return true on successfully setup.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool isSetup();
    
    /**
     * @brief Poll a socket to detect packets.
     * @return true on packet found.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool poll(SOCKET socket, int millis = 0);
    
    /**
     * @brief Receives a packet from own socket.
     * @return The received packet. Returns an invalid packet if polling fails. Returns an invalid packet on receiving fail.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    // Packet receive(int millis = 0, int maxSize = MAX_BUF_SIZE);

    /**
     * @brief Sends a packet from specified socket.
     * @param data The packet to send.
     * @param ip The IP to send the packet to.
     * @param socket The socket to use to send the packet.
     * @return true on successful send.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool send(const Packet& data, CLIENT& client) const noexcept;

    /**
     * @brief Check the type of the socket.
     * @return The <code>CONNECTION_TYPE</code> of the socket.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    CONNECTION_TYPE type();
    
protected:
#ifdef _WIN32
    WSADATA mWsaData;
#endif
    CONNECTION_TYPE mType;
    uint16_t mPort;
    SOCKET mSocket;
    sockaddr_in mUdpClient;
    bool mIsSetup;
    
    // ----- Creation ----- Destruction -----
    
    Socket(CONNECTION_TYPE cType, uint16_t port);
    ~Socket();

    // ----- Read -----

    // Max size only needed for UDP connections
    Packet _receive(CLIENT& socket, int millis = 0, int maxSize = MAX_BUF_SIZE);
};

