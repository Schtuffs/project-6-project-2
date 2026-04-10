#include <print>

#include "sockets/Sockets.h"

enum class PACKET_TYPE {
    ID,
    TEXT,
};

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
                Packet p(1);
                p << (int32_t)10;
                server.send(client, packet);
                break;
            }
            case PACKET_TYPE::TEXT: {
                std::println("TEXT");
                Packet p(1);
                p << (int32_t)20;
                server.send(client, packet);
                break;
            }
            default: {
                std::println("UNKNOWN");
                Packet p(1);
                p << (int32_t)30;
                server.send(client, packet);
                break;
            }
        }
    });

    server.detach();
    while (server.isRunning()) {}
    
    return 0;
}

