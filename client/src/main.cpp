#include "DateTime.h"
#include "sockets/ClientSocket.h"
#include "sockets/Packet.h"
#include "sockets/Sockets.h"
#include <boost/algorithm/string/classification.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <vector>

#define TELEMETRY_FILE "../resources/katl-kefd-B737-700.txt"

enum class PacketType {
    ID,
    Text
};

int main(void) {
    int32_t clientID = -1;

    ClientSocket clientSocket = ClientSocket(CONNECTION_TYPE::TCP, "10.144.115.247");
    if (!clientSocket.isSetup()) {
        std::cout << "Socket not setup unfortunately" << std::endl;
        return EXIT_FAILURE;
    }

    Packet idPacket(sizeof(int32_t));
    idPacket << (int32_t)PacketType::ID;
    clientSocket.send(idPacket);

    Packet startupPacket = clientSocket.receive(0, sizeof(int32_t));
    if (!startupPacket.isValid()) {
        std::cout << "Failed to receive startup packet containing client ID." << std::endl;
        return EXIT_FAILURE;
    }

    int stub = -1;
    startupPacket >> stub;
    startupPacket >> clientID;
    std::cout << "Client id: " << clientID << std::endl;
    
    std::ifstream telemetryFile = std::ifstream(TELEMETRY_FILE);
    if (!telemetryFile.is_open()) {
        std::cout << "Failed to open file: " << TELEMETRY_FILE << std::endl;
        return EXIT_FAILURE;
    }

    std::string line = "";
    while (std::getline(telemetryFile, line)) {
        std::vector<std::string> telemetry = std::vector<std::string>();
        boost::trim_if(line, boost::is_any_of(" "));
        boost::split(telemetry, line, boost::is_any_of(","));
        
        float remainingFuel = std::stof(telemetry.back());
        telemetry.pop_back();
        
        DateTime dateTime = DateTime(telemetry.back());
        
        Packet packet = Packet(sizeof(int32_t) + sizeof(float) + sizeof(DateTime));
        packet << (int32_t)PacketType::Text << dateTime << remainingFuel;

        clientSocket.send(packet);
    }

    return 0;
}

