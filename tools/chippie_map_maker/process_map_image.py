#!/usr/bin/env python3

from PIL import Image
import numpy as np
import pickle
import matplotlib.pyplot as plt
import argparse
import os
import hashlib

DEFAULT_TERRAIN_TYPES = [
    "CLEAR",
    "PORTAL",
    "WALL",
    "CHIP",
    "HINT",
    "SOCKET",
    "WATER",
    "FIRE",
    "GRAVEL",
    "ICE",
    "DIRT",
    "TRAP",
    "BOMB",
    "INVISIBLE_WALL",
    "APPEARING_WALL",
    "MAGIC_TILE_WALL",
    "MAGIC_TILE_CLEAR",
    "GREEN_BUTTON_CLEAR",
    "GREEN_BUTTON_WALL",
    "GREEN_BUTTON",
    "BLUE_BUTTON",
    "BROWN_BUTTON",
    "RED_BUTTON",
    "GREEN_DOOR",
    "RED_DOOR",
    "CYAN_DOOR",
    "YELLOW_DOOR",
    "GREEN_KEY",
    "RED_KEY",
    "CYAN_KEY",
    "YELLOW_KEY",
    "THIEF",
    "WALL_TRAP",
    "CLONER_FIRE_DANCER",
    "ICE_TOPLEFT",
    "ICE_TOPRIGHT",
    "ICE_BOTLEFT",
    "ICE_BOTRIGHT",
    "PUSH_FLOOR_UP",
    "PUSH_FLOOR_DOWN",
    "PUSH_FLOOR_LEFT",
    "PUSH_FLOOR_RIGHT",
    "THIN_WALL_TOP",
    "THIN_WALL_BOT",
    "THIN_WALL_LEFT",
    "THIN_WALL_RIGHT",
    "TELEPORTER",
    "FIRE_BOOTS",
    "FLIPPERS",
    "ICE_SKATES",
    "SUCTION_BOOTS",
]

DEFAULT_ENTITY_TYPES = [
    "CHIPPIE",
    "BLUE_TANK_THAT_MOVES_DOWN",
    "BLUE_TANK_THAT_MOVES_LEFT",
    "BLUE_TANK_THAT_MOVES_UP",
    "BLUE_TANK_THAT_MOVES_RIGHT",
    "WATER_GLIDER",
    "FIRE_DANCER",
    "PURPLE_BALL",
    "CYAN_STICK_BALL",
    "BACTERIA",
    "FROG_MONSTER",
    "CENTIPEDE",
    "MOVEABLE_BLOCK",
]

DEFAULT_FACINGS = [
    "LEFT",
    "RIGHT",
    "UP",
    "DOWN",
]

# TERRAIN_TYPES = DEFAULT_TERRAIN_TYPES
# ENTITY_TYPES = DEFAULT_ENTITY_TYPES
# FACINGS = DEFAULT_FACINGS
TERRAIN_TYPES = []
ENTITY_TYPES = []
FACINGS = []

class MapImage():
    TILELEN = 32
    MAPLEN = 32

    def __init__(self, file_path):
        self.image = np.asarray(Image.open(file_path).crop((0, 0, self.TILELEN*self.MAPLEN, self.TILELEN*self.MAPLEN)))

    def get_tile_as_array(self, tile_xy):
        pixelx, pixely = self._to_pixel_xy(tile_xy)
        return self.image[pixely:pixely+self.TILELEN, pixelx:pixelx+self.TILELEN, ...]
    
    def _to_pixel_xy(self, tile_xy):
        return tile_xy[0] * self.TILELEN, tile_xy[1] * self.TILELEN

class TileCache():
    def __init__(self, cache_file=None):
        # a dict where key is the hash of the tile and value is the annotation
        # { 
        #  hash for tile 1: str,
        #  hash for tile 2: str,
        #  ...
        # ]
        self.annotations = {} if cache_file is None else self.load(cache_file)

    def find(self, tile_sample):
        '''
        Returns the annotation if it finds the tile sample
        Otherwise, returns None
        '''
        tile_hash = self._hash(tile_sample)
        if tile_hash in self.annotations:
            return self.annotations[tile_hash]
        return None

    def register(self, tile_sample, string_annotation):
        tile_hash = self._hash(tile_sample)
        self.annotations[tile_hash] = string_annotation
    
    def load(self, input_path):
        with open(input_path, 'rb') as fid:
            return pickle.load(fid)

    def save(self, output_path):
        with open(output_path, 'wb') as fid:
            pickle.dump(self.annotations, fid)

    def _hash(self, sample):
        hashobj = hashlib.sha256()
        hashobj.update(sample.tobytes())
        return hashobj.digest()

class UserImageDialog():
    def __init__(self, imagedata):
        self.data = imagedata
    
    def ask(self):
        h = plt.figure(1)
        plt.imshow(self.data)
        plt.title("What is this?")
        plt.ion()
        plt.show()
        plt.pause(0.001)
        result = self._process_tree()
        return result
    
    def _process_tree(self):
        print("Press 'h' for commands")
        terrain = None
        entity = None
        while True:
            print("Commands are q - quit, t - terrain, e - entity, nt - new terrain, ne - new entity")
            response = input("> ")
            if response == 'q':
                if terrain is None:
                    print("can't quit yet!  A terrain is required.  Please enter one.")
                    continue
                else:
                    break
            if response == 't':
                terrain = self._process_terrain() 
                continue
            if response == 'e':
                entity = self._process_entity()
                continue
            if response == 'nt':
                self._process_new_terrain()
                continue
            if response == 'ne':
                self._process_new_entity()
                continue
            
        def combine_results(user_t, user_e):
            result = user_t
            if user_e is not None:
                result += f"+{user_e}"
            return result

        final = combine_results(terrain, entity)
        print(f"adding {final}")
        return final
    
    def _process_new_terrain(self):
        while True:
            print("Enter the new terrain identifier.  It should be in all caps (I'm not going to check, though)")
            newt = input("> ")
            print(f"You entered '{newt}'.  Enter [y/n] to confirm or cancel.")
            response = input("> ")
            if response == 'y':
                TERRAIN_TYPES.append(newt)
                break;

    def _process_new_entity(self):
        while True:
            print("Enter the new entity identifier.  It should be in all caps (I'm not going to check, though)")
            newt = input("> ")
            print(f"You entered '{newt}'.  Enter [y/n] to confirm or cancel.")
            response = input("> ")
            if response == 'y':
                ENTITY_TYPES.append(newt)
                break;

    def _process_terrain(self):
        return self._general_process(TERRAIN_TYPES)
    
    def _process_entity(self):
        entity_str = self._general_process(ENTITY_TYPES)

        print("Now, select a facing:")
        facing_str = self._general_process(FACINGS)

        return f"{entity_str}_{facing_str}"
    
    def _general_process(self, listing):
        while True:
            print("Select one of the following:")
            for idx, terr in enumerate(listing):
                print(f"  {idx}) {terr}")
            idx = input("> ")
            try:
                idx = int(idx)
                if idx < 0 or (not idx < len(listing)):
                    print(f"Invalid: {idx} out of range [0, {len(listing)-1}].  Try again.")
                    continue
            except:
                print(f"Invalid: {idx} is non-numeric.  Try again.")
                continue
            break
        return listing[idx]


class ResultReport():
    '''
    In level_*.cpp:

    #include "level_*.hpp"
    #include "chippie/entities/chippie_entity.hpp"
    #include <cstring>

    namespace chippie::level_*
    {
    namespace 
    {
        constexpr int TIME_REMAINING{ 0 };
        constexpr int CHIPS_REMAINING{ 0 };
        constexpr const char* HINT_TEXT{ nullptr };
        constexpr const char* LEVEL_NAME_TEXT{ nullptr };

        constexpr std::array DATA
        {
            terrain_type::WALL, terrain_type::WALL, etc. for the whole 1024 tiles.
        };

    }

    void load_level_from_rom_stub(state &game_state) noexcept
    {
        /* set the time limit, if any */
        game_state.time_remaining = TIME_REMAINING;
        game_state.countdown_timer.reset();

        /* set the amount of chips for the level */
        game_state.chippie_inventory.set(inventory_item::CHIPS, CHIPS_REMAINING);

        /* set the hint string, which can be null */
        game_state.hint_text = HINT_TEXT;
        /* set the level name */
        game_state.level_name_text = LEVEL_NAME_TEXT;

        /* load the map data */
        std::memcpy(std::data(game_state.the_map.map_data), std::data(DATA), std::size(DATA) * sizeof(terrain_type));
    }

    void load_entity_list_from_rom_stub(state &game_state) noexcept
    {
        game_state.entity_list.clear();

        // by convention, Chippie is always the first one in the entity_list
        game_state.entity_list.push_back(chippie::create(game_state, {.x = 15, .y = 14}, direction::DOWN, 0));
    }

    }
    '''
    def __init__(self, level):
        self.terrain = np.zeros((32,32), dtype=np.int8) + TERRAIN_TYPES.index("CLEAR")
        self.entities = [] 
        self.level = level

    def add(self, annotation, xy):
        terrain, ent = self._parse(annotation)
        if ent is not None:
            self.entities.append([ent, xy])
        self.terrain[xy[1], xy[0]] = TERRAIN_TYPES.index(terrain)

    def save(self, source_path, header_path):
        with open(source_path, 'w') as fid:
            fid.write(self._source())
        with open(header_path, 'w') as fid:
            fid.write(self._header())
        with open("level.hpp", "w") as fid:
            fid.write(self._master_header())
        with open("level.cpp", "w") as fid:
            fid.write(self._master_source())

    def _parse(self, string):
        if '+' in string:
            items = string.split('+')
            return items[0], items[1]
        else:
            return string, None
    
    def _stringify_terrain(self, terrain):
        return f"terrain_type::{terrain}"
    
    def _stringify_entity_creation(self, entity, xy):
        if '_' in entity:
            items = entity.split('_')
            ent_name = '_'.join(items[0:-1]).lower()

            # handle the exceptional case...
            if ent_name == "moveable_block":
                ent_name = "moveable_block_entity"

            facing = items[-1]
            return f"{ent_name}::create(game_state, {{.x = {xy[0]}, .y = {xy[1]}}}, direction::{facing}, uuid++)"
    
    def _resolve_terrain(self):
        result = ''
        for yy in range(32):
            result += '        '
            for xx in range(32):
                terr = TERRAIN_TYPES[self.terrain[yy, xx]]
                result += f"{self._stringify_terrain(terr)}, "
            result += '\n'
        return result
    
    def _resolve_entity_list(self):
        chippie = [ent for ent in self.entities if "CHIPPIE" in ent[0]]
        the_rest = [ent for ent in self.entities if "CHIPPIE" not in ent[0]]

        result = ''
        if len(chippie) > 0:
            chipp = chippie[0]
            print(f"BNP chippie is {chipp}")
            result += f"    game_state.entity_list.push_back("
            result += self._stringify_entity_creation(chipp[0], chipp[1])
            result += f");\n"

        for ent in the_rest:
            print(f"BNP other is {ent}")
            result += f"    game_state.entity_list.push_back("
            result += self._stringify_entity_creation(ent[0], ent[1])
            result += f");\n"
        
        # throw in a static assert, just for good measure
        result += f"    static_assert(decltype(game_state.entity_list){{}}.capacity() >= {len(self.entities)});\n"

        # we'll also throw in static asserts for red button, brown button, and teleporter lists
        # as well as helpful messages that these lists need manual updating
        def count_terrain_type(terrain):
            acc = 0
            for yy in range(32):
                for xx in range(32):
                    if terrain == TERRAIN_TYPES[self.terrain[yy, xx]]:
                        acc = acc + 1
            return acc
        
        number_of_red_buttons = count_terrain_type("RED_BUTTON")
        number_of_brown_buttons = count_terrain_type("BROWN_BUTTON")
        number_of_teleporters = count_terrain_type("TELEPORTER")

        if number_of_red_buttons > 0:
            for idx in range(0, number_of_red_buttons):
                result += f"    game_state.red_button_list.push_back(red_button{{\n"
                result += f"        .button = {{.x = <INT>, .y = <INT>}},\n"
                result += f"        .clone_spawn = {{.x = <INT>, .y = <INT>}},\n"
                result += f"        .clone_facing = <direction::>,\n"
                result += f"        .clone_type = <entity_type::>,\n"
                result += f"    }});\n"
            result += f"    static_assert(decltype(game_state.red_button_list){{}}.capacity() >= {number_of_red_buttons});\n"
            result += f"#error \"Just a friendly reminder to fill in red_button_list :)\"\n"
        if number_of_brown_buttons > 0:
            for idx in range(0, number_of_brown_buttons):
                result += f"    game_state.brown_button_list.push_back(brown_button{{\n"
                result += f"        .button = {{.x = <INT>, .y = <INT>}},\n"
                result += f"        .trap = {{.x = <INT>, .y = <INT>}},\n"
                result += f"    }});\n"
            result += f"    static_assert(decltype(game_state.brown_button_list){{}}.capacity() >= {number_of_brown_buttons});\n"
            result += f"#error \"Just a friendly reminder to fill in brown_button_list :)\"\n"
        if number_of_teleporters > 0:
            for idx in range(0, number_of_brown_buttons):
                result += f"    game_state.teleport_list.push_back(teleporter{{.entry_location = {{.x = <INT>, .y = <INT> }}}});\n"
                result += f"    game_state.teleport_list.back().set_exit(<direction::>, {{.x = <INT>, .y = <INT>}}, <direction::>);\n"
                result += f"    // plus other directions, if needed\n"
                result += f"\n"
            result += f"    static_assert(decltype(game_state.teleport_list){{}}.capacity() >= {number_of_teleporters});\n"
            result += f"#error \"Just a friendly reminder to fill in teleport_list :)\"\n"

        return result


    def _header(self):
        result =  "/* AUTO GENERATED BY process_map_image.py */\n"
        result += f"#ifndef CHIPPIE_LEVEL_{self.level}_HPP\n"
        result += f"#define CHIPPIE_LEVEL_{self.level}_HPP\n"
        result += f"\n"
        result += f"#include \"chippie/state/state.hpp\"\n"
        result += f"\n"
        result += f"namespace chippie::level_{self.level}\n"
        result += f"{{\n"
        result += f"    void load_level(state&) noexcept;\n"
        result += f"}}\n"
        result += f"#endif\n"
        return result

    def _source(self):
        result =  "/* AUTO GENERATED BY process_map_image.py */\n"
        result += f"#include \"level_{self.level}.hpp\"\n"
        result += f"#include \"chippie/entities/entities.hpp\"\n"
        result += f"#include <cstring>\n"
        result += f"\n"
        result += f"namespace chippie::level_{self.level}\n"
        result += f"{{\n"
        result += f"namespace\n"
        result += f"{{\n"
        result += f"    constexpr int TIME_REMAINING{{ 0 }};\n"
        result += f"    constexpr int CHIPS_REMAINING{{ 0 }};\n"
        result += f"    constexpr const char* HINT_TEXT{{ nullptr }};\n"
        result += f"    constexpr const char* LEVEL_NAME_TEXT{{ nullptr }};\n"
        result += f"#error \"Just a friendly reminder to fill in time remaining, chips remaining, etc :)\"\n"
        result += f"\n"
        result += f"    /* clang-format off */\n"
        result += f"    constexpr std::array DATA\n"
        result += f"    {{\n"
        result += f"{self._resolve_terrain()}"
        result += f"    }};\n"
        result += f"    /* clang-format on */\n"
        result += f"\n"

        result += f"void load_level_from_rom_stub(state &game_state) noexcept\n"
        result += f"{{\n"
        result += f"    /* set the time limit, if any */\n"
        result += f"    game_state.time_remaining = TIME_REMAINING;\n"
        result += f"    game_state.countdown_timer.reset();\n"
        result += f"\n"
        result += f"    /* set the amount of chips for the level */\n"
        result += f"    game_state.chippie_inventory.set(inventory_item::CHIPS, CHIPS_REMAINING);\n"
        result += f"\n"
        result += f"    /* set the hint string, which can be null */\n"
        result += f"    game_state.hint_text = HINT_TEXT;\n"
        result += f"    /* set the level name */\n"
        result += f"    game_state.level_name_text = LEVEL_NAME_TEXT;\n"
        result += f"\n"
        result += f"    /* load the map data */\n"
        result += f"    std::memcpy(std::data(game_state.the_map.map_data), std::data(DATA), std::size(DATA) * sizeof(terrain_type));\n"
        result += f"}}\n"
        result += f"\n"
    
        result += f"void load_entity_list_from_rom_stub(state &game_state) noexcept\n"
        result += f"{{\n"
        result += f"    game_state.entity_list.clear();\n"
        result += f"    uint8_t uuid = 0;\n"
        result += f"\n"
        result += f"    // by convention, Chippie is always the first one in the entity_list\n"
        result += f"{self._resolve_entity_list()}\n"
        result += f"}}\n"
        result += f"\n"
        result += f"}}\n" # closes the anonomous namespace

        result += f"void load_level(state &game_state) noexcept\n"
        result += f"{{\n"
        result += f"    load_level_from_rom_stub(game_state);\n"
        result += f"    load_entity_list_from_rom_stub(game_state);\n"
        result += f"}}\n"
        result += f"\n"
        result += f"}}\n" # closes the chippie::level_* namespace
        result += f"\n"

        return result
    
    def _master_header(self):
        result =  "/* AUTO GENERATED BY process_map_image.py */\n"
        result += "#ifndef LEVEL_HPP\n"
        result += "#define LEVEL_HPP"
        result += "\n"
        result += "#include \"chippie/state/state.hpp\"\n"
        result += "\n"
        result += "namespace chippie::level\n"
        result += "{\n"
        result += "    [[nodiscard]] uint32_t get_max_level() noexcept;\n"
        result += "    void load(state& game_state, uint32_t level) noexcept;\n"
        result += "}\n"
        result += "\n"
        result += "#endif"

        return result


    def _master_source(self):
        '''
        #include "level.hpp"

        #include <cmath>
        #include <array>
        #include <cstdint>

        #include "level_1.hpp"
        #include "level_2.hpp"
        ...
        #include "level_{self.level}.hpp"

        namespace chippie
        {

        namespace {

        constexpr std::array jump_table {
            level_1::load_level,
            level_2::load_level,
            ...
            level_{self.level}::load_level
        };

        }

        namespace level
        {
            uint32_t get_max_level() noexcept
            {
                return std::size(jump_table);
            }

            void load(state& game_state, uint32_t level) noexcept
            {
                level = std::min(std::size(jump_table)-1, std::max(1U, level));
                jump_table[level - 1](game_state);
            }
        }

        }

        '''

        def gen_includes(levels):
            result = ''
            for idx in range(0, self.level):
                result += f"#include \"level_{idx+1}.hpp\"\n"
            return result

        def gen_jump_table_contents(levels):
            result = ''
            for idx in range(0, self.level):
                result += f"    level_{idx+1}::load_level,\n"
            return result
        
        sresult =  "/* AUTO GENERATED BY process_map_image.py */\n"
        sresult += "#include \"level.hpp\"\n"
        sresult += "\n"
        sresult += "#include <cmath>\n"
        sresult += "#include <array>\n"
        sresult += "#include <cstdint>\n"
        sresult += "\n"
        sresult += f"{gen_includes(self.level)}"
        sresult += "\n"
        sresult += "namespace chippie\n"
        sresult += "{\n"
        sresult += "\n"
        sresult += "namespace\n"
        sresult += "{\n"
        sresult += "\n"
        sresult += "constexpr std::array jump_table {\n"
        sresult += f"{gen_jump_table_contents(self.level)}"
        sresult += "};\n"
        sresult += "\n"
        sresult += "}\n" # closes anon namespace
        sresult += "\n"
        sresult += "namespace level\n"
        sresult += "{\n"
        sresult += "uint32_t get_max_level() noexcept\n"
        sresult += "{\n"
        sresult += "    return std::size(jump_table);\n"
        sresult += "}\n"
        sresult += "\n"
        sresult += "void load(state& game_state, uint32_t level) noexcept\n"
        sresult += "{\n"
        sresult += "    level = std::min(static_cast<uint32_t>(std::size(jump_table)), std::max(uint32_t{1U}, level));\n"
        sresult += "    jump_table[level - 1](game_state);\n"
        sresult += "}\n"
        sresult += "}\n" # closes level namespace
        sresult += "}\n" # closes chippie namespace

        return sresult 


def load_existing(filepath):
    return filepath if os.path.exists(filepath) else None

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="whatever")
    parser.add_argument("IMAGE", type=str, help="Image of the map from that german website")
    parser.add_argument("LEVEL", type=int, help="Level number")
    parser.add_argument("--meta", type=str, default='meta.pkl')
    parser.add_argument("--tiles", type=str, default='cache.pkl')
    args = parser.parse_args()

    if os.path.exists(args.meta) is True:
        with open(args.meta, 'rb') as fid:
            meta = pickle.load(fid)
            TERRAIN_TYPES = meta['TERRAIN_TYPES']
            ENTITY_TYPES = meta['ENTITY_TYPES']
            FACINGS = meta['FACINGS']
            print(f"terrain types: {TERRAIN_TYPES}")
            print(f"entity types: {ENTITY_TYPES}")
            print(f"facings: {FACINGS}")

    mapimg = MapImage(args.IMAGE)

    cache = TileCache(load_existing(args.tiles))

    result = ResultReport(args.LEVEL)



    '''
    What are we trying to accomplish?
    The idea is to input a well-specified image of the map and output a std::array<terrain_type> 
    of the map, such that a simple memcpy of that array to the static_map object in state is all
    that's required.

    What's the process?

    We load the map image
    We then iterate through each tile location on the map
        Take a slice of this tile from the map image
        Compare it against the annotated samples stored in the file cache
        If it matches, add that annotation to the output std::array<terrain_type> map artifact
        If no match found, ask the user for input:
            Display the tile
            Ask user to identify what this tile is
                Select from a controlled list
                Add an option for "don't know", which will quit the processing immediately
    '''

    for yy in range(0, mapimg.MAPLEN):
        for xx in range(0, mapimg.MAPLEN):
            tile = mapimg.get_tile_as_array((xx,yy))

            item = cache.find(tile)
            if item is None:
                dialog = UserImageDialog(tile)
                annotation = dialog.ask()
                cache.register(tile, annotation)
                item = cache.find(tile)

            result.add(item, (xx,yy))
    
    result.save(f"level_{args.LEVEL}.cpp", f"level_{args.LEVEL}.hpp")
    cache.save(args.tiles)

    with open(args.meta, 'wb') as fid:
        meta = {}
        meta['TERRAIN_TYPES'] = TERRAIN_TYPES
        meta['ENTITY_TYPES'] = ENTITY_TYPES
        meta['FACINGS'] = FACINGS
        pickle.dump(meta, fid)

else:
    pass