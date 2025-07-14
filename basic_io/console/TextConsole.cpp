#include "TextConsole.hpp"

#include "../bsio.hpp"
#include "glyphs/letters.hpp"
#include <array>
#include <cstddef>

namespace {

constexpr bool check_if_printable(const char c) {
  // defintely not printable if bits 6 and 5 are unset, if all bit are set, or
  // if only bit 5 is set
  //  ASCII is not arbitrarily layed out, it would seem
  //  (https://en.wikipedia.org/wiki/ASCII).
  // or, if c > 31 and c < 127
  return c > 31 && c < 127;
}
constexpr bool check_if_newline(const char c) { return c == '\n' || c == '\r'; }
constexpr bool check_if_tab(const char c) { return c == '\t'; }
constexpr bool check_if_null(const char ch) { return ch == '\0'; }
constexpr bool check_if_backspace(const char ch) { return ch == 0x08; }

} // namespace

namespace bsio {

int TextConsole::console_column_count() const noexcept {
  const auto dims{get_console_width_and_height()};
  return dims.width;
}
int TextConsole::console_line_count() const noexcept {
  const auto dims{get_console_width_and_height()};
  return dims.height;
}
void TextConsole::decrement_column() {
  if (m_column == 1) {
    jump_to_previous_row();
  } else {
    --m_column;
  }
}
void TextConsole::increment_column() {
  if (m_column == console_column_count() - 1) {
    jump_to_new_row();
  } else {
    ++m_column;
  }
}
void TextConsole::decrement_row() {
  if (m_line > 0) {
    --m_line;
  }
}
void TextConsole::increment_row() {
  if (m_line == console_line_count() - 1) {
    scroll_up(1);
  } else {
    ++m_line;
  }
}
void TextConsole::jump_to_previous_row() {
  decrement_row();
  m_column = console_column_count() - 1;
}
void TextConsole::jump_to_new_row() {
  increment_row();
  m_column = 0;
}

TextConsole::TextConsole() : m_column{START_COLUMN}, m_line{START_LINE} {}

void TextConsole::clear() {
  clear_console();
  m_column = START_COLUMN;
  m_line = START_LINE;
}

void TextConsole::putc(char c) {
  if (check_if_printable(c)) {
    draw_letter(m_column, m_line, c);
    increment_column();
    draw_letter(m_column, m_line, '_');
    return;
  }
  if (check_if_newline(c)) {
    draw_letter(m_column, m_line, ' ');
    jump_to_new_row();
    draw_letter(m_column, m_line, '_');
    return;
  }
  if (check_if_tab(c)) {
    draw_letter(m_column, m_line, ' ');
    increment_column();
    draw_letter(m_column, m_line, ' ');
    increment_column();
    draw_letter(m_column, m_line, '_');
    return;
  }
  if (check_if_backspace(c)) {
    draw_letter(m_column, m_line, ' ');
    decrement_column();
    draw_letter(m_column, m_line, '_');
  }
}

} // namespace bsio