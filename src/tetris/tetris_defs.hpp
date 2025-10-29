#if !defined(TETRIS_DEFS_HPP)
#define TETRIS_DEFS_HPP

#include <cstdint>
#include <array>

#include "common/BitImage.hpp"

#include "screen/TileDef.h"
#include "screen/glyphs/letters.hpp"
#include "screen/screen.hpp"

namespace tetris {
inline constexpr uint32_t PLAY_NO_COLS{10};
inline constexpr uint32_t PLAY_NO_ROWS{20};
inline constexpr uint32_t SQUARE_SIZE{12};
inline constexpr int32_t INITIAL_GAME_TICK_US{1'000'000};
inline constexpr int32_t INPUT_POLL_TICK_US{10'000};

struct Location {
  uint32_t x;
  uint32_t y;
};

using ScreenLocation = Location;

struct TileTransform {
  uint32_t off;
  uint32_t scale;
};

struct TileGridCfg {
  TileTransform xdimension;
  TileTransform ydimension;

  uint32_t grid_width;
  uint32_t grid_height;
};

struct GuiCfg {
  Location scoring_box_start;
  uint8_t scoring_box_width;
  uint8_t scoring_box_height;
  Location line_score_text;
  Location piece_preview;
  uint8_t piece_preview_width;
  uint8_t piece_preview_height;
};


/* Tetriminos
 *
 * As we all know, there are seven:
 *
 *    ++++  A
 *
 *    +++   +++  B/C
 *      +   +
 *
 *    ++    ++   D/E
 *     ++  ++
 *
 *     +    ++   F and G
 *    +++   ++
 *
 */
struct Tetrimino {
public:
  /*
   *         01
   *        AB23
   *        9854
   *         76
   */
  enum struct Code : uint16_t {
    BLANK = 0b0,
    A = 0b110000001100,
    B = 0b100000011100,
    C = 0b111000000100,
    D = 0b100000110100,
    E = 0b101100000100,
    F = 0b000100110100,
    G = 0b100100100100
  };

  // Tetrimino() noexcept = default;

  explicit constexpr Tetrimino(Code init) noexcept
      : m_blocks{static_cast<uint32_t>(init)}, m_code{init} {}

  constexpr void rotate() noexcept {
    /* rotate left by 9 bits in a 12-bit word */
    /*
        9 + 12 = 21, so probably easiest to do everything in a 32-bit word

        word=init
          ****'****'****'****'****'uvwx'yzAB'CDEF
        word <<= 9
          ****'****'***u'vwxy'zABC'DEF0'0000'0000
        word |= init >> 3
          ****'****'***u'vwxy'zABC'DEFu'vwxy'zABC
    */

    const auto tmp{m_blocks};
    m_blocks <<= 9;
    m_blocks |= tmp >> 3;
    m_blocks &= 0b1111'1111'1111U;
  }

  [[nodiscard]] constexpr uint32_t data() const noexcept { return m_blocks; };
  [[nodiscard]] constexpr Code code() const noexcept { return m_code; }

private:
  uint32_t m_blocks;
  Code m_code;
};

[[nodiscard]] constexpr Tetrimino::Code
to_tetrimino_code(uint32_t value) noexcept {
  switch (value) {
  case 0:
    return Tetrimino::Code::BLANK;
  case 1:
    return Tetrimino::Code::A;
  case 2:
    return Tetrimino::Code::B;
  case 3:
    return Tetrimino::Code::C;
  case 4:
    return Tetrimino::Code::D;
  case 5:
    return Tetrimino::Code::E;
  case 6:
    return Tetrimino::Code::F;
  case 7:
    return Tetrimino::Code::G;
  }
  return Tetrimino::Code::G;
}
[[nodiscard]] constexpr uint32_t to_index(Tetrimino::Code code) noexcept {
  switch (code) {
  case Tetrimino::Code::A:
    return 1;
  case Tetrimino::Code::B:
    return 2;
  case Tetrimino::Code::C:
    return 3;
  case Tetrimino::Code::D:
    return 4;
  case Tetrimino::Code::E:
    return 5;
  case Tetrimino::Code::F:
    return 6;
  case Tetrimino::Code::G:
    return 7;
  case Tetrimino::Code::BLANK:
    return 0;
  }
  return 0;
}

/* palette and screen stuff */
inline constexpr screen::Format VIDEO_FORMAT{screen::Format::RGB565_LUT4};

inline constexpr std::array Palette{
    /* clang-format off */
    screen::Clut{.r = 0, .g = 0, .b = 0},           /* black */
    screen::Clut{.r = 255, .g = 0, .b = 0},         /* red */
    screen::Clut{.r = 0, .g = 220, .b = 0},         /* green */

    screen::Clut{.r = 0, .g = 0, .b = 255},         /* blue */
    screen::Clut{.r = 0, .g = 220, .b = 220},       /* cyan */
    screen::Clut{.r = 220, .g = 220, .b = 0},       /* yellow */

    screen::Clut{.r = 245, .g = 0, .b = 245},       /* magenta */
    screen::Clut{.r = 84, .g = 84, .b = 84},        /* dark grey */
    screen::Clut{.r = 255, .g = 255, .b = 255},     /* white */

    screen::Clut{.r = 255, .g = 128, .b = 128},     /* light red */
    screen::Clut{.r = 200, .g = 255, .b = 200},     /* light green */
    screen::Clut{.r = 128, .g = 128, .b = 255},     /* light blue */

    screen::Clut{.r = 200, .g = 255, .b = 255},     /* light cyan */
    screen::Clut{.r = 255, .g = 255, .b = 200},     /* light yellow */
    screen::Clut{.r = 255, .g = 128, .b = 255},     /* light magenta */

    screen::Clut{.r = 188, .g = 188, .b = 188},     /* light grey */
    /* clang-format on */
};

inline constexpr uint8_t BLACK{0};
inline constexpr uint8_t RED{1};
inline constexpr uint8_t GREEN{2};
inline constexpr uint8_t BLUE{3};
inline constexpr uint8_t CYAN{4};
inline constexpr uint8_t YELLOW{5};
inline constexpr uint8_t MAGENTA{6};
inline constexpr uint8_t DRKGRY{7};
inline constexpr uint8_t WHITE{8};
inline constexpr uint8_t LRED{9};
inline constexpr uint8_t LGREEN{10};
inline constexpr uint8_t LBLUE{11};
inline constexpr uint8_t LCYAN{12};
inline constexpr uint8_t LYELLOW{13};
inline constexpr uint8_t LMAGENTA{14};
inline constexpr uint8_t LGREY{15};

namespace constexpr_tests {

[[nodiscard]] constexpr bool test_tetriminos() {

  Tetrimino dut{Tetrimino::Code::A};

  bool result = dut.data() == static_cast<uint32_t>(Tetrimino::Code::A);

  dut.rotate();
  // BA9876543210
  result &= dut.data() == 0b100110000001;

  dut.rotate();
  // BA9876543210
  result &= dut.data() == 0b001100110000;

  dut.rotate();
  // BA9876543210
  result &= dut.data() == 0b000001100110;

  dut.rotate();
  // BA9876543210
  result &= dut.data() == static_cast<uint32_t>(Tetrimino::Code::A);

  return result;
}

static_assert(test_tetriminos());

[[nodiscard]] constexpr bool test_threebitimage() {
  ThreeBitImage<3, 3> dut{};

  bool result{true};

  /* first, confirm all the bits are set to 0 */
  for (uint32_t yy = 0; yy < 3; ++yy) {
    for (uint32_t xx = 0; xx < 3; ++xx) {
      result &= dut.get(xx, yy) == 0;
    }
  }

  /* second, confirm set-get pattern */

  for (uint8_t testval = 0; testval < 0b111; ++testval) {
    dut.set(testval, 2, 2);
    result &= dut.get(2, 2) == testval;
  }

  return result;
}

// static_assert(test_threebitimage());

} // namespace constexpr_tests

} // namespace tetris
#endif