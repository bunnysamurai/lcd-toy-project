#if !defined(CURSOR_HPP)
#define CURSOR_HPP

template <class T> struct Cursor final {
public:
  constexpr explicit Cursor(T pos_limit) noexcept
      : m_idx{0}, m_limit{pos_limit} {}

  constexpr void reset() noexcept { m_idx = 0; }

  constexpr Cursor &operator++() noexcept {
    m_idx = (m_idx < m_limit - 1) ? m_idx + 1 : m_idx;
    return *this;
  }
  constexpr Cursor operator++(int) noexcept {
    const Cursor copy{*this};
    operator++();
    return copy;
  }

  constexpr Cursor &operator--() noexcept {
    m_idx = (m_idx > 0) ? m_idx - 1 : m_idx;
    return *this;
  }
  constexpr Cursor operator--(int) noexcept {
    const Cursor copy{*this};
    operator--();
    return copy;
  }

  [[nodiscard]] constexpr operator T() const noexcept { return m_idx; }

private:
  T m_idx;
  T m_limit;
};

namespace constexpr_testing {

static constexpr bool run_test() {
  bool status{true};

  /* operator++(), operator--(), operator T() */
  {
    Cursor<unsigned int> dut{3};

    status &= dut == 0;
    ++dut;
    status &= dut == 1;
    ++dut;
    status &= dut == 2;
    ++dut;
    status &= dut == 2;
    ++dut;
    status &= dut == 2;

    --dut;
    status &= dut == 1;
    --dut;
    status &= dut == 0;
    --dut;
    status &= dut == 0;
  }
  /* operator++(int), operator--(int), operator T() */
  {
    Cursor<unsigned int> dut{3};

    status &= dut == 0;
    dut++;
    status &= dut == 1;
    dut++;
    status &= dut == 2;
    dut++;
    status &= dut == 2;
    dut++;
    status &= dut == 2;

    dut--;
    status &= dut == 1;
    dut--;
    status &= dut == 0;
    dut--;
    status &= dut == 0;
  }
  /* reset() */
  {
    Cursor<unsigned int> dut{3};

    dut++;
    dut++;
    status &= dut == 2;

    dut.reset();
    status &= dut == 0;
  }

  return status;
}
static_assert(run_test());

} // namespace constexpr_testing
#endif