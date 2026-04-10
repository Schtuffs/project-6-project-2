#include <map>
#include <print>

#include "sockets/Sockets.h"
#include "DateTime.h"

enum class PACKET_TYPE {
    ID,
    TEXT,
};

typedef struct PLANE_PACKET {
    int32_t type;
    int32_t id;
    DateTime time;
    float fuel;

    friend Packet& operator>>(Packet& packet, PLANE_PACKET& plane) {
        packet >> plane.type >> plane.id >> plane.time >> plane.fuel;
        return packet;
    }
} PLANE_PACKET;

std::map<int32_t, float> fuelValues;

Packet generateId(PLANE_PACKET& plane) {
    Packet packet;
    std::println("ID: {}", plane.id);
    packet << (int32_t)plane.id;
    return packet;
}

int main(void) {
    ServerSocket server(CONNECTION_TYPE::TCP);
    if (!server.isSetup()) {
        std::println(stderr, "ERROR: Failed to setup server.");
        return 1;
    }

    server.addReceive([&](CLIENT client, Packet packet) {
        PLANE_PACKET plane;
        packet >> plane;

        // Determine what to do with packet type
        switch (static_cast<PACKET_TYPE>(plane.type)) {
            case PACKET_TYPE::ID: {
                std::println("ID");
                Packet p = generateId(plane);
                server.send(client, p);
                break;
            }
            case PACKET_TYPE::TEXT: {

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

