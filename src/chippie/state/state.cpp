#include "state.hpp"

#include "chippie/chippie_common.hpp"
#include "chippie/entities/basic_entity.hpp"
#include "chippie/entities/chippie_entity.hpp"
#include "chippie/entities/entity_types.hpp"
#include "chippie/events/event.hpp"
#include "chippie/events/event_types.hpp"
#include "chippie/render/paint_utils.hpp"
#include "chippie/state/chippie_inventory.hpp"
#include "chippie/textures/texture.hpp"
#include "chippie/textures/texture_defs.hpp"
#include "common/screen_utils.hpp"
#include "gamepad/gamepad.hpp"
#include "screen/gfx/dialog.hpp"
#include "screen/gfx/shapes.hpp"
#include "screen/glyphs/letter_utils.hpp"
#include "screen/glyphs/letters.hpp"
#include "screen/screen.hpp"
#include "static_map.hpp"
#include "terrain_effects.hpp"

#include "embp/containers.hpp"

#include "pico/time.h"
#include <pico/types.h>

#include <algorithm>
#include <cstring>
#include <functional>

// #define DEBUG_PRINT
#ifdef DEBUG_PRINT
#include <pico/printf.h>
#endif

namespace chippie
{

namespace
{
/** hack to work with current design of event.hpp  */
static State *myself{nullptr};
} // namespace

State::State() noexcept
{
    intialize_event_handlers();
    init_play_grid();

    /*
    State is now magically, and subtly, a singleton...
    An alternative is to use either:
    * event_handler uses std::function<void()> instead of void(*)()
    * use the event system's dequeue_next_event and process them ourselves, in lieu of helper function
    event::process_events()
    */
    myself = this;

    /* we use the dialog, so let's set its palette */
    struct dialog_palette
    {
        uint16_t bright_highlight;
        uint16_t shadow_highlight;
        uint16_t background;
        uint16_t font_color;
    };

    screen::gfx::set_dialog_palette({
        .bright_highlight = WHITE,
        .shadow_highlight = DRKGRY,
        .background = BLACK,
        .font_color = YELLOW,
    });
}

void State::intialize_event_handlers() noexcept
{
    event::register_handler(event::event_handler{
        .identifier = event::event_type::TIME_UP,
        .handler = [] { myself->handle_time_up(); },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::NEXT_LEVEL,
        .handler = [] { myself->handle_portal(); },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::DISPLAY_HINT,
        .handler = [] { myself->handle_display_hint(); },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::CLEAR_HINT,
        .handler = [] { myself->handle_clear_hint(); },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::OPEN_MENU,
        .handler = [] { myself->handle_open_menu(); },
    });
    event::register_handler(event::event_handler{
        .identifier = event::event_type::MOVE_MOVABLE_BLOCK,
        .handler = [] { myself->handle_moving_movable_block(); },
    });
}

void State::handle_portal() noexcept
{
    active = false;
}
void State::handle_time_up() noexcept
{
    handle_portal();
}
void State::handle_open_menu() noexcept
{
    handle_portal();
}

void State::handle_display_hint() noexcept
{
    hint_displayable = true;
}

void State::handle_clear_hint() noexcept
{
    hint_displayable = false;
}

bool State::is_active() noexcept
{
    return active;
}

const static_map &State::get_map() const noexcept
{
    return the_map;
}

static_map &State::get_map() noexcept
{
    return the_map;
};

void State::load_level() noexcept
{
    load_level_from_rom_stub(level_number);
    load_entity_list_from_rom_stub(level_number);
    paint();
}

void State::process() noexcept
{
    process_opening_level_dialog();

    process_time_remaining();

    move_entities();

    /* TODO event processing, which may change the size of entity_list as well as make changes to the_map */
    event::process_events();

    /* entity cleanup */
    auto first_dead_itr{
        std::partition(std::begin(entity_list), std::end(entity_list), [](const auto &ent) { return ent.alive; })};
    entity_list.resize(std::distance(std::begin(entity_list), first_dead_itr));

    /* paint the entire frame every 33 ms */
    if (absolute_time_diff_us(last_paint_time, get_absolute_time()) > PAINT_TIME_INTERVAL_US)
    {
        last_paint_time = delayed_by_us(last_paint_time, PAINT_TIME_INTERVAL_US);
        paint();
    }
}

void State::paint() noexcept
{
    screen::pause_screen();

    paint_the_background();

    paint_the_map();

    if (!hint_displayable)
    {
        paint_chip_count_display();
        paint_timer_display();
        paint_level_display();
        paint_inventory();
    }
    else
    {
        paint_hint();
    }

    for (const auto &ent : entity_list)
    {
        if (within_view_port(ent.loc))
        {
            paint_entity(ent);
        }
    }

    screen::resume_screen();
}

void State::process_opening_level_dialog() noexcept
{
    if (!display_level_name_once)
    {
        return;
    }

    paint();

    display_level_name_once = false;

    /* draw the level dialog */
    screen::gfx::display_dialog_box(level_name_text);

    /* spin until the user presses a button */
    while (true)
    {
        const auto [up, down, right, left, etc, unused]{gamepad::five::get()};
        const bool result = up | down | right | left | etc;

        if (result)
        {
            break;
        }

        /* TODO pico-sdk specific call should be abstracted for better portability */
        sleep_ms(1);
    }

    countdown_timer.reset();
    last_paint_time = get_absolute_time();
}

void State::load_entity_list_from_rom_stub([[maybe_unused]] uint8_t level) noexcept
{
    entity_list.clear();
    // by convention, Chippie is always the first one in the entity_list
    entity_list.push_back(chippie::create({.x = 15, .y = 14}, direction::DOWN, 0));
}

void State::load_level_from_rom_stub(uint8_t level) noexcept
{
    /* set the time limit, if any */
    time_remaining = 100;
    countdown_timer.reset();

    /* set the amount of chips for the level */
    access_chippie_inventory().set(inventory_item::CHIPS, 11);

    /* set the hint string, which can be null */
    hint_text = "Collect chips to get past the chip socket. Use keys to open doors.";
    /* set the level name */
    level_name_text = "LESSON 1\nPassword: BDHP";

    /* clear the map data */
    std::memset(std::data(the_map.map_data), static_cast<int>(terrain_type::CLEAR),
                sizeof(terrain_type) * std::size(the_map.map_data));

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

    /* row 18 */
    the_map[Grid::Location{.x = 12, .y = 18}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 18}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 18}] = terrain_type::WALL;

    /* row 19 */
    the_map[Grid::Location{.x = 12, .y = 19}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 19}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 19}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 14, .y = 19}] = terrain_type::CHIP;
    the_map[Grid::Location{.x = 16, .y = 19}] = terrain_type::CHIP;

    /* row 20 */
    the_map[Grid::Location{.x = 12, .y = 20}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 15, .y = 20}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 18, .y = 20}] = terrain_type::WALL;
    the_map[Grid::Location{.x = 16, .y = 20}] = terrain_type::GREEN_KEY;

    /* row 21 */
    for (uint32_t xx = 12; xx < 19; ++xx)
    {
        the_map[Grid::Location{.x = xx, .y = 21}] = terrain_type::WALL;
    }
}

void State::move_entities() noexcept
{
    for (auto &ent : entity_list)
    {

        /*  If this guy is dead, or timer hasn't expired yet for a move, then skip.
            Dead entities are cleaned up later.
        */
        if (!ent.alive || absolute_time_diff_us(get_absolute_time(), ent.next_time) > 0)
        {
            continue;
        }

        /* Update next move time w/o drift */
        ent.next_time = delayed_by_us(ent.next_time, get_entity_velocity(ent.identity));

        /* get the function table for this entity */
        const auto entity_handles{get_state_functions(ent.identity)};

        /* begin processing with the entry handler */
        if (entity_handles.entry_handler != nullptr)
        {
            entity_handles.entry_handler(ent);
        }

        /* next, compute where this entity wants to move */
        const auto [nextloc, nextfacing]{entity_handles.process_move_handler != nullptr
                                             ? entity_handles.process_move_handler(ent)
                                             : std::make_pair(ent.loc, ent.facing)};

        if (play_grid.out_of_bounds(nextloc))
        {
            ent.alive = false;
            continue;
        }

        /* At least for now, we always update the facing, even if we don't end up moving.
           note that the collision handler may further edit the facing. */
        ent.facing = nextfacing;

        /* if the next location is the same as the current, then there's nothing more to do */
        if (nextloc == ent.loc)
        {
            continue;
        }

        /* then, determine and resolve collisions */
        const collision_result collision{find_collisions(nextloc)};

        const collision_action action{
            entity_handles.entity_collision_handler != nullptr
                ? entity_handles.entity_collision_handler(ent, collision.other, collision.tile)
                : collision_action::NO_ACTION_NEEDED};

        /* resolve side-effects from collision handler code */
        if (action == collision_action::NO_ACTION_NEEDED)
        {
            continue;
        }

        if (action == collision_action::MARK_DEAD)
        {
            ent.alive = false;
            continue;
        }

        /* TODO feed this with the tile from the previous entity location */
        apply_terrain_exit_effect(ent, the_map[ent.loc]);

        if (action == collision_action::APPLY_NEXT_LOCATION)
        {
            ent.loc = nextloc;
        }
        else
        {
            /* made it this far?  We can attempt the move */
            const auto direction_to_move{[&]() {
                if (action == collision_action::MOVE_FORWARD)
                {
                    return relative_direction::FORWARD;
                }
                if (action == collision_action::MOVE_BACKWARD)
                {
                    return relative_direction::BACKWARD;
                }
                if (action == collision_action::MOVE_RIGHT)
                {
                    return relative_direction::RIGHT;
                }
                if (action == collision_action::MOVE_LEFT)
                {
                    return relative_direction::LEFT;
                }
                return relative_direction::FORWARD;
            }()};

            /* then, effect any terrain */
            move_relative_direction(ent, direction_to_move);
        }

        apply_terrain_entry_effect(ent, collision.tile);

        /* finally, process the exit handler */
        if (entity_handles.exit_handler != nullptr)
        {
            entity_handles.exit_handler(ent);
        }
    }
}

[[nodiscard]] collision_result State::find_collisions(Grid::Location next_location) noexcept
{
    collision_result result{.other = nullptr, .tile = the_map[next_location]};

    /* search for other entities */
    auto entitr = std::find_if(std::begin(entity_list), std::end(entity_list),
                               [&](const auto &other) { return other.loc == next_location; });
    if (entitr != std::end(entity_list))
    {
        result.other = entitr;
    }

    return result;
}

void State::process_time_remaining() noexcept
{
    if (countdown_timer.elapsed())
    {
        countdown_timer.increment();

        if (time_remaining == 0)
        {

            event::register_event(event::event_type::TIME_UP);
            return;
        }

        --time_remaining;
    }
}

void State::update_view_port_position() noexcept
{
    /* view port is always centered on Chippie, within the bounds of the play field */
    const auto [chipx, chipy]{entity_list.front().loc};

    /* compute x location */
    if (chipx < 4U)
    {
        view_port.topleft.x = 0;
    }
    else if (chipx + 4U >= play_grid.config().grid_width)
    {
        view_port.topleft.x = play_grid.config().grid_width - view_port.size.width;
    }
    else
    {
        view_port.topleft.x = chipx - 4U;
    }

    /* again, for y */
    if (chipy < 4U)
    {
        view_port.topleft.y = 0;
    }
    else if (chipy + 4U >= play_grid.config().grid_height)
    {
        view_port.topleft.y = play_grid.config().grid_height - view_port.size.height;
    }
    else
    {
        view_port.topleft.y = chipy - 4U;
    }
}

[[nodiscard]] bool State::within_view_port(Grid::Location loc) noexcept
{
    return view_port.contains({.x = loc.x, .y = loc.y});
}

void State::paint_the_background() noexcept
{
    /* we use a light grey background */
    screen::fill_screen((LGREY << 4) | LGREY);
}

void State::paint_the_map() noexcept
{
    /* first, update the view_port */
    update_view_port_position();

    /* next, draw the view port on the physical screen */
    for (uint32_t yy = 0; yy < view_port.size.height; ++yy)
    {
        for (uint32_t xx = 0; xx < view_port.size.width; ++xx)
        {
            const Grid::Location abs_loc{.x = xx + view_port.topleft.x, .y = yy + view_port.topleft.y};
            const terrain_type terrain{the_map[abs_loc]};
            draw_tile_index(view_grid, make_relative_to_view_port(abs_loc), texture::get_terrain_texture_type(terrain));
        }
    }
}
void State::paint_chip_count_display() noexcept
{
    const auto chip_count{access_chippie_inventory().check(inventory_item::CHIPS)};
    const auto [msb, mmb, lsb]{screen::bcd<3>(chip_count)};

    draw_tile_index(chip_count_grid, {.x = 0, .y = 0}, texture::get_digit_texture(msb));
    draw_tile_index(chip_count_grid, {.x = 1, .y = 0}, texture::get_digit_texture(mmb));
    draw_tile_index(chip_count_grid, {.x = 2, .y = 0}, texture::get_digit_texture(lsb));
}

void State::paint_timer_display() noexcept
{
    const auto [msb, mmb, lsb]{screen::bcd<3>(time_remaining)};

    draw_tile_index(time_remaining_grid, {.x = 0, .y = 0}, texture::get_digit_texture(msb));
    draw_tile_index(time_remaining_grid, {.x = 1, .y = 0}, texture::get_digit_texture(mmb));
    draw_tile_index(time_remaining_grid, {.x = 2, .y = 0}, texture::get_digit_texture(lsb));
}

void State::paint_level_display() noexcept
{
    const auto [msb, mmb, lsb]{screen::bcd<3>(level_number)};

    draw_tile_index(level_count_grid, {.x = 0, .y = 0}, texture::get_digit_texture(msb));
    draw_tile_index(level_count_grid, {.x = 1, .y = 0}, texture::get_digit_texture(mmb));
    draw_tile_index(level_count_grid, {.x = 2, .y = 0}, texture::get_digit_texture(lsb));
}

void State::paint_hint() noexcept
{
    screen::gfx::draw_rect(hint_area, BLACK, 0);

    /* draw the letters out one by one */
    if (!hint_text)
    {
        return;
    }

    int start_y{1};
    int start_x{1};
    int ii{};
    while (hint_text[ii] != '\0')
    {
        const auto [outx, outy]{hint_print_grid.to_native({.x = start_x, .y = start_y})};

        screen::draw_standard_character_to_4bpp_display(hint_text[ii], outx, outy, WHITE, BLACK);

        /* then, increment to the next column or do a carriage return if at the end of the line */
        if (start_x + 1 < hint_print_grid.config().grid_width - 1)
        {
            ++start_x;
        }
        else
        {
            start_x = 1;
            ++start_y;
        }
        ++ii;
    }
}

void State::paint_inventory() noexcept
{
    /* order is boots, suckers, skates, flippers, red, cyan, yellow, green */

    int xloc{0};

    static constexpr std::array inventory_items{
        inventory_item::FIRE_BOOTS, inventory_item::SUCTION_BOOTS, inventory_item::ICE_SKATES,
        inventory_item::FLIPPERS,   inventory_item::RED_KEY,       inventory_item::CYAN_KEY,
        inventory_item::YELLOW_KEY, inventory_item::GREEN_KEY,
    };

    static constexpr std::array item_textures{
        texture::texture_type::SOCKET,     texture::texture_type::SOCKET,    texture::texture_type::SOCKET,
        texture::texture_type::SOCKET,     texture::texture_type::RED_KEY,   texture::texture_type::CYAN_KEY,
        texture::texture_type::YELLOW_KEY, texture::texture_type::GREEN_KEY,
    };

    const auto clear_type{texture::get_texture(texture::texture_type::CLEAR)};

    for (int xloc = 0; xloc < std::size(inventory_items); ++xloc)
    {

        draw_tile_index(inventory_grid, {.x = xloc, .y = 0},
                        access_chippie_inventory().check(inventory_items[xloc])
                            ? texture::get_texture(item_textures[xloc])
                            : clear_type);
    }
}

void State::paint_entity(const entity &ent) const noexcept
{
    const auto loc{make_relative_to_view_port(ent.loc)};
    draw_tile_index(view_grid, loc, texture::get_entity_texture_type(ent));
}

[[nodiscard]] Grid::Location State::make_relative_to_view_port(Grid::Location abs_loc) const noexcept
{
    return {.x = abs_loc.x - view_port.topleft.x, .y = abs_loc.y - view_port.topleft.y};
}

void State::init_play_grid() noexcept
{
    const auto [width, height]{screen::get_virtual_screen_size()};
    const Grid::GridCfg view_cfg{
        .xdimension = {.off = (width - TILE_SIDE_LENGTH * 9) / 2, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 9,
        .grid_height = 9,
    };
    view_grid = Grid{view_cfg};

    static constexpr Grid::GridCfg play_cfg{
        .xdimension = {.off = 0, .scale = 1},
        .ydimension = {.off = 0, .scale = 1},
        .grid_width = 32,
        .grid_height = 32,
    };
    play_grid = Grid{play_cfg};

    const auto interval{(width - TILE_SIDE_LENGTH * 3 * 3) / 4};
    const Grid::GridCfg level_count_cfg{
        .xdimension = {.off = interval, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10 + 9 * TILE_SIDE_LENGTH + 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 3,
        .grid_height = 1,
    };
    const Grid::GridCfg time_remaining_cfg{
        .xdimension = {.off = 2 * interval + 3 * TILE_SIDE_LENGTH, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10 + 9 * TILE_SIDE_LENGTH + 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 3,
        .grid_height = 1,
    };
    const Grid::GridCfg chip_count_cfg{
        .xdimension = {.off = 3 * interval + 2 * 3 * TILE_SIDE_LENGTH, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = 10 + 9 * TILE_SIDE_LENGTH + 10, .scale = TILE_SIDE_LENGTH},
        .grid_width = 3,
        .grid_height = 1,
    };

    chip_count_grid = Grid{chip_count_cfg};
    level_count_grid = Grid{level_count_cfg};
    time_remaining_grid = Grid{time_remaining_cfg};

    /* hint area is the portion of the screen where we'll print the hint text.  It lives in the space between the bottom
     * of the view grid and the bottom of the physical screen */
    hint_area.topleft.x = level_count_cfg.xdimension.off;
    hint_area.topleft.y = level_count_cfg.ydimension.off;
    hint_area.size.width = width - 2 * hint_area.topleft.x;
    const auto bottom_row_of_view_area{view_cfg.grid_height * view_cfg.ydimension.scale + view_cfg.ydimension.off};
    const auto total_height_available{height - bottom_row_of_view_area};
    const auto height_after_applying_margin{total_height_available - (hint_area.topleft.y - bottom_row_of_view_area) -
                                            view_cfg.ydimension.off};
    hint_area.size.height = height_after_applying_margin;

    const Grid::GridCfg hint_print_grid_cfg{
        .xdimension = {.off = hint_area.topleft.x, .scale = glyphs::tile::width()},
        .ydimension = {.off = hint_area.topleft.y, .scale = glyphs::tile::height()},
        .grid_width = hint_area.size.width / glyphs::tile::width(),
        .grid_height = hint_area.size.height / glyphs::tile::height(),
    };
    hint_print_grid = Grid{hint_print_grid_cfg};

    /* inventory grid fills the final gap between the three counters and the bottom of the screen */
    const auto height_for_inventory_grid{height_after_applying_margin - TILE_SIDE_LENGTH};
    const auto bottom_row_of_chip_count{chip_count_cfg.ydimension.off +
                                        chip_count_cfg.ydimension.scale * chip_count_cfg.grid_height};
    const Grid::GridCfg inventory_cfg{
        .xdimension = {.off = width / 2 - 4 * TILE_SIDE_LENGTH, .scale = TILE_SIDE_LENGTH},
        .ydimension = {.off = bottom_row_of_chip_count + height_for_inventory_grid / 2 - TILE_SIDE_LENGTH / 2,
                       .scale = TILE_SIDE_LENGTH},
        .grid_width = 8,
        .grid_height = 1,
    };
    inventory_grid = Grid{inventory_cfg};

#ifdef DEBUG_PRINT
    printf("hint area\n");
    printf("    x      %d\n", hint_area.topleft.x);
    printf("    y      %d\n", hint_area.topleft.y);
    printf("    width  %d\n", hint_area.size.width);
    printf("    height %d\n", hint_area.size.height);
#endif
}
} // namespace chippie