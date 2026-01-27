#pragma once

#include<cstdint>

namespace hgl
{
    union TimeID
    {
        uint64_t time_id;

        struct
        {
            unsigned int year:12;           ///< Year (0-4095)
            unsigned int month:4;           ///< Month (1-12)
            unsigned int day:5;             ///< Day (1-31)
            unsigned int hour:5;            ///< Hour (0-23)
            unsigned int minute:6;          ///< Minute (0-59)
            unsigned int second:6;          ///< Second (0-59)
            unsigned int micro_second:20;   ///< Microsecond (0-999999)
        };
    };

    constexpr const size_t TimeIDBytes=sizeof(TimeID);

    TimeID GetCurrentTimeID();
}// namespace hgl
