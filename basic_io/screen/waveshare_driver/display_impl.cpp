// TODO This file will be directly included by the build configuration... is this a good idea? WHO KNOWS!

// TODO consider not using the Pico SDK?
#include "pico/stdlib.h"

#include "dispWaveshareLcd.h"
#include "pinout.h"

namespace
{
    void setup_for_input(uint id)
    {
        gpio_init(id);
        gpio_set_dir(id, false);
    }
    void setup_for_output(uint id)
    {
        gpio_init(id);
        gpio_set_dir(id, true);
    }
}

  bool init(const uint8_t *video_buf, uint8_t bpp)
{
    bool status{true};
    setup_for_output(PIN_TOUCH_CS);
    setup_for_output(PIN_LCD_RESET);
    setup_for_output(PIN_LCD_DnC);
    setup_for_output(PIN_LCD_CS);
    setup_for_output(PIN_SPI_CLK);
    setup_for_output(PIN_SPI_MOSI);
    setup_for_input(PIN_SPI_MISO);
    setup_for_output(PIN_LCD_BL);

    status &= dispInit(video_buf, 1);
    dispSetDepth(bpp);

    // printf("INFO: Turning on backlight\n");

    gpio_put(PIN_LCD_BL, true);

    return status;
}