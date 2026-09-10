#if !defined(TIME_UTILS_HPP)
#define TIME_UTILS_HPP

#include <utility>

#include <pico/printf.h>

namespace embp
{

template <class clock_details> class timer final
{
  public:
    using time_base_t = typename std::decay_t<clock_details>::time_base_t;
    using time_diff_t = typename std::decay_t<clock_details>::time_diff_t;

    explicit constexpr timer(time_diff_t period, clock_details clock_instance)
        : m_clock{clock_instance}, m_period{period}, m_last_touch{m_clock.now()}
    {
    }

    explicit constexpr timer(time_diff_t period) : timer{period, clock_details{}}
    {
    }

    [[nodiscard]] constexpr time_diff_t time() const noexcept
    {
        return m_clock.time_diff(m_last_touch, m_clock.now());
    }

    [[nodiscard]] constexpr bool elapsed() const noexcept
    {
        return time() > m_period;
    }

    constexpr void reset() noexcept
    {
        m_last_touch = m_clock.now();
    }

    constexpr void increment() noexcept
    {
        m_last_touch = m_clock.increment_time_point(m_last_touch, m_period);
    };

    [[nodiscard]] constexpr time_diff_t period() const noexcept
    {
        return m_period;
    }
    constexpr void period(time_diff_t new_period) noexcept
    {
        m_period = new_period;
    }

  private:
    clock_details m_clock;
    time_base_t m_last_touch;
    time_diff_t m_period;
};

template <class clock_details> class pauseable_clock final
{
  public:
    using time_base_t = typename clock_details::time_base_t;
    using time_diff_t = typename clock_details::time_diff_t;

    constexpr pauseable_clock() noexcept : m_base{clock_details::now()}
    {
    }

    [[nodiscard]] constexpr time_base_t now() const noexcept
    {
        /* duration since the epoch base */
        return time_diff(m_base, clock_details::now());
    }
    [[nodiscard]] constexpr time_diff_t time_diff(time_base_t from, time_base_t to) const noexcept
    {
        return clock_details::time_diff(from, to);
    }
    [[nodiscard]] constexpr time_base_t increment_time_point(time_base_t point, time_diff_t dur) const noexcept
    {
        return clock_details::increment_time_point(point, dur);
    }

    /**
        @brief Pause time.  Useful for controlling how a game's state progresses if there are things like menus.
     */
    constexpr void pause() noexcept
    {
        m_pause_point = clock_details::now();
    }

    /**
        @brief Resume time.  All calls to now() act as if time actually stood still.
     */
    constexpr void resume() noexcept
    {
        /* advance m_base by the duration of time since the last call to pause */
        const auto nowtime{clock_details::now()};
        m_base = increment_time_point(m_base, time_diff(m_pause_point, nowtime));
    }

  private:
    time_base_t m_base;
    time_base_t m_pause_point;
};

} // namespace embp

#endif