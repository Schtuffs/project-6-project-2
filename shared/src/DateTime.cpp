#include "DateTime.h"
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include <boost/algorithm/string.hpp>

DateTime::DateTime(int32_t month, int32_t day, int32_t year, int32_t hour, int32_t minute, int32_t second) {
    this-> month = month;
    this->day = day;
    this->year = year;
    this->hour = hour;
    this->minute = minute;
    this->second = second;
}

DateTime::DateTime(std::string dateTime) {
    std::vector<std::string> dateTimeValues = std::vector<std::string>();
    boost::split(dateTimeValues, dateTime, boost::is_any_of("_ :"));

    if (dateTimeValues.size() != 6) {
        std::cout << "Invalid date time string." << std::endl;
        return;
    }

    month = std::stoi(dateTimeValues[0]);
    day = std::stoi(dateTimeValues[1]);
    year = std::stoi(dateTimeValues[2]);
    hour = std::stoi(dateTimeValues[3]);
    minute = std::stoi(dateTimeValues[4]);
    second = std::stoi(dateTimeValues[5]);
}

int32_t DateTime::GetMonth() const {
    return month;
}

int32_t DateTime::GetDay() const {
    return day;
}

int32_t DateTime::GetYear() const {
    return year;
}

int32_t DateTime::GetHour() const {
    return hour;
}

int32_t DateTime::GetMinute() const {
    return minute;
}

int32_t DateTime::GetSecond() const {
    return second;
}

Packet& operator <<(Packet& packet, DateTime& dateTime) {
    packet << dateTime.month << dateTime.day << dateTime.year << dateTime.hour << dateTime.minute << dateTime.second;
    return packet;
}

Packet& operator >>(Packet& packet, DateTime& dateTime) {
    packet >> dateTime.month >> dateTime.day >> dateTime.year >> dateTime.hour >> dateTime.minute >> dateTime.second;
    return packet;
}