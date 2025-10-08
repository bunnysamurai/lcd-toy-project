#if !defined(TIMER_HPP)
#define TIMER_HPP

template <class TimerDetails> class Timer final {
public:
  using time_base_t = typename TimerDetails::time_base_t;
  using time_diff_t = typename TimerDetails::time_diff_t;

  explicit constexpr Timer(time_diff_t period)
      : m_period{period}, m_last_touch{TimerDetails::now()} {}

  [[nodiscard]] constexpr time_base_t time() const noexcept {
    TimerDetails::now();
  }
  [[nodiscard]] constexpr bool elapsed() const noexcept {
    return TimerDetails::time_diff(m_last_touch, TimerDetails::now()) >
           m_period;
  }
  constexpr void reset() noexcept { m_last_touch = TimerDetails::now(); }

  [[nodiscard]] constexpr time_diff_t period() const noexcept {
    return m_period;
  }
  constexpr void period(time_diff_t new_period) noexcept {
    m_period = new_period;
  }

private:
  time_base_t m_last_touch;
  time_diff_t m_period;
};

#endif