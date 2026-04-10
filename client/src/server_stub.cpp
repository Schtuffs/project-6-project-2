#include "sockets/Sockets.h"

enum class PacketType {
    ID,
    Text
};

int main() {
    ServerSocket serverSocket = ServerSocket(CONNECTION_TYPE::TCP);
    if (!serverSocket.isSetup()) {
        std::cout << "server destroyed" << std::endl;
        return EXIT_FAILURE;
    }

    int idPool = 1;

    serverSocket.addReceive([&serverSocket, &idPool](CLIENT client, Packet packet) {
        if (packet.isValid()) {
            int packetTypeInt = -1;
            packet >> packetTypeInt;
            PacketType packetType = (PacketType)packetTypeInt;

            switch (packetType) {
                case PacketType::ID: {
                    Packet sendingPacket = Packet(sizeof(PacketType));
                    sendingPacket << (int32_t)PacketType::ID;
                    serverSocket.send(client, sendingPacket);
                    idPool++;
                }
                break;

                case PacketType::Text: {
                    
                }
                break;
            }
        }
    });

    serverSocket.detach();
    while (serverSocket.isRunning()) {
        
    }

    return 0;
}