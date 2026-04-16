#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <map>
#include <mutex>
#include <print>
#include <utility>

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

static std::map<int32_t, std::pair<float, FILE*>> fuelValues;
static std::mutex mtx;

Packet generateId() {
    Packet packet;
    int32_t id = std::this_thread::get_id()._Get_underlying_id();
    packet << id;

    mtx.lock();
    fuelValues[id].first = 0;
    fuelValues[id].second = nullptr;
    mtx.unlock();

    return packet;
}

void calculateFuel(PLANE_PACKET& plane) {
    auto& data = fuelValues[plane.id];
    float prev = data.first;
    float delta = prev - plane.fuel;
    data.first = plane.fuel;

    if (!data.second) {
        constexpr const char* location = "../resources/";
        constexpr const char* extension = ".csv";
        std::string filename = location + std::to_string(plane.id) + extension;
        FILE* file = fopen(filename.c_str(), "w");
        if (file) {
            data.second = file;
        }
        else {
            std::println(stderr, "Failure: {}", location + std::to_string(plane.id) + extension);
            return;
        }
    }

    std::println(data.second, "{:.6}", delta);
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
                throw 1;
                break;
            }
        }
    });

    server.addClientClose([]() {
        fclose(fuelValues[std::this_thread::get_id()._Get_underlying_id()].second);
        mtx.lock();
        fuelValues.erase(std::this_thread::get_id()._Get_underlying_id());
        mtx.unlock();
    });

    server.detach();
    while (server.isRunning()) {}
    
    return 0;
}

