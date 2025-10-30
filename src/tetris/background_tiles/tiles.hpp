#ifndef TETRIS_BKGRN_TILES_HPP
#define TETRIS_BKGRN_TILES_HPP

#include <array>

#include "tetris_bkgrnd_lvl_1.hpp"
#include "tetris_bkgrnd_lvl_2.hpp"
#include "tetris_bkgrnd_lvl_3.hpp"
#include "tetris_bkgrnd_lvl_4.hpp"
// #include "tetris_bkgrnd_lvl_5.hpp"
// #include "tetris_bkgrnd_lvl_6.hpp"
// #include "tetris_bkgrnd_lvl_7.hpp"
// #include "tetris_bkgrnd_lvl_8.hpp"
// #include "tetris_bkgrnd_lvl_9.hpp"
// #include "tetris_bkgrnd_lvl_10.hpp"

namespace tetris {

// clang-format off
inline constexpr std::array BACKGROUND_TILES{
    TETRIS_BKGRND_LVL_1_TILE,
    TETRIS_BKGRND_LVL_2_TILE,
    TETRIS_BKGRND_LVL_3_TILE,
    TETRIS_BKGRND_LVL_4_TILE,
    // TETRIS_BKGRND_LVL_5_TILE,
    // TETRIS_BKGRND_LVL_6_TILE,
    // TETRIS_BKGRND_LVL_7_TILE,
    // TETRIS_BKGRND_LVL_8_TILE,
    // TETRIS_BKGRND_LVL_9_TILE,
    // TETRIS_BKGRND_LVL_10_TILE,
};
// clang-format on

}

#endif