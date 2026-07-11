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

TERRAIN_TYPES = DEFAULT_TERRAIN_TYPES
ENTITY_TYPES = DEFAULT_ENTITY_TYPES
FACINGS = DEFAULT_FACINGS

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
        return self._process_tree()
    
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
                terrain = self._process_new_terrain()
                continue
            if response == 'ne':
                entity = self._process_new_entity()
                continue
            
        def combine_results(user_t, user_e):
            result = user_t
            if user_e is not None:
                result += f"+{user_e}"
            return result

        return combine_results(terrain, entity)
    
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
    def __init__(self):
        pass

    def add(self, annotation, xy):
        pass

    def save(self, filepath):
        pass

def load_existing(filepath):
    return filepath if os.path.exists(filepath) else None

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="whatever")
    parser.add_argument("IMAGE", type=str, help="Image of the map from that german website")
    args = parser.parse_args()

    mapimg = MapImage(args.IMAGE)

    cache = TileCache(load_existing("cache.pkl"))

    result = ResultReport()


    if os.path.exists('meta.pkl') is True:
        with open('meta.pkl', 'rb') as fid:
            meta = pickle.load(fid)
            TERRAIN_TYPES = meta['TERRAIN_TYPES']
            ENTITY_TYPES = meta['ENTITY_TYPES']
            FACINGS = meta['FACINGS']

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
    
    result.save("output.cpp")
    cache.save("cache.pkl")

    with open('meta.pkl', 'wb') as fid:
        meta = {}
        meta['TERRAIN_TYPES'] = TERRAIN_TYPES
        meta['ENTITY_TYPES'] = ENTITY_TYPES
        meta['FACINGS'] = FACINGS
        pickle.dump(meta, fid)

else:
    pass