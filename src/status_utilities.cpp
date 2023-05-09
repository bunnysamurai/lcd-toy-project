#include "status_utilities.hpp"

#include "pico/stdlib.h"

BlinkStatus::BlinkStatus(Milliseconds period_ms) : m_period{period_ms}
{
    init_gpio();
}

void BlinkStatus::blink_forever()
{
    while (true)
    {
        sleep_ms(m_period.value);
        m_state ^= 0x01;
        gpio_put(PICO_DEFAULT_LED_PIN, m_state);
    }
}

void BlinkStatus::init_gpio()
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}