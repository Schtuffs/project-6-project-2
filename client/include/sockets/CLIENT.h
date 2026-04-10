#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    typedef int socklen_t;
    #define POLL WSAPoll
    #define CLOSE closesocket
#else
    typedef int SOCKET;
    constexpr int INVALID_SOCKET =  -1;
    constexpr int SOCKET_ERROR =    -1;
    #define CLOSE close
    #include <unistd.h>
    #include <poll.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #define POLL poll
#endif

#include <cstdint>

/**
 * @brief Holds information for sending packets back after receiving.
 * @author Kyle Wagler
 * @date 2026-04-09
 */
typedef struct CLIENT {
    /** The socket for the connection. */
    SOCKET socket;
    /** The IP for the connection. */
    in_addr ip;
    /** The port for the connection. */
    uint16_t port;
} CLIENT;

