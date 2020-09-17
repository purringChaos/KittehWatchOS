#include "display/St7789.h"
#include "display/spi.h"
#include "platform/includes/thread.h"
#include "types/numbers.h"
#include <FreeRTOS.h>
#include <hal/nrf_gpio.h>
#include <libraries/delay/nrf_delay.h>
#include <lvgl/lvgl.h>
#include <assert.h>

// Some init stuff was taken from https://github.com/lvgl/lv_port_esp32

typedef struct {
  u8 cmd;
  u8 data[16];
  u8 databytes; // No of data in data; bit 7 = delay after set; 0xFF = end of
                // cmds.
} lcd_init_cmd_t;

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

  lcd_init_cmd_t st7789_init_cmds[] = {
      {0xCF, {0x00, 0x83, 0X30}, 3},             // Unknown mystery bytes
      {0xED, {0x64, 0x03, 0X12, 0X81}, 4},       // Unknown mystery bytes
      {0xE8, {0x85, 0x01, 0x79}, 3},             // Set power control 2
      {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5}, // Unknown mystery bytes
      {0xF7, {0x20}, 1},                         // Unknown mystery bytes
      {0xEA, {0x00, 0x00}, 2},                   // Unknown mystery bytes
      {0xC0, {0x26}, 1},                         // LCD control mode
      {0xC1, {0x11}, 1},                         // Set ID
      {0xC5, {0x35, 0x3E}, 2},                   // Set vcoms offset
      {0xC7, {0xBE}, 1},                         // set  CABC control
      {0x36, {0x00}, 1},                         // Memory Address Data Control
      {0x3A, {0x55}, 1},                         // column modifier
      //{0x21, {0}, 0},                            // invert screen
      {0xB1, {0x00, 0x1B}, 2},                   // RGB mode
      {0xF2, {0x08}, 1},                         // Unknown
      {0x26, {0x01}, 1},                         // Set Gamma
      {0xE0,
       {0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12,
        0x14, 0x17},
       14}, // More gamma control
      {0xE1,
       {0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17,
        0x1B, 0x1E},
       14},                                // More gamma control
      {0x2A, {0x00, 0x00, 0x00, 0xEF}, 4}, // column set
      {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4}, // row set
      {0x2C, {0}, 0},                      // idk
      {0xB7, {0x07}, 1},                   // gate control
      {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4}, // idk
      {0x11, {0}, 0x80},                   // idk
      {0x29, {0}, 0x80},                   // yeet on
      {0, {0}, 0xff},
  };

  u16 cmd = 0;
  while (st7789_init_cmds[cmd].databytes != 0xff) {
    St7789_WriteCommand(st7789_init_cmds[cmd].cmd);
    St7789_WriteData(st7789_init_cmds[cmd].data,
                     st7789_init_cmds[cmd].databytes & 0x1F);
    if (st7789_init_cmds[cmd].databytes & 0x80) {
      nrf_delay_ms(100);
    }
    cmd++;
  }
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

   assert(area->x1 >= 0); 
   assert(area->x1 < 240);
    assert(area->x2 >= 0);
     assert(area->x2 < 240);
    assert(area->y1 >= 0);
    assert(area->y1 < 240);
    assert(area->y2 >= 0);
    assert(area->y2 < 240);
    assert(area->y2 >= area->y1);
    assert(area->x2 >= area->x1);

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