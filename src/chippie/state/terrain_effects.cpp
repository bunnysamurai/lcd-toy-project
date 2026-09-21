#include "terrain_effects.hpp"

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/events/event.hpp"
#include "chippie/events/event_types.hpp"
#include "chippie/state/red_brown_buttons.hpp"
#include "chippie/state/terrain_types.hpp"
#include "chippie_inventory.hpp"

#include <algorithm>

// #define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include "pico/printf.h"
#endif

namespace chippie
{
namespace
{
/**
    @brief Check for terrain that is ALWAYS opaque for chippie and requires no side-effects.  Can be facing dependent
   (aka thin walls).
 */
[[nodiscard]] bool check_terrain_is_opaque_for_chippie(const entity &ent, terrain_type terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::WALL:
    case terrain_type::INVISIBLE_WALL:
    case terrain_type::GREEN_BUTTON_WALL:
    case terrain_type::CLONER_FIRE_DANCER:
    case terrain_type::CLONER_MOVEABLE_BLOCK:
    case terrain_type::CLONER_FROG_MONSTER:
    case terrain_type::CLONER_BLUE_TANK_THAT_MOVES_LEFT:
    case terrain_type::CLONER_CENTIPEDE:
    case terrain_type::CLONER_PURPLE_BALL:
    case terrain_type::CLONER_CYAN_STICK_BALL:
        return true;

    case terrain_type::THIN_WALL_TOP:
        return ent.facing == direction::DOWN;
    case terrain_type::THIN_WALL_BOT:
        return ent.facing == direction::UP;
    case terrain_type::THIN_WALL_LEFT:
        return ent.facing == direction::RIGHT;
    case terrain_type::THIN_WALL_RIGHT:
        return ent.facing == direction::LEFT;
    case terrain_type::THIN_WALL_BOTRIGHT:
        return ent.facing == direction::LEFT || ent.facing == direction::UP;

    case terrain_type::ICE_TOPLEFT:
        return ent.facing == direction::DOWN || ent.facing == direction::RIGHT;
    case terrain_type::ICE_TOPRIGHT:
        return ent.facing == direction::DOWN || ent.facing == direction::LEFT;
    case terrain_type::ICE_BOTLEFT:
        return ent.facing == direction::UP || ent.facing == direction::RIGHT;
    case terrain_type::ICE_BOTRIGHT:
        return ent.facing == direction::UP || ent.facing == direction::LEFT;

    case terrain_type::CLEAR:
    case terrain_type::PORTAL:
    case terrain_type::CHIP:
    case terrain_type::HINT:
    case terrain_type::SOCKET:
    case terrain_type::WATER:
    case terrain_type::FIRE:
    case terrain_type::GRAVEL:
    case terrain_type::ICE:
    case terrain_type::DIRT:
    case terrain_type::TRAP:
    case terrain_type::BOMB:
    case terrain_type::APPEARING_WALL:
    case terrain_type::MAGIC_TILE_WALL:
    case terrain_type::MAGIC_TILE_CLEAR:
    case terrain_type::GREEN_BUTTON_CLEAR:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::RED_BUTTON:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
    case terrain_type::GREEN_KEY:
    case terrain_type::RED_KEY:
    case terrain_type::CYAN_KEY:
    case terrain_type::YELLOW_KEY:
    case terrain_type::THIEF:
    case terrain_type::WALL_TRAP:
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
    case terrain_type::PUSH_FLOOR_MULTI:
    case terrain_type::TELEPORTER:
    case terrain_type::FIRE_BOOTS:
    case terrain_type::FLIPPERS:
    case terrain_type::ICE_SKATES:
    case terrain_type::SUCTION_BOOTS:
        return false;
    }

    return false;
}

[[nodiscard]] inline constexpr bool check_is_chip(const entity &ent) noexcept
{
    return ent.identity == entity_type::CHIPPIE;
}

[[nodiscard]] inline constexpr bool check_has_item(const entity &ent, inventory_item item) noexcept
{
    return ent.game_state->chippie_inventory.check(item) > 0;
}

[[nodiscard]] inline constexpr bool check_for_chip_and_item(const entity &ent, inventory_item item) noexcept
{
    return check_is_chip(ent) && check_has_item(ent, item);
};

[[nodiscard]] inline bool check_if_opaque_on_ice(const entity &ent, Grid::Location nextloc) noexcept
{
    /* helper lambda used twice in the logic below */
    const auto check_loc_has_moveable{[&](const Grid::Location testloc) -> bool {
        const auto entitr{find_entity_collision(ent, testloc, std::begin(ent.game_state->entity_list),
                                                std::end(ent.game_state->entity_list))};
        return entitr != std::end(ent.game_state->entity_list) && entitr->identity == entity_type::MOVEABLE_BLOCK;
    }};

    const auto terrain{ent.game_state->the_map[nextloc]};

    const auto next_loc_has_moveable{check_loc_has_moveable(nextloc)};

    if (ent.identity == entity_type::CHIPPIE)
    {
        /* extra check if the space behind the moveable block is clear */
        /* if there's a moveable block AND the space behind the moveable block is opaque, then it
         * is opaque for us */
        const auto moveable_present_and_space_behind_is_opaque{[&]() {
            if (!next_loc_has_moveable)
            {
                return false;
            }

            /* if any of the following are true, the move is disallowed and check_if_opaque_on_ice should return
               true

                the terrain the moveable block will move into is opaque
                the location the moveable block will move into already has a moveable block
            */
            const auto nextnextloc{move(nextloc, ent.facing)}; /* simulates the move the block will make */
            const auto nextnexterrain{ent.game_state->the_map[nextnextloc]};
            const auto already_has_moveable_block{check_loc_has_moveable(nextnextloc)};
            return check_terrain_is_opaque_for_moveable(ent.facing, nextnexterrain) || already_has_moveable_block;
        }()};

        return moveable_present_and_space_behind_is_opaque || check_terrain_is_opaque_for_chippie(ent, terrain);
    }

    return next_loc_has_moveable || check_terrain_is_opaque(ent, terrain);
}

inline void apply_fire_effect(entity &ent) noexcept
{

    /* everything dies except water glider, moveable block,  and fire dancer */
    switch (ent.identity)
    {
    case entity_type::MOVEABLE_BLOCK:
    case entity_type::WATER_GLIDER:
    case entity_type::FIRE_DANCER:
        break;
    case entity_type::CHIPPIE:
        if (!check_has_item(ent, inventory_item::FIRE_BOOTS))
        {
            event::register_event(event::event_type::GOT_BURNED);
        }
        break;
    default:
        ent.alive = false;
        break;
    }
}

inline void apply_water_effect(entity &ent) noexcept
{
    /* everything dies except water glider and chippie with flippers... moveable block is special */
    switch (ent.identity)
    {
    case entity_type::MOVEABLE_BLOCK:
        ent.game_state->the_map[ent.loc] = terrain_type::DIRT;
        ent.alive = false;
    case entity_type::WATER_GLIDER:
        break;
    case entity_type::CHIPPIE:
        if (!check_has_item(ent, inventory_item::FLIPPERS))
        {
            event::register_event(event::event_type::FELL_IN_WATER);
        }
        break;
    default:
        ent.alive = false;
    }
}

inline void handle_red_button(entity &ent) noexcept
{
    auto &game_state{*(ent.game_state)};

    auto redbutton{
        std::ranges::find_if(game_state.red_button_list, [&](const auto &button) { return button.button == ent.loc; })};

    if (redbutton != std::cend(game_state.red_button_list))
    {
#ifdef DEBUG_PRINT
        printf("calling generate on a red button\n");
#endif
        /* activate the clone machine */
        redbutton->generate(game_state);
    }
}

inline void handle_trap(entity &ent) noexcept
{
    /* search for the matching button */
    const auto &listing{ent.game_state->brown_button_list};
    const auto entity_location{ent.loc};

    /* by definition, the search can't fail, so we'll skip the usual check */
    const auto trap_itr{
        std::ranges::find_if(listing, [=](const brown_button &button) { return button.trap == entity_location; })};

    ent.trapped = trap_itr->check_trap_is_set(*(ent.game_state));
}

inline void handle_brown_button(entity &ent) noexcept
{
    /* if there is an entity trapped, untrap it */

    /* search for the matching trap */
    const auto &listing{ent.game_state->brown_button_list};
    const auto entity_location{ent.loc};

    /* by definition, the search can't fail, so we'll skip the usual check */
    const auto button_itr{
        std::ranges::find_if(listing, [=](const brown_button &button) { return button.button == entity_location; })};

    /* search for an entity that might be trapped */
    auto trapped_itr{std::ranges::find_if(ent.game_state->entity_list,
                                          [=](const entity &otherent) { return otherent.loc == button_itr->trap; })};

    if (trapped_itr != std::end(ent.game_state->entity_list))
    {
        trapped_itr->trapped = false;
    }
}

inline void handle_bomb(entity &ent) noexcept
{
    if (check_is_chip(ent))
    {
        event::register_event(event::event_type::EXPLODED);
    }

    /* everything dies to the bomb */
    ent.alive = false;
}

inline void handle_teleporter(entity &ent) noexcept
{
    static constexpr uint64_t TELEPORT_TIC_PERIOD_US{100'000};

    /* by definition, this search cannot fail */
    const auto [newfacing, newloc]{ent.game_state->teleport_list.compute_exit(ent, ent.loc, ent.facing)};

    ent.loc = newloc;
    ent.facing = newfacing;
    ent.next_time = ent.next_time - get_entity_velocity(ent.identity) + TELEPORT_TIC_PERIOD_US;
}

} // namespace

void apply_terrain_entry_effect(entity &ent, terrain_type terrain) noexcept
{
    auto &map_tile{ent.game_state->the_map[ent.loc]};

    switch (terrain)
    {
    case terrain_type::PORTAL:
        event::register_event(event::event_type::NEXT_LEVEL);
        break;
    case terrain_type::FIRE:
        apply_fire_effect(ent);
        break;
    case terrain_type::WATER:
        apply_water_effect(ent);
        break;
    case terrain_type::TRAP:
        handle_trap(ent);
        break;
    case terrain_type::CHIP:
        ent.game_state->chippie_inventory.remove(inventory_item::CHIPS);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::SOCKET:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::GREEN_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::GREEN_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::RED_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::RED_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::CYAN_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::CYAN_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::YELLOW_KEY:
        ent.game_state->chippie_inventory.add(inventory_item::YELLOW_KEY);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::BOMB:
        handle_bomb(ent);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::RED_BUTTON:
        handle_red_button(ent);
        break;
    case terrain_type::BROWN_BUTTON:
        handle_brown_button(ent);
        break;
    case terrain_type::HINT:
        event::register_event(event::event_type::DISPLAY_HINT);
        break;
    case terrain_type::MAGIC_TILE_CLEAR:
    case terrain_type::DIRT:
        map_tile = terrain_type::CLEAR;
        break;
    /* directional terrain features */
    case terrain_type::PUSH_FLOOR_UP:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::UP;
        }
        break;
    case terrain_type::PUSH_FLOOR_DOWN:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::DOWN;
        }
        break;
    case terrain_type::PUSH_FLOOR_LEFT:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::LEFT;
        }
        break;
    case terrain_type::PUSH_FLOOR_RIGHT:
        if (!check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS))
        {
            ent.facing = direction::RIGHT;
        }
        break;

    /* the loot! (boots) */
    case terrain_type::FIRE_BOOTS:
        ent.game_state->chippie_inventory.add(inventory_item::FIRE_BOOTS);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::FLIPPERS:
        ent.game_state->chippie_inventory.add(inventory_item::FLIPPERS);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::ICE_SKATES:
        ent.game_state->chippie_inventory.add(inventory_item::ICE_SKATES);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::SUCTION_BOOTS:
        ent.game_state->chippie_inventory.add(inventory_item::SUCTION_BOOTS);
        map_tile = terrain_type::CLEAR;
        break;
    case terrain_type::GREEN_BUTTON:
        event::register_event(event::event_type::GREEN_BUTTON);
        break;
    case terrain_type::BLUE_BUTTON:
        event::register_event(event::event_type::BLUE_BUTTON);
        break;

    case terrain_type::WALL_TRAP:
        map_tile = terrain_type::WALL;
        break;

    case terrain_type::THIEF:
        ent.game_state->chippie_inventory.set(inventory_item::SUCTION_BOOTS, 0);
        ent.game_state->chippie_inventory.set(inventory_item::ICE_SKATES, 0);
        ent.game_state->chippie_inventory.set(inventory_item::FLIPPERS, 0);
        ent.game_state->chippie_inventory.set(inventory_item::FIRE_BOOTS, 0);
        break;

    case terrain_type::TELEPORTER:
        handle_teleporter(ent);
        break;

    /* these are handled in the override code */
    case terrain_type::ICE:
    case terrain_type::ICE_TOPLEFT:
    case terrain_type::ICE_TOPRIGHT:
    case terrain_type::ICE_BOTLEFT:
    case terrain_type::ICE_BOTRIGHT:

    /* and the rest */
    case terrain_type::CLEAR:
    case terrain_type::WALL:
    case terrain_type::GRAVEL:
    case terrain_type::THIN_WALL_TOP:
    case terrain_type::THIN_WALL_BOT:
    case terrain_type::THIN_WALL_LEFT:
    case terrain_type::THIN_WALL_RIGHT:
    case terrain_type::THIN_WALL_BOTRIGHT:
    case terrain_type::PUSH_FLOOR_MULTI:
        break;
    }
}

void apply_terrain_exit_effect(entity &ent, terrain_type terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::HINT:
        event::register_event(event::event_type::CLEAR_HINT);
        break;
    case terrain_type::PORTAL:
    case terrain_type::FIRE:
    case terrain_type::WATER:
    case terrain_type::ICE:
    case terrain_type::TRAP:
    case terrain_type::CHIP:
    case terrain_type::SOCKET:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
    case terrain_type::GREEN_KEY:
    case terrain_type::RED_KEY:
    case terrain_type::CYAN_KEY:
    case terrain_type::YELLOW_KEY:
    case terrain_type::BOMB:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::CLEAR:
    case terrain_type::WALL:

    /* non-directional */
    case terrain_type::GRAVEL:
    case terrain_type::DIRT:

    /* buttons, doors, keys */
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::RED_BUTTON:

    /* directional terrain features */
    case terrain_type::ICE_TOPLEFT:
    case terrain_type::ICE_TOPRIGHT:
    case terrain_type::ICE_BOTLEFT:
    case terrain_type::ICE_BOTRIGHT:
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
    case terrain_type::PUSH_FLOOR_MULTI:
    case terrain_type::THIN_WALL_BOT:
    case terrain_type::THIN_WALL_TOP:
    case terrain_type::THIN_WALL_LEFT:
    case terrain_type::THIN_WALL_RIGHT:
    case terrain_type::THIN_WALL_BOTRIGHT:
        break;
    }
}

void apply_terrain_override_effect(entity &ent, Grid::Location &nextloc, direction &nextfacing,
                                   terrain_type terrain) noexcept
{
    static constexpr uint64_t FIXED_PERIOD_US{100'000};

    auto &&to_direction{[&](terrain_type push_floor_typespecific_type) {
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_DOWN)
        {
            return direction::DOWN;
        }
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_UP)
        {
            return direction::UP;
        }
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_LEFT)
        {
            return direction::LEFT;
        }
        if (push_floor_typespecific_type == terrain_type::PUSH_FLOOR_RIGHT)
        {
            return direction::RIGHT;
        }

        return ent.facing;
    }};

    auto &&adjust_facing_on_ice{[&](const direction input_facing, const terrain_type ice_terrain) {
        if (ice_terrain == terrain_type::ICE_TOPLEFT)
        {
            return input_facing == direction::UP ? direction::RIGHT : direction::DOWN;
        }

        if (ice_terrain == terrain_type::ICE_TOPRIGHT)
        {
            return input_facing == direction::UP ? direction::LEFT : direction::DOWN;
        }

        if (ice_terrain == terrain_type::ICE_BOTLEFT)
        {
            return input_facing == direction::DOWN ? direction::RIGHT : direction::UP;
        }

        if (ice_terrain == terrain_type::ICE_BOTRIGHT)
        {
            return input_facing == direction::DOWN ? direction::LEFT : direction::UP;
        }

        return input_facing;
    }};

    /* just the force floors, ice, and thin walls do this */
    switch (terrain)
    {
    case terrain_type::ICE:
    case terrain_type::ICE_TOPLEFT:
    case terrain_type::ICE_TOPRIGHT:
    case terrain_type::ICE_BOTLEFT:
    case terrain_type::ICE_BOTRIGHT:
        if (!check_for_chip_and_item(ent, inventory_item::ICE_SKATES))
        {
            ent.facing = adjust_facing_on_ice(ent.facing, terrain);
            nextloc = move(ent.loc, ent.facing);
            /* if the nextloc is opaque for this entity, reverse facing */
            if (check_if_opaque_on_ice(ent, nextloc))
            {
                // nextfacing = reverse(ent.facing);
                nextfacing = adjust_facing_on_ice(reverse(ent.facing), terrain);
                nextloc = move(ent.loc, nextfacing);
            }
            else
            {
                nextfacing = ent.facing;
            }
            /* force the move at a fixed rate */
            ent.next_time = ent.game_state->the_clock.increment_time_point(
                (ent.next_time - get_entity_velocity(ent.identity)), FIXED_PERIOD_US);
        }
        break;
    /* push floors allow movements if it's orthogonal to the floor's direction */
    /* so for example, with PUSH_FLOOR_UP, if the next facing is either LEFT or RIGHT, do nothing */
    /* otherwise, behaviour is identical to ice */
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
        if (!(check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS) ||
              (ent.facing != direction::LEFT && nextfacing == direction::LEFT) ||
              (ent.facing != direction::RIGHT && nextfacing == direction::RIGHT)))
        {
            nextloc = move(ent.loc, ent.facing);
            nextfacing = to_direction(terrain);
            /* force the move at a fixed rate */
            ent.next_time = ent.next_time - get_entity_velocity(ent.identity) + FIXED_PERIOD_US;
        }
        break;
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
        if (!(check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS) ||
              (ent.facing != direction::UP && nextfacing == direction::UP) ||
              (ent.facing != direction::DOWN && nextfacing == direction::DOWN)))
        {
            nextloc = move(ent.loc, ent.facing);
            nextfacing = to_direction(terrain);
            /* force the move at a fixed rate */
            ent.next_time = ent.next_time - get_entity_velocity(ent.identity) + FIXED_PERIOD_US;
        }
        break;
    case terrain_type::PUSH_FLOOR_MULTI:
        if (!(check_for_chip_and_item(ent, inventory_item::SUCTION_BOOTS) || (ent.facing != nextfacing)))
        {
            nextloc = move(ent.loc, ent.facing);
            nextfacing = ent.facing;
            /* force the move at a fixed rate */
            ent.next_time = ent.next_time - get_entity_velocity(ent.identity) + FIXED_PERIOD_US;
        }
        break;
    case terrain_type::THIN_WALL_TOP:
        if (nextfacing == direction::UP)
        {
            nextloc = ent.loc;
        }
        break;
    case terrain_type::THIN_WALL_BOT:
        if (nextfacing == direction::DOWN)
        {
            nextloc = ent.loc;
        }
        break;
    case terrain_type::THIN_WALL_LEFT:
        if (nextfacing == direction::LEFT)
        {
            nextloc = ent.loc;
        }
        break;
    case terrain_type::THIN_WALL_RIGHT:
        if (nextfacing == direction::RIGHT)
        {
            nextloc = ent.loc;
        }
        break;
    case terrain_type::THIN_WALL_BOTRIGHT:
        if (nextfacing == direction::RIGHT || nextfacing == direction::DOWN)
        {
            nextloc = ent.loc;
        }
        break;

    case terrain_type::TELEPORTER:
        nextloc = move(ent.loc, ent.facing);
        nextfacing = ent.facing;
        break;
    default:
        break;
    }
}

bool check_terrain_is_opaque(const entity &ent, terrain_type terrain) noexcept
{
    if (ent.identity == entity_type::CHIPPIE)
    {
        return check_terrain_is_opaque_for_chippie(ent, terrain);
    }

    switch (terrain)
    {
    case terrain_type::PORTAL:
    case terrain_type::WALL:
    case terrain_type::SOCKET:
    case terrain_type::CHIP:
    case terrain_type::GRAVEL:
    case terrain_type::DIRT:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
    case terrain_type::GREEN_BUTTON_WALL:
    case terrain_type::CLONER_FIRE_DANCER:
    case terrain_type::CLONER_MOVEABLE_BLOCK:
    case terrain_type::CLONER_FROG_MONSTER:
    case terrain_type::GREEN_KEY:
    case terrain_type::RED_KEY:
    case terrain_type::CYAN_KEY:
    case terrain_type::YELLOW_KEY:
    case terrain_type::INVISIBLE_WALL:
    case terrain_type::APPEARING_WALL:
    case terrain_type::MAGIC_TILE_WALL:
    case terrain_type::MAGIC_TILE_CLEAR:
    case terrain_type::CLONER_BLUE_TANK_THAT_MOVES_LEFT:
    case terrain_type::CLONER_CENTIPEDE:
    case terrain_type::CLONER_PURPLE_BALL:
    case terrain_type::CLONER_CYAN_STICK_BALL:
    case terrain_type::FIRE_BOOTS:
    case terrain_type::FLIPPERS:
    case terrain_type::ICE_SKATES:
    case terrain_type::SUCTION_BOOTS:
    case terrain_type::WALL_TRAP:
        return true;

    case terrain_type::THIN_WALL_TOP:
        return ent.facing == direction::DOWN;
    case terrain_type::THIN_WALL_BOT:
        return ent.facing == direction::UP;
    case terrain_type::THIN_WALL_LEFT:
        return ent.facing == direction::RIGHT;
    case terrain_type::THIN_WALL_RIGHT:
        return ent.facing == direction::LEFT;
    case terrain_type::THIN_WALL_BOTRIGHT:
        return ent.facing == direction::LEFT || ent.facing == direction::UP;

    case terrain_type::ICE_TOPLEFT:
        return ent.facing == direction::DOWN || ent.facing == direction::RIGHT;
    case terrain_type::ICE_TOPRIGHT:
        return ent.facing == direction::DOWN || ent.facing == direction::LEFT;
    case terrain_type::ICE_BOTLEFT:
        return ent.facing == direction::UP || ent.facing == direction::RIGHT;
    case terrain_type::ICE_BOTRIGHT:
        return ent.facing == direction::UP || ent.facing == direction::LEFT;

    case terrain_type::FIRE:
        return ent.identity != entity_type::WATER_GLIDER && ent.identity != entity_type::FIRE_DANCER &&
               ent.identity != entity_type::MOVEABLE_BLOCK;

    case terrain_type::CLEAR:
    case terrain_type::HINT:
    case terrain_type::WATER:
    case terrain_type::ICE:
    case terrain_type::TRAP:
    case terrain_type::BOMB:
    case terrain_type::GREEN_BUTTON_CLEAR:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::RED_BUTTON:
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
    case terrain_type::THIEF:
    case terrain_type::PUSH_FLOOR_MULTI:
    case terrain_type::TELEPORTER:
        return false;
    }

    return false;
}

[[nodiscard]] bool check_terrain_is_opaque_for_moveable(direction dir, terrain_type terrain) noexcept
{
    switch (terrain)
    {
    case terrain_type::WALL:
    case terrain_type::DIRT:
    case terrain_type::PORTAL:
    case terrain_type::SOCKET:
    case terrain_type::GRAVEL:
    case terrain_type::GREEN_DOOR:
    case terrain_type::RED_DOOR:
    case terrain_type::CYAN_DOOR:
    case terrain_type::YELLOW_DOOR:
    case terrain_type::GREEN_BUTTON_WALL:
    case terrain_type::CLONER_FIRE_DANCER:
    case terrain_type::CLONER_MOVEABLE_BLOCK:
    case terrain_type::CLONER_FROG_MONSTER:
    case terrain_type::INVISIBLE_WALL:
    case terrain_type::APPEARING_WALL:
    case terrain_type::MAGIC_TILE_WALL:
    case terrain_type::CLONER_BLUE_TANK_THAT_MOVES_LEFT:
    case terrain_type::CLONER_CENTIPEDE:
    case terrain_type::CLONER_PURPLE_BALL:
    case terrain_type::CLONER_CYAN_STICK_BALL:
        return true;

    case terrain_type::THIN_WALL_TOP:
        return dir == direction::DOWN;
    case terrain_type::THIN_WALL_BOT:
        return dir == direction::UP;
    case terrain_type::THIN_WALL_LEFT:
        return dir == direction::RIGHT;
    case terrain_type::THIN_WALL_RIGHT:
        return dir == direction::LEFT;
    case terrain_type::THIN_WALL_BOTRIGHT:
        return dir == direction::LEFT || dir == direction::UP;

    case terrain_type::ICE_TOPLEFT:
        return dir == direction::DOWN || dir == direction::RIGHT;
    case terrain_type::ICE_TOPRIGHT:
        return dir == direction::DOWN || dir == direction::LEFT;
    case terrain_type::ICE_BOTLEFT:
        return dir == direction::UP || dir == direction::RIGHT;
    case terrain_type::ICE_BOTRIGHT:
        return dir == direction::UP || dir == direction::LEFT;

    case terrain_type::CLEAR:
    case terrain_type::CHIP:
    case terrain_type::HINT:
    case terrain_type::WATER:
    case terrain_type::FIRE:
    case terrain_type::ICE:
    case terrain_type::TRAP:
    case terrain_type::BOMB:
    case terrain_type::GREEN_BUTTON_CLEAR:
    case terrain_type::GREEN_BUTTON:
    case terrain_type::BLUE_BUTTON:
    case terrain_type::BROWN_BUTTON:
    case terrain_type::RED_BUTTON:
    case terrain_type::GREEN_KEY:
    case terrain_type::RED_KEY:
    case terrain_type::CYAN_KEY:
    case terrain_type::YELLOW_KEY:
    case terrain_type::PUSH_FLOOR_UP:
    case terrain_type::PUSH_FLOOR_DOWN:
    case terrain_type::PUSH_FLOOR_LEFT:
    case terrain_type::PUSH_FLOOR_RIGHT:
    case terrain_type::PUSH_FLOOR_MULTI:
    case terrain_type::MAGIC_TILE_CLEAR:
    case terrain_type::THIEF:
    case terrain_type::WALL_TRAP:
    case terrain_type::TELEPORTER:
    case terrain_type::FIRE_BOOTS:
    case terrain_type::FLIPPERS:
    case terrain_type::ICE_SKATES:
    case terrain_type::SUCTION_BOOTS:
        return false;
    }

    return false;
}

} // namespace chippie