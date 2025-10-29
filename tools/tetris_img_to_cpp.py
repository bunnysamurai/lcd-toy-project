#!/usr/bin/env python3

import argparse
import numpy as np
from PIL import Image
from matplotlib import pyplot as plt



# for tetris, specifically
pallete_map = {
        "BLACK": [0, 0,  0],
        "RED": [255, 0,  0],
        "GREEN": [0, 220,  0],
        "BLUE": [0, 0,  255],
        "CYAN": [0, 220,  220],
        "YELLOW": [220, 220,  0],
        "MAGENTA": [245, 0,  245],
        "DRKGRY": [84, 84,  84],
        "WHITE": [255, 255,  255],
        "LRED": [255, 128,  128],
        "LGREEN": [200, 255,  200],
        "LBLUE": [128, 128,  255],
        "LCYAN": [200, 255,  255],
        "LYELLOW": [255, 255,  200],
        "LMAGENTA": [255, 128,  255],
        "LGREY": [188, 188,  188]
    }

def to_numpy(img):
    w = img.width
    h = img.height
    result = np.array(img.getdata())
    if img.mode == "RGB":
        return np.reshape(result, (h,w,3))
    else:
        result = np.reshape(result, (h,w,4))
        return result[...,0:-1]

def convert_to_pallete_identifier(mapping, rgb_tuple_like_thing):
    for key in mapping:
        value = mapping[key]
        if np.all(rgb_tuple_like_thing == value):
            return key
    return None

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="bespoke tool for easing creating of larger tiles for the MevenBoy")
    parser.add_argument("PNG", help="path to a .png file", type=str)
    parser.add_argument("NAME", help="identifier for the array", type=str)

    args = parser.parse_args()

    img = to_numpy(Image.open(args.PNG))

    name = args.NAME
    header = f'#ifndef {name}_HPP\n#define {name}_HPP\n\n#include <array>\n#include "common/utilities.hpp"\n\ninline constexpr std::array {name} {{ embp::pfold('

    # convert image data to a long list of pallete identifier strings
    resultstring = header
    for row in range(0,img.shape[0]):
        resultstring += '\n'
        for col in range(0,img.shape[1]):
            # find a match
            # add this to the lookup
            resultstring += f"{convert_to_pallete_identifier(pallete_map, img[row,col,:])},"
    resultstring = resultstring[:-1]
    resultstring += '\n)};'

    # now add the tile struct

    resultstring +=f"\n\ninline constexpr screen::Tile {name}_TILE {{\n    .side_length = {img.shape[0]},\n    .format = VIDEO_FORMAT,\n    .data = std::data({name})\n}};"

    # and close the header guard
    resultstring += '\n#endif'

    print(resultstring)
