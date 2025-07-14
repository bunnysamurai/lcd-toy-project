#if !defined(TEXTCONSOLE_HPP)
#define TEXTCONSOLE_HPP

#include <cstdint>

namespace bsio {

class TextConsole {
public:
  TextConsole();

  void putc(char c);

private:
  static constexpr uint32_t START_COLUMN{0};
  static constexpr uint32_t START_LINE{0};

  uint32_t m_column;
  uint32_t m_line;

  [[nodiscard]] int console_column_count() const noexcept;
  [[nodiscard]] int console_line_count() const noexcept;

  void decrement_column();
  void increment_column();
  void decrement_row();
  void increment_row();
  void jump_to_previous_row();
  void jump_to_new_row();
};

} // namespace bsio
#endif
