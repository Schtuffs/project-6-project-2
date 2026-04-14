#include <sockets/Packet.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

#include <array>
#include <cmath>
#include <cstring>
#include <print>

template <typename IntegerType, typename... Bytes>
[[nodiscard]] constexpr IntegerType toInteger(Bytes... byte)
{
    static_assert(sizeof(IntegerType) >= sizeof...(Bytes), "IntegerType not large enough to contain bytes");

    IntegerType integer = 0;
    std::size_t index   = 0;
    return ((integer |= static_cast<IntegerType>(static_cast<IntegerType>(byte) << 8 * index++)), ...);
}

// ----- Creation ----- Destruction -----

Packet::Packet(uint64_t size) : m_buffer(nullptr), m_writePointer(0), m_readPointer(0), m_size(size) {
    if (m_size < 1 && m_size != PACKET_INVALID) {
        m_size = 1;
    }

    if (m_size != PACKET_INVALID) {
        m_buffer = new BUFFER[m_size];
    }
}

Packet::Packet(const Packet::BUFFER* buf, uint64_t size) : m_buffer(nullptr), m_writePointer(0), m_readPointer(0), m_size(size) {
    if (!buf) {
        throw std::string("PACKET ERROR: buffer was null");
    }
    if (m_size < 1 && m_size != PACKET_INVALID) {
        m_size = 1;
    }
    
    if (m_size != PACKET_INVALID) {
        m_buffer = new BUFFER[m_size];
        std::memcpy(m_buffer, buf, m_size);
        m_writePointer = m_size;
    }
}

Packet::Packet(const Packet& other) : m_buffer(nullptr), m_writePointer(other.m_writePointer), m_readPointer(0), m_size(other.m_size) {
    if (m_size != PACKET_INVALID) {
        m_buffer = new BUFFER[m_size];
        std::memcpy(m_buffer, other.m_buffer, m_writePointer);
    }
}

Packet::~Packet() {
    delete[] m_buffer;
}



// ----- Read -----

Packet::BUFFER* Packet::data() const noexcept {
    return m_buffer;
}

uint64_t Packet::size() const noexcept {
    return m_writePointer;
}

bool Packet::isValid() const noexcept {
    return m_size != PACKET_INVALID;
}



// ----- Update -----

bool Packet::reallocate() {
    // Create valid allocation for invalid packet
    if (m_buffer == nullptr) {
        m_size = 1;
        m_buffer = new BUFFER[m_size];
        return true;
    }
    
    // Prepare new allocation space
    uint64_t newSize = std::ceil((m_size * 1.5) + 1);
    BUFFER* newBuf = new BUFFER[newSize];
    if (!newBuf) {
        std::println(stderr, "PACKET ERROR: failed to allocate a larger buffer");
        return false;
    }

    // Copy the data and change pointers
    std::memcpy(newBuf, m_buffer, m_writePointer);
    delete[] m_buffer;
    m_buffer = newBuf;
    m_size = newSize;
    return true;
}



// ----- INSERTIONS -----

Packet& Packet::push(const void* data, uint64_t size) {
    // Revalidate invalid packet
    if (m_buffer == nullptr) {
        if (!reallocate()) {
            return *this;
        }
    }

    // Get enough size for packet data
    while (m_writePointer + size > m_size) {
        if (!reallocate()) {
            return *this;
        }
    }

    // Add new data and update pointer
    std::memcpy(m_buffer + m_writePointer, data, size);
    m_writePointer += size;
    return *this;
}

Packet& Packet::operator<<(int8_t val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(uint8_t val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(int16_t val) {
    val = static_cast<int16_t>(htons(static_cast<uint16_t>(val)));
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(uint16_t val) {
    val = htons(val);
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(int32_t val) {
    val = static_cast<int32_t>(htonl(static_cast<uint32_t>(val)));
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(uint32_t val) {
    val = htonl(val);
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(int64_t val) {
    // Taken from SFML
    const std::array toWrite = {static_cast<std::uint8_t>((val >> 56) & 0xFF),
                                static_cast<std::uint8_t>((val >> 48) & 0xFF),
                                static_cast<std::uint8_t>((val >> 40) & 0xFF),
                                static_cast<std::uint8_t>((val >> 32) & 0xFF),
                                static_cast<std::uint8_t>((val >> 24) & 0xFF),
                                static_cast<std::uint8_t>((val >> 16) & 0xFF),
                                static_cast<std::uint8_t>((val >>  8) & 0xFF),
                                static_cast<std::uint8_t>((val)       & 0xFF)};

    return push(toWrite.data(), toWrite.size());
}

Packet& Packet::operator<<(uint64_t val) {
    // Taken from SFML
    const std::array toWrite = {static_cast<std::uint8_t>((val >> 56) & 0xFF),
                                static_cast<std::uint8_t>((val >> 48) & 0xFF),
                                static_cast<std::uint8_t>((val >> 40) & 0xFF),
                                static_cast<std::uint8_t>((val >> 32) & 0xFF),
                                static_cast<std::uint8_t>((val >> 24) & 0xFF),
                                static_cast<std::uint8_t>((val >> 16) & 0xFF),
                                static_cast<std::uint8_t>((val >>  8) & 0xFF),
                                static_cast<std::uint8_t>((val)       & 0xFF)};

    return push(toWrite.data(), toWrite.size());
}

Packet& Packet::operator<<(float val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(double val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(const std::string& val) {
    return push(val.c_str(), val.length() + 1);
}



// ----- REMOVALS -----

Packet& Packet::pop(void* data, uint64_t size) {
    if (!data) {
        std::println(stderr, "ERROR: Packet could not pop to null.");
        return *this;
    }

    if (!m_buffer) {
        std::println(stderr, "ERROR: Packet could not pop from null.");
        return *this;
    }

    if (m_readPointer + size > m_size) {
        std::println(stderr, "ERROR: Packet reading more data than it contains.");
        return *this;
    }

    std::memcpy(data, m_buffer + m_readPointer, size);
    m_readPointer += size;
    return *this;
}

Packet& Packet::operator>>(int8_t& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(uint8_t& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(int16_t& val) {
    pop(&val, sizeof(val));
    val = static_cast<int16_t>(ntohs(static_cast<uint16_t>(val)));
    return *this;
}

Packet& Packet::operator>>(uint16_t& val) {
    pop(&val, sizeof(val));
    val = ntohs(val);
    return *this;
}

Packet& Packet::operator>>(int32_t& val) {
    pop(&val, sizeof(val));
    val = static_cast<int32_t>(ntohs(static_cast<uint32_t>(val)));
    return *this;
}

Packet& Packet::operator>>(uint32_t& val) {
    pop(&val, sizeof(val));
    val = ntohl(val);
    return *this;
}

Packet& Packet::operator>>(int64_t& val) {
    std::array<std::byte, sizeof(val)> bytes{};
    pop(bytes.data(), bytes.size());
    
    val = toInteger<int64_t>(bytes[7], bytes[6], bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0]);
    return *this;
}

Packet& Packet::operator>>(uint64_t& val) {
    std::array<std::byte, sizeof(val)> bytes{};
    pop(bytes.data(), bytes.size());

    val = toInteger<uint64_t>(bytes[7], bytes[6], bytes[5], bytes[4], bytes[3], bytes[2], bytes[1], bytes[0]);
    return *this;
}

Packet& Packet::operator>>(float& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(double& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(std::string& val) {
    val = "";
    int8_t c = 0;

    do {
        pop(&c, sizeof(c));
        val += c;
    } while (c != '\0');

    return *this;
}

