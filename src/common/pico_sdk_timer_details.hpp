#if !defined(PICO_SDK_TIMER_DETAILS_HPP)
#define PICO_SDK_TIMER_DETAILS_HPP

#include <cstdint>

#include "pico/time.h"

namespace timer_details
{

struct PicoSdk
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
} // namespace timer_details

#endif