#include "bsp.h"

uint8_t pinLcdBacklight1 = 14;
uint8_t pinLcdBacklight2 = 22;
uint8_t pinLcdBacklight3 = 23;

void platform_initBacklight()
{
	nrf_gpio_cfg_output(pinLcdBacklight1);
	nrf_gpio_cfg_output(pinLcdBacklight2);
	nrf_gpio_cfg_output(pinLcdBacklight3);
	platform_setBacklight(3);
}

void platform_setBacklight(short level)
{
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
