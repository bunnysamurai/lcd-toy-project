#include "embp/variable_array.hpp"
#if !defined(STATE_HPP)
#define STATE_HPP

#include "chippie/chippie_common.hpp"
#include "chippie/collision/collision.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/entities/purple_ball.hpp"
#include "collision/collision.hpp"
#include "embp/containers.hpp"
#include "static_map.hpp"
#include "terrain_effects.hpp"

#include <cstring>

namespace chippie
{

struct State
{
  public:
    void load_level() noexcept
    {
        clear_collision_list();

        load_level_from_rom_stub(level_number);
        load_entity_list_from_rom_stub(level_number);
    }

    void load_entity_list_from_rom_stub([[maybe_unused]] uint8_t level) noexcept
    {
        entity_list.clear();
        entity_list.push_back(create_purple_ball(Grid::Location{.x = 2, .y = 2}, direction::LEFT));
        entity_list.push_back(create_purple_ball(Grid::Location{.x = 2, .y = 2}, direction::LEFT));
    }

    void load_level_from_rom_stub(uint8_t level) noexcept
    {
        /* clear the map data */
        std::memset(std::data(the_map.map_data), terrain_type::CLEAR, std::size(the_map.map_data));

        /* this is literally level 1... don't worry, I'll write a tool for this kind of thing */
        the_map[Grid::Location{.x = 10, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 11, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 12, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 13, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 14, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 16, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 17, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 18, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 19, .y = 8}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 20, .y = 8}] = terrain_type::WALL;

        the_map[Grid::Location{.x = 10, .y = 9}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 12, .y = 9}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 14, .y = 9}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 15, .y = 9}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 16, .y = 9}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 18, .y = 9}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 20, .y = 9}] = terrain_type::WALL;

        the_map[Grid::Location{.x = 10, .y = 10}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 14, .y = 10}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 15, .y = 10}] = terrain_type::PORTAL;
        the_map[Grid::Location{.x = 16, .y = 10}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 20, .y = 10}] = terrain_type::WALL;

        the_map[Grid::Location{.x = 8, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 9, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 10, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 11, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 12, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 13, .y = 11}] = terrain_type::GREEN_DOOR;
        the_map[Grid::Location{.x = 14, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 15, .y = 11}] = terrain_type::SOCKET;
        the_map[Grid::Location{.x = 16, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 17, .y = 11}] = terrain_type::GREEN_DOOR;
        the_map[Grid::Location{.x = 18, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 19, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 20, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 21, .y = 11}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 22, .y = 11}] = terrain_type::WALL;

        the_map[Grid::Location{.x = 8, .y = 12}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 10, .y = 12}] = terrain_type::YELLOW_KEY;
        the_map[Grid::Location{.x = 12, .y = 12}] = terrain_type::CYAN_DOOR;
        the_map[Grid::Location{.x = 18, .y = 12}] = terrain_type::RED_DOOR;
        the_map[Grid::Location{.x = 20, .y = 12}] = terrain_type::YELLOW_KEY;
        the_map[Grid::Location{.x = 22, .y = 12}] = terrain_type::WALL;

        the_map[Grid::Location{.x = 8, .y = 13}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 10, .y = 13}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 12, .y = 13}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 13, .y = 13}] = terrain_type::CYAN_KEY;
        the_map[Grid::Location{.x = 15, .y = 13}] = terrain_type::HINT;
        the_map[Grid::Location{.x = 17, .y = 13}] = terrain_type::RED_KEY;
        the_map[Grid::Location{.x = 18, .y = 13}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 20, .y = 13}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 22, .y = 13}] = terrain_type::WALL;

        the_map[Grid::Location{.x = 8, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 9, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 10, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 11, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 12, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 13, .y = 14}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 17, .y = 14}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 18, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 19, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 20, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 21, .y = 14}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 22, .y = 14}] = terrain_type::WALL;

        the_map[Grid::Location{.x = 8, .y = 15}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 12, .y = 15}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 18, .y = 15}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 22, .y = 15}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 10, .y = 15}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 20, .y = 15}] = terrain_type::CHIP;
        the_map[Grid::Location{.x = 13, .y = 15}] = terrain_type::CYAN_KEY;
        the_map[Grid::Location{.x = 17, .y = 15}] = terrain_type::RED_KEY;

        the_map[Grid::Location{.x = 8, .y = 16}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 12, .y = 16}] = terrain_type::RED_DOOR;
        the_map[Grid::Location{.x = 18, .y = 16}] = terrain_type::CYAN_DOOR;
        the_map[Grid::Location{.x = 22, .y = 16}] = terrain_type::WALL;
        the_map[Grid::Location{.x = 15, .y = 16}] = terrain_type::CHIP;

        for (uint32_t xx = 8; xx < 23; ++xx)
        {
            if (xx != 14 && xx != 16)
            {
                the_map[Grid::Location{.x = xx, .y = 17}] = terrain_type::WALL;
            }
            else
            {
                the_map[Grid::Location{.x = xx, .y = 17}] = terrain_type::YELLOW_DOOR;
            }
        }
    }

    void process() noexcept
    {
        move_entities();

        /* TODO event processing, which may change the size of entity_list as well as make changes to the_map */
        event::process_events();

        /* entity cleanup */
        auto first_dead_itr{
            std::partition(std::begin(entity_list), std::end(entity_list), [](const auto *ent) { return ent->alive; })};
        entity_list.resize(std::distance(std::begin(entity_list), first_dead_itr));

        /* paint the entire frame */
        paint_the_map();
        paint_chip_count_display();
        paint_timer_display();
        paint_level_display();

        for (auto *ent : entity_list)
        {
            ent->paint();
        }

        if (event::exit())
        {
            return;
        }
    }

    void decrement_chip_count() noexcept
    {
        if (chip_count > 0)
        {
            --chip_count;
        }
    }

    [[nodiscard]] uint16_t get_chip_count() noexcept
    {
        return chip_count;
    }

  private:
    void move_entities() noexcept
    {
        for (auto ent : entity_list)
        {
            if (!ent.alive)
            {
                continue;
            }

            const auto id{ent.identity};
            const auto nextloc{next_loc_ftable[id](ent)};
            const collision_result collision{check_for_collision(ent, nextloc)};

            switch (collision_ftable[id](ent, collision.other, collision.tile))
            {
            case collision_action::NO_ACTION_NEEDED:
                break;
            case collision_action::APPLY_NEXT_MOVE:
                ent.loc = nextloc;
                apply_terrain_effect(ent, *collision.tile);
                break;
            case collision_action::MARK_DEAD:
                ent.alive = false;
                break;
            }
        }
    }

    [[nodiscard]] collision_result check_for_collision(const entity *this_entity, Grid::Location next_location) noexcept
    {
        collision_result result{.other = nullptr, .tile = the_map[next_location]};

        /* search for other entities */
        auto entitr = std::find(std::begin(entity_list), std::end(entity_list),
                                [&](const auto &other) { return other.loc == next_location; });
        if (entitr != std::end(entity_list))
        {
            result.other = entitr;
        }

        return result;
    }

    uint8_t level_number{1};
    Grid::Location view_port; /* top, left corner of the viewable portion of the screen */
    embp::variable_array<entity, 128> entity_list;
    uint16_t chip_count;

    static_assert(static_cast<int>(entity_type::ENTITY_TYPE_SIZE) == 9);
    /* clang-format off */
    static std::array<compute_next_fun_t, entity_type::ENTITY_TYPE_SIZE> next_loc_ftable{
        nullptr,
        nullptr,
        nullptr,
        purple_ball::compute_next_location,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };
    static std::array<handle_collision_fun_t, entity_type::ENTITY_TYPE_SIZE> collision_ftable{
        nullptr,
        nullptr,
        nullptr,
        purple_ball::handle_collision,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };
    /* clang-format on */

    static_map the_map;
};

[[nodiscard]] State &get_game_state() noexcept;

} // namespace chippie

#endif