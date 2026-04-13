#include <print>

#include "sockets/Sockets.h"
#include "datetime.hpp"

enum class PACKET_TYPE {
    ID,
    TEXT,
};

typedef struct PLANE_PACKET {
    int32_t type;
    int32_t id;
    DateTime time;
    float fuel;
    int32_t requestID;
} PLANE_PACKET;

Packet generateId() {
    Packet packet;
    packet << 12;
    return packet;
}

int main(void) {
    ServerSocket server(CONNECTION_TYPE::TCP);
    if (!server.isSetup()) {
        std::println(stderr, "ERROR: Failed to setup server.");
        return 1;
    }

    server.addReceive([&](CLIENT client, Packet packet) {
        int32_t intType;
        packet >> intType;
        PACKET_TYPE type = static_cast<PACKET_TYPE>(intType);

        // Determine what to do with packet type
        switch (type) {
            case PACKET_TYPE::ID: {
                std::println("ID");
                Packet p = generateId();
                server.send(client, p);
                break;
            }
            case PACKET_TYPE::TEXT: {
                std::println("TEXT");
                Packet p;
                p << (int32_t)20;
                server.send(client, p);
                break;
            }
            default: {
                std::println("UNKNOWN");
                Packet p;
                p << (int32_t)30;
                server.send(client, p);
                break;
            }
        }
    });

    server.detach();
    while (server.isRunning()) {}
    
    return 0;
}

