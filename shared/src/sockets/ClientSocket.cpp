#include <sockets/ClientSocket.h>

#include <print>

// ----- Creation ----- Destruction -----

ClientSocket::ClientSocket(CONNECTION_TYPE cType, const std::string& ip, uint16_t port) : Socket(cType, port), mIp(ip) {
    // Check socket creation
    if (!mIsSetup) {
        return;
    }

    // Prepare server info
    mSvrAddr.sin_family = AF_INET;
    mSvrAddr.sin_port = htons(mPort);
    mSvrAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // Only TCP needs connect
    if (mType == CONNECTION_TYPE::TCP) {
        // Connect to server
        if (connect(mSocket, (sockaddr*)&mSvrAddr, sizeof(mSvrAddr)) != 0) {
            std::print("ERROR: Failed to connect client to server\n");
            mIsSetup = false;
            return;
        }
    }

    std::print("{} client sending to IP \"{}\" at port {}\n", (mType == CONNECTION_TYPE::TCP ? "TCP" : "UDP"), mIp, mPort);
}

ClientSocket::~ClientSocket() {
    // Nothing todo
}

bool ClientSocket::send(const Packet& packet) {
    CLIENT server;
    server.socket = mSocket;
    server.ip = mSvrAddr.sin_addr;
    server.port = htons(mPort);
    return Socket::send(packet, server);
}

Packet ClientSocket::receive(int millis, int maxSize) {
    CLIENT server;
    server.socket = mSocket;
    return Socket::_receive(server, millis, maxSize);
}

