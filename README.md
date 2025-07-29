# Intro

A toy project, to play around with the rp2040 driver [found here](https://dmitry.gr/?r=06.%20Thoughts&proj=09.ComplexPioMachines) for this [LCD kit](https://www.waveshare.com/pico-restouch-lcd-2.8.htm).

# Getting All Setup

Requires:
* [Pico SDK](https://github.com/raspberrypi/pico-sdk)
* [PIO machinery](https://dmitry.gr/images/pioMachinery.zip), which is already in [driver](driver).
* [A Pico](https://www.digikey.com/short/00m5w7tv)
* An [LCD from Waveshare](https://www.waveshare.com/pico-restouch-lcd-2.8.htm)

# Building

Install the Pico SDK (to `~/sw/pico-sdk`, for example), then build:
```bash
$ cmake -G Ninja -S. -Bbuild -DCMAKE_BUILD_TYPE=Release -DPICO_SDK_PATH=~/sw/pico-sdk
$ cmake --build build
```

## configurations

By default, keyboard inputs are processed via tinyusb, with stdout being directed to the waveshare screen.

### `-DBUILD_WITH_STDIO_USB`

* Instead, shell will run in a terminal, and keyboard inputs accepted via the terminal.  Access it via your favourite program, for example:
  * `$ minicom -D /dev/ttyACM0`
* Useful for development and debugging.

## reports

Uses `elf-size-analyze` from [jedrzejbocar](https://github.com/jedrzejboczar/elf-size-analyze) and `size` from GNU.

# Think thoughts and TODOs

* Modify the waveshare driver to include options for 2bit and 4bit color LUTs