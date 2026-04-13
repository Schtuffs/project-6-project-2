#include <sockets/Packet.h>

#include <cstring>
#include <print>

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
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(uint16_t val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(int32_t val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(uint32_t val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(int64_t val) {
    return push(&val, sizeof(val));
}

Packet& Packet::operator<<(uint64_t val) {
    return push(&val, sizeof(val));
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
        std::println(stderr, "ERROR: Packet could not pop to null");
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
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(uint16_t& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(int32_t& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(uint32_t& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(int64_t& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(uint64_t& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(float& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(double& val) {
    return pop(&val, sizeof(val));
}

Packet& Packet::operator>>(std::string& val) {
    val = "";
    int8_t c;

    do {
        pop(&c, sizeof(c));
        val += c;
    } while (c != '\0');

    return *this;
}

