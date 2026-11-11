#if !defined(TILE_MANIP_HPP)
#define TILE_MANIP_HPP

#include <cstdint>
#include <cstring>

namespace screen::details {

/* all of these functions have preconditions:
    1. src and dst do not overlap.
    2. pattern and replacement do NOT compare equal.
 */

inline void
copy_and_replace_1bpp([[maybe_unused]] const uint8_t *src, uint32_t len,
                      uint8_t *__restrict__ dst, uint32_t pattern,
                      [[maybe_unused]] uint32_t replacement) noexcept {
  if (pattern == 0) {
    memset(dst, 0xFF, len);
    return;
  }
  memset(dst, 0x00U, len);
}

inline void copy_and_replace_2bpp(const uint8_t *src, uint32_t len,
                                  uint8_t *__restrict__ dst, uint32_t pattern,
                                  uint32_t replacement) noexcept {
  const auto pattern_expanded{pattern | (pattern << 2) | (pattern << 4) |
                              (pattern << 6)};
  const auto replacement_expanded{replacement | (replacement << 2) |
                                  (replacement << 4) | (replacement << 6)};

  for (int ii = 0; ii < len; ++ii) {
    const auto cmp{src[ii] ^ pattern_expanded};
    if (cmp == 0) {
      dst[ii] = replacement_expanded;
    } else {
      uint8_t result{src[ii]};
      if ((cmp & 0b11) == 0) {
        result |= replacement;
      }
      if ((cmp & (0b11 << 2)) == 0) {
        result |= replacement << 2;
      }
      if ((cmp & (0b11 << 4)) == 0) {
        result |= replacement << 4;
      }
      if ((cmp & (0b11 << 6)) == 0) {
        result |= replacement << 6;
      }
      dst[ii] = result;
    }
  }
}

inline void copy_and_replace_4bpp(const uint8_t *src, uint32_t len,
                                  uint8_t *__restrict__ dst, uint32_t pattern,
                                  uint32_t replacement) noexcept {

  const auto pattern_expanded{pattern | (pattern << 4)};
  const auto replacement_expanded{replacement | (replacement << 4)};

  for (int ii = 0; ii < len; ++ii) {
    const auto cmp{src[ii] ^ pattern_expanded};
    if (cmp == 0) {
      dst[ii] = replacement_expanded;
    } else {
      if ((cmp & 0x0F) == 0) {
        /* bottom nibble compares equal */
        dst[ii] = (src[ii] & 0xF0) | replacement;
      } else if ((cmp & 0xF0) == 0) {
        /* top nibble compares equal */
        dst[ii] = (replacement << 4) | (src[ii] & 0x0F);
      } else {
        /* neither compare equal */
        dst[ii] = src[ii];
      }
    }
  }
}

inline void copy_and_replace_8bpp(const uint8_t *src, uint32_t len,
                                  uint8_t *__restrict__ dst, uint32_t pattern,
                                  uint32_t replacement) noexcept {
  for (int ii = 0; ii < len; ++ii) {
    dst[ii] = pattern == src[ii] ? static_cast<uint8_t>(replacement) : src[ii];
  }
}

inline void copy_and_replace_16bpp(const uint8_t *src, uint32_t len,
                                   uint8_t *__restrict__ dst, uint32_t pattern,
                                   uint32_t replacement) noexcept {

  const uint8_t rep_msb{static_cast<uint8_t>(replacement >> 8)};
  const uint8_t rep_lsb{static_cast<uint8_t>(replacement)};

  for (int ii = 0; ii < len; ii += 2) {

    uint16_t srcdata;
    memcpy(&srcdata, &src[ii], sizeof(uint16_t));

    if (srcdata == pattern) {
      dst[ii] = src[ii];
      dst[ii + 1] = src[ii + 1];
    } else {
      dst[ii] = rep_lsb;
      dst[ii + 1] = rep_msb;
    }
  }
}

} // namespace screen::details

#endif