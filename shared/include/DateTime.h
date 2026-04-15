#pragma once

#include "sockets/Packet.h"
#include <cstdint>
#include <string>
#include <ostream>

/**
 * @class DateTime
 * @brief Represents a date and time in the format MM-DD-YYYY hh:mm:ss.
 * @author Harris Ibrahimi
 * @date 2026-04-07
 */
class DateTime {
private:
    int32_t month;
    int32_t day;
    int32_t year;
    int32_t hour;
    int32_t minute;
    int32_t second;

public:
    /**
     * @brief Constructs a new blank date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    DateTime();

    /**
     * @brief Constructs a new date time value with the specified parameters.
     * @param month The month of the date time.
     * @param day The day of the date time.
     * @param year The year of the date time.
     * @param hour The hour of the date time.
     * @param minute The minutes of the date time.
     * @param second The seconds of the date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    DateTime(int32_t month, int32_t day, int32_t year, int32_t hour, int32_t minute, int32_t second);

    /**
     * @brief Constructs a new date time value from a foramtted string (MM_DD_YYYY hh:mm:ss).
     * @param dateTime The string to parse the date time from.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    DateTime(std::string dateTime);

    /**
     * @brief Gets the month value of the date time.
     * @return The month value of the date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    int32_t GetMonth() const;

    /**
     * @brief Gets the day value of the date time.
     * @return The day value of the date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    int32_t GetDay() const;

    /**
     * @brief Gets the year value of the date time.
     * @return The year value of the date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    int32_t GetYear() const;
    
    /**
     * @brief Gets the hour value of the date time.
     * @return The hour value of the date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    int32_t GetHour() const;

    /**
     * @brief Gets the minutes value of the date time.
     * @return The minutes value of the date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    int32_t GetMinute() const;

    /**
     * @brief Gets the seconds value of the date time.
     * @return The seconds value of the date time.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    int32_t GetSecond() const;

    /**
     * @brief Prints out a date time to stdout.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    friend std::ostream& operator <<(std::ostream& cout, DateTime& dateTime);

    /**
     * @brief Inserts the date time into a packet.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    friend Packet& operator <<(Packet& packet, DateTime& dateTime);

    /**
     * @brief Extracts a date time value from a packet.
     * @author Harris Ibrahimi
     * @date 2026-04-07
     */
    friend Packet& operator >>(Packet& packet, DateTime& dateTime);

};