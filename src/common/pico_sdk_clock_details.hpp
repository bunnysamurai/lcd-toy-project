#if !defined(PICO_SDK_CLOCK_DETAILS_HPP)
#define PICO_SDK_CLOCK_DETAILS_HPP

#include <cstdint>

#include "pico/time.h"

namespace clock_details
{

/* TODO standard API for my clocks... think about utilizing the C++ library chrono? */
struct pico_sdk_steady_clock 
{
    using time_base_t = absolute_time_t;
    using time_diff_t = int64_t;

    [[nodiscard]] static time_base_t now() noexcept
    {
        return get_absolute_time();
    }
    [[nodiscard]] static time_diff_t time_diff(time_base_t from, time_base_t to) noexcept
    {
        return absolute_time_diff_us(from, to);
    }
    [[nodiscard]] static time_base_t increment_time_point(time_base_t point, time_diff_t dur) noexcept
    {
        return delayed_by_us(point, dur);
    }
};
} // namespace clock_details

#endif