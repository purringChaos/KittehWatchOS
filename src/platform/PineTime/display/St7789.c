#include "St7789.h"
#include "display/St7789.h"
#include "display/spi.h"
#include "types/numbers.h"
#include <FreeRTOS.h>
#include <hal/nrf_gpio.h>
#include <libraries/delay/nrf_delay.h>
#include <lvgl/lvgl.h>
#include <task.h>

void St7789_Init() {
  // Set all the output pins
  nrf_gpio_cfg_output(25);
  nrf_gpio_pin_set(25);
  nrf_gpio_cfg_output(18);
  nrf_gpio_cfg_output(26);
  nrf_gpio_pin_set(26);

  // turn it off and on again
  nrf_gpio_pin_clear(26);
  nrf_delay_ms(100);
  nrf_gpio_pin_set(26);
  nrf_delay_ms(100);
  // reset controller
  St7789_WriteCommand(0x01);
  nrf_delay_ms(150);
  // Get out of sleep, wake the fuck up.
  St7789_WriteCommand(0x11);
  // Set colour mode
  St7789_WriteCommand(0x3a);
  St7789_WriteSingleData(0x55); // 0x55 = 16 colour mode
  nrf_delay_ms(10);
  // Allow memory access.
  St7789_WriteCommand(0x1e);
  St7789_WriteSingleData(0x00);
  // invert display
  St7789_WriteCommand(0x21);
  nrf_delay_ms(10);
  // set to normal mode
  St7789_WriteCommand(0x13);
  nrf_delay_ms(10);
  // Turn it on!
  St7789_WriteCommand(0x29);
}

void St7789_WriteCommand(u8 cmd) {
  nrf_gpio_pin_clear(18);
  SPI_Write(25, &cmd, 1);
}

void St7789_WriteSingleData(u8 data) {
  nrf_gpio_pin_set(18);
  SPI_Write(25, &data, 1);
}

void St7789_WriteData(u8 *data, size_t size) {
  nrf_gpio_pin_set(18);
  SPI_Write(25, data, size);
}

void St7789_Flush(lv_disp_drv_t *driver, const lv_area_t *area,
                  lv_color_t *colour_data) {
  ulTaskNotifyTake(pdTRUE, 500);
  u8 data[4] = {0};
  u8 x1 = area->x1;
  u8 x2 = area->x2;
  u8 y1 = area->y1;
  u8 y2 = area->y2;
  // Set Columns
  St7789_WriteCommand(0x2a);
  data[0] = (x1 >> 8) & 0xFF;
  data[1] = x1 & 0xFF;
  data[2] = (x2 >> 8) & 0xFF;
  data[3] = x2 & 0xFF;
  St7789_WriteData(data, 4);
  // Set Rows
  St7789_WriteCommand(0x2b);
  data[0] = (y1 >> 8) & 0xFF;
  data[1] = y1 & 0xFF;
  data[2] = (2 >> 8) & 0xFF;
  data[3] = y2 & 0xFF;
  St7789_WriteData(data, 4);
  // Sending colour data.
  St7789_WriteCommand(0x2c);
  u32 size = lv_area_get_width(area) * lv_area_get_height(area);
  St7789_WriteData((void *)colour_data, size * 2);
  lv_disp_flush_ready(driver);
}