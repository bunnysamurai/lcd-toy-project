# Intro
A toy project, to play around with the rp2040 driver [found here](https://dmitry.gr/?r=06.%20Thoughts&proj=09.ComplexPioMachines) for this [LCD kit](https://www.waveshare.com/pico-restouch-lcd-2.8.htm).

# Getting All Setup
Requires:
* [Pico SDK](https://github.com/raspberrypi/pico-sdk)
* [PIO machinery](https://dmitry.gr/images/pioMachinery.zip), which is already in [driver](driver).
* [A Pico](https://www.digikey.com/short/00m5w7tv)
* An [LCD from Waveshare](https://www.waveshare.com/pico-restouch-lcd-2.8.htm)

Install the Pico SDK, then build:
```bash
$ cmake -S. -Bbuild && cmake --build build
```

# Think thoughts and TODOs

* Add a Shell class, which encapsulates what StaticCommandLine is trying to do, but more cleanly and with a better interface?
  * Ability to register commands at runtime
  * Can execute any of those commands
  * Front line for handling keyboard input
* Either the TileBuffer or the SPI LCD driver itself needs to handle reorientating the characters on the display
  * The logic lives in the TextConsole class
  * I'm thinking it can move to TileBuffer
* TileBuffer needs a rethink
  * A bit too complicated, I feel it's doing too much
  * Drawing a tile should be "super straightforward"
  * Might just store a buffer/linked-list of tile references
  * Then, at the right time, someone reads through the linked list and renders the tile objects
    * "Render" here means converting the tile object to a pixel map
      * Might happen a line at a time, so someone defines how to serialize a line of the tile that TileBuffer will depend
    * That pixel map then gets written directly to the correct location in the raw video buffer
    * Needs to handle all the video formats (1bpp, 2bpp, etc)
    * Here's a process for serialization:
      * The Tile tells TileBuffer how many pixels tall and wide it is
      * TileBuffer knows the pixel format of the display
      * Using the bpp format, TileBuffer will set aside a tile buffer just large enough to hold the rendered tile.
      * TileBuffer will read each pixel in Tile and store it to it's tile buffer, doing an necessary conversions
        * How to read the pixel and how to convert it can be the responsibility of the Tile
        * However, what formats are possible is defined by TileBuffer
        * Both Tile and TileBuffer will depend on a standard definition of pixel formats
      * This tile buffer gets written directly to the raw video buffer
