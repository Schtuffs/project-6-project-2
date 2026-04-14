#include "DateTime.h"
#include "sockets/Sockets.h"
#include <boost/algorithm/string/classification.hpp>
#include <cstdint>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <print>
#include <chrono>

using namespace std::chrono_literals;

#define TELEMETRY_FILE "../resources/katl-kefd-B737-700.txt"

enum class PacketType {
    ID,
    Text
};

int main(int argc, char** argv) {
    int32_t clientID = -1;

    ClientSocket* clientSocket = nullptr;
    if (argc >= 2) {
        clientSocket = new ClientSocket(CONNECTION_TYPE::TCP, argv[1]);
    }
    else {
        clientSocket = new ClientSocket(CONNECTION_TYPE::TCP);
    }
    if (!clientSocket->isSetup()) {
        return EXIT_FAILURE;
    }

    Packet idPacket;
    DateTime d;
    idPacket << (int32_t)PacketType::ID << 3 << d << 1.3f;
    clientSocket->send(idPacket);

    Packet startupPacket = clientSocket->receive(200);
    if (!startupPacket.isValid()) {
        std::println(stderr, "ERROR: Failed to receive valid id.");
        return EXIT_FAILURE;
    }

    startupPacket >> clientID;
    std::println("Client id: {}", clientID);
    
    std::ifstream telemetryFile = std::ifstream(TELEMETRY_FILE);
    if (!telemetryFile.is_open()) {
        std::println("Failed to open file: {}", TELEMETRY_FILE);
        return EXIT_FAILURE;
    }

    uint64_t packetSize = sizeof(int32_t) * 3 + sizeof(DateTime);
    std::string line = "";
    while (std::getline(telemetryFile, line)) {
        std::println("{}", line);

        std::vector<std::string> telemetry = std::vector<std::string>();
        boost::trim_if(line, boost::is_any_of(" \n"));
        if (line.empty()) {
            continue;
        }

        boost::split(telemetry, line, boost::is_any_of(","));

        telemetry.pop_back();
        float remainingFuel = std::stof(telemetry.back());
        telemetry.pop_back();

        DateTime dateTime = DateTime(telemetry.back());

        Packet packet = Packet(packetSize);
        packet << (int32_t)PacketType::Text << clientID << dateTime << remainingFuel;
        clientSocket->send(packet);
    }

    std::this_thread::sleep_for(1s);
    delete clientSocket;
    return 0;
}

