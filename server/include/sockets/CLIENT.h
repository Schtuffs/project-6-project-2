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

typedef struct CLIENT {
    SOCKET socket;
    in_addr ip;
    uint16_t port;
} CLIENT;

