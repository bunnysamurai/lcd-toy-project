#include "screen.hpp"

// TODO consider not using the Pico SDK?
#include "pico/stdlib.h"

#include "waveshare_driver/dispWaveshareLcd.h"
#include "waveshare_driver/pinout.h"

namespace screen
{
    namespace
    {
        void setup_for_input(uint id) noexcept
        {
            gpio_init(id);
            gpio_set_dir(id, false);
        }
        void setup_for_output(uint id) noexcept
        {
            gpio_init(id);
            gpio_set_dir(id, true);
        }

        constexpr DispDimensions_t PHYSICAL_CREEN_SIZE{.width = PHYSICAL_SIZE.width, .height = PHYSICAL_SIZE.height};

        [[nodiscard]] constexpr bool range_check_dimensions(Dimensions testdim) noexcept
        {
            return testdim.width <= PHYSICAL_SIZE.width && testdim.height <= PHYSICAL_SIZE.height;
        }
    }

    Format get_format() noexcept { return {.bpp = dispGetDepth()}; }
    Dimensions get_virtual_screen_size() noexcept
    {
        const auto dims{dispGetVirtualDimensions()};
        return {.width = dims.width, .height = dims.height};
    }
    void set_virtual_screen_size([[maybe_unused]] Position new_topleft, Dimensions new_size) noexcept
    {
        dispSetVirtualDimensions({.width = new_size.width, .height = new_size.height});
    }

    const uint8_t *get_video_buffer() noexcept
    {
        return dispGetVideoBuffer();
    }

    bool init(const uint8_t* video_buf, [[maybe_unused]] Position virtual_topleft, Dimensions virtual_size, Format format) noexcept
    {
        if (!range_check_dimensions(virtual_size))
        {
            return false;
        }

        bool status{true};

        setup_for_output(PIN_TOUCH_CS);
        setup_for_output(PIN_LCD_RESET);
        setup_for_output(PIN_LCD_DnC);
        setup_for_output(PIN_LCD_CS);
        setup_for_output(PIN_SPI_CLK);
        setup_for_output(PIN_SPI_MOSI);
        setup_for_input(PIN_SPI_MISO);
        setup_for_output(PIN_LCD_BL);

        status &= dispInit(video_buf, format.bpp, {.width = virtual_size.width, .height = virtual_size.height}, PHYSICAL_SIZE);

        // printf("INFO: Turning on backlight\n");

        gpio_put(PIN_LCD_BL, true);

        return status;
    }

    void set_video_buffer(const uint8_t *buffer) noexcept
    {
        dispSetVideoBuffer(buffer);
    }
}