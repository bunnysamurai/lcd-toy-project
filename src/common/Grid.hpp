#if !defined(GRID_HPP)
#define GRID_HPP

#include <cstdint>

/** @brief Overlaying a playing grid.
 *
 *  Use this class to help with overlaying a "play grid" on top of a larger 2-D
 * structure. The primary use-case for me: play grid over a section of the
 * display.
 */

struct NativeLocation {
  uint32_t x;
  uint32_t y;
};

[[nodiscard]] constexpr bool operator==(NativeLocation lhs,
                                        NativeLocation rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

class Grid {
public:
  struct Location {
    uint32_t x;
    uint32_t y;
  };
  struct MaybeLocation {
    bool is_valid;
    Location loc;
  };
  struct GridTransform {
    uint32_t off;
    uint32_t scale;
  };
  struct GridCfg {
    GridTransform xdimension;
    GridTransform ydimension;

    uint32_t grid_width;
    uint32_t grid_height;
  };

  Grid() = default;
  explicit constexpr Grid(GridCfg cfg) : m_cfg{cfg} {};

  /** @brief Grid {x,y} -> Native {x,y}
   */
  [[nodiscard]] constexpr NativeLocation
  to_native(Location grid_xy) const noexcept {
    /* TODO consider adding range checks here?  Or should it go somewhere else?
     */
    return {.x = static_cast<uint32_t>(grid_xy.x * m_cfg.xdimension.scale +
                                       m_cfg.xdimension.off),
            .y = static_cast<uint32_t>(grid_xy.y * m_cfg.ydimension.scale +
                                       m_cfg.ydimension.off)};
  }

  /** @brief Native {x,y} -> Grid {x,y}
   *
   * WARNING: Will be expensive if scaling is not a clean power-of-2.
   */
  [[nodiscard]] constexpr MaybeLocation
  to_grid(NativeLocation native_xy) const noexcept {
    if (native_xy.x < m_cfg.xdimension.off &&
        native_xy.y < m_cfg.ydimension.off) {
      return {.is_valid = false};
    }

    return {.is_valid = true,
            .loc = {.x = (native_xy.x - m_cfg.xdimension.off) /
                         m_cfg.xdimension.scale,
                    .y = (native_xy.y - m_cfg.ydimension.off) /
                         m_cfg.ydimension.scale}};
  }

  [[nodiscard]] constexpr const GridCfg &config() const noexcept {
    return m_cfg;
  }

private:
  GridCfg m_cfg;
};

[[nodiscard]] constexpr bool operator==(Grid::Location lhs,
                                        Grid::Location rhs) noexcept {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}
#endif