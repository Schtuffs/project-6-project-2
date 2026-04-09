#pragma once

#include <cstdint>
#include <cmath>
#include <iostream>
#include <string>

#include <sockets/Array.h>

/**
 * @class Packet
 * @brief The default packet class.
 * @author Kyle Wagler
 * @date 2026-03-20
 */
class Packet {
public:
    static constexpr uint64_t PACKET_INVALID   = 0xffffffffffffffff;

    typedef char BUFFER;

    // ----- Creation ----- Destruction -----

    /**
     * @brief Prepares a new packet.
     * @param initialSize A starting size for packet creation. Default is -1 indicating invalid packet.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    Packet(uint64_t initialSize = PACKET_INVALID);
    /**
     * @brief Duplicates another packet.
     * @param other The packet to duplicate.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    Packet(const Packet& other);
    /**
     * @brief Creates packet with given buffer data.
     * @param data The data buffer to copy.
     * @param size The size of the data buffer.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    Packet(const BUFFER* data, uint64_t size);
    /**
     * @brief Free the memory.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    ~Packet();
    
    // ----- Read -----
    
    /**
     * @brief Returns the internal buffer of the packaged packet.
     * @return The packets data buffer. Not recommended to change manually.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    BUFFER* data() const noexcept;
    
    /**
     * @brief Returns the total size of the packet.
     * @return The total packet size.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    uint64_t size() const noexcept;
    
    /**
     * @brief Checks the validity of the packet.
     * @return `true` on valid packet.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    bool isValid() const noexcept;

    // ----- Update -----

    /**
     * @brief This function is used to add data into the packet.
     * @param data A pointer to the data.
     * @param size The total byte size of the data.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    Packet& push(const void* data, uint64_t size);
    Packet& operator<<(int8_t val);
    Packet& operator<<(uint8_t val);
    Packet& operator<<(int16_t val);
    Packet& operator<<(uint16_t val);
    Packet& operator<<(int32_t val);
    Packet& operator<<(uint32_t val);
    Packet& operator<<(int64_t val);
    Packet& operator<<(uint64_t val);
    Packet& operator<<(float val);
    Packet& operator<<(double val);
    Packet& operator<<(const std::string& val);
    
    /**
     * @brief This function is used to get data from the packet.
     * @param data A pointer to the data.
     * @param size The total byte size of the data.
     * @author Kyle Wagler
     * @date 2026-03-20
     */
    Packet& pop(void* data, uint64_t size);
    Packet& operator>>(int8_t& val);
    Packet& operator>>(uint8_t& val);
    Packet& operator>>(int16_t& val);
    Packet& operator>>(uint16_t& val);
    Packet& operator>>(int32_t& val);
    Packet& operator>>(uint32_t& val);
    Packet& operator>>(int64_t& val);
    Packet& operator>>(uint64_t& val);
    Packet& operator>>(float& val);
    Packet& operator>>(double& val);
    Packet& operator>>(std::string& val);
    
private:
    BUFFER* m_buffer;
    uint64_t m_writePointer, m_readPointer, m_size;

    bool reallocate();
};

