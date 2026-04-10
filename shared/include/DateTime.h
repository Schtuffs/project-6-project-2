#pragma once

#include <cstdint>
#include "sockets/Packet.h"
#include <string>
#include <ostream>

class DateTime {
private:
    int32_t month;
    int32_t day;
    int32_t year;
    int32_t hour;
    int32_t minute;
    int32_t second;

public:
    DateTime();
    DateTime(int32_t month, int32_t day, int32_t year, int32_t hour, int32_t minute, int32_t second);
    DateTime(std::string dateTime);

    int32_t GetMonth() const;
    int32_t GetDay() const;
    int32_t GetYear() const;
    int32_t GetHour() const;
    int32_t GetMinute() const;
    int32_t GetSecond() const;

    friend std::ostream& operator <<(std::ostream& cout, DateTime& dateTime);
    friend Packet& operator <<(Packet& packet, DateTime& dateTime);
    friend Packet& operator >>(Packet& packet, DateTime& dateTime);

};