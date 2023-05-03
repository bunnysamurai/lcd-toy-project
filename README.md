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
