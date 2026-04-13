#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <map>
#include <print>

#include "Sockets.h"
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

Packet generateId() {
    srand(time(nullptr));
    Packet packet;
    int32_t id = rand() & INT32_MAX;
    packet << id;
    return packet;
}

void calculateFuel(PLANE_PACKET& plane) {
    float prev = fuelValues[plane.id];
    float delta = prev - plane.fuel;
    fuelValues[plane.id] = plane.fuel;

    constexpr const char* location = "../resources/";
    constexpr const char* extension = ".csv";
    std::string filename = location + std::to_string(plane.id) + extension;
    //FILE* file = fopen(filename.c_str(), "w");
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        //std::println(file, "{}", delta);
        file << delta << "\n";
        file.close();
        //fclose(file);
    }
    else {
        std::println(stderr, "Failure: {}", location + std::to_string(plane.id) + extension);
    }
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
                Packet p = generateId();
                server.send(client, p);
                break;
            }
            case PACKET_TYPE::TEXT: {
                calculateFuel(plane);
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

