#include "platform/includes/backlight.h"
#include "types/numbers.h"
#include <hal/nrf_gpio.h>

u8 pinLcdBacklight1 = 14;
u8 pinLcdBacklight2 = 22;
u8 pinLcdBacklight3 = 23;

void platform_initBacklight() {
  nrf_gpio_cfg_output(pinLcdBacklight1);
  nrf_gpio_cfg_output(pinLcdBacklight2);
  nrf_gpio_cfg_output(pinLcdBacklight3);
  platform_setBacklight(3);
}

void platform_setBacklight(u8 level) {
  switch (level) {
  default:
  case 3:
    nrf_gpio_pin_clear(pinLcdBacklight1);
    nrf_gpio_pin_clear(pinLcdBacklight2);
    nrf_gpio_pin_clear(pinLcdBacklight3);
    break;
  case 2:
    nrf_gpio_pin_clear(pinLcdBacklight1);
    nrf_gpio_pin_clear(pinLcdBacklight2);
    nrf_gpio_pin_set(pinLcdBacklight3);
    break;
  case 1:
    nrf_gpio_pin_clear(pinLcdBacklight1);
    nrf_gpio_pin_set(pinLcdBacklight2);
    nrf_gpio_pin_set(pinLcdBacklight3);
    break;
  case 0:
    nrf_gpio_pin_set(pinLcdBacklight1);
    nrf_gpio_pin_set(pinLcdBacklight2);
    nrf_gpio_pin_set(pinLcdBacklight3);
    break;
  }
}
