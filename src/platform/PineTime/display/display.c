#include "lvgl/lvgl.h"
#include "platform/includes/backlight.h"

#include "display/St7789.h"
#include "display/i2c.h"
#include "display/spi.h"
#include <FreeRTOS.h>
#include <hal/nrf_rtc.h>
#include <legacy/nrf_drv_clock.h>
#include <libraries/delay/nrf_delay.h>
#include <softdevice/common/nrf_sdh.h>
#include <task.h>

static lv_disp_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
/*
bool touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  u8 touchData[63];

  I2C_Read(0x15, 0, touchData, 63);

  u16 x = (touchData[9] & 0x0f << 8) | touchData[10];
  u16 y = (touchData[11] & 0x0f << 8) | touchData[12];

  u8 action = touchData[9] >> 6;

  if (action == 0) {
    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PR;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
  // this is not a buffered driver so
  // there is nothing else buffered to read
  return false;
}
*/
void platform_initDisplay() {
  // I2C_Init();
  platform_setBacklight(3);

  SPI_Init();
  St7789_Init();
 static lv_color_t buffer[240 * 2];



  //static lv_color_t buffer2[240 * 2];


#define BUF_W 50
#define BUF_H 50

volatile bool h = true;
lv_color_t buf[BUF_W * BUF_H];
lv_color_t * buf_p = buf;
uint16_t x, y;
for(y = 0; y < BUF_H; y++) {
    for(x = 0; x < BUF_W; x++){
        (*buf_p) = LV_COLOR_RED;
        buf_p++;
    }
}

lv_area_t a;
a.x1 = 50;
a.y1 = 50;
a.x2 = a.x1 + BUF_W - 1;
a.y2 = a.y1 + BUF_H - 1;
St7789_Flush(NULL, &a, buf);



/*

  lv_disp_buf_init(&disp_buf, buffer, buffer2, 240 * 2);
  lv_disp_drv_init(&disp_drv);
  disp_drv.buffer = &disp_buf;
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = St7789_Flush;
  lv_disp_drv_register(&disp_drv);




*/



  /*  nrf_gpio_cfg_output(10);
    nrf_gpio_pin_set(10);
    nrf_delay_ms(50);
    nrf_gpio_pin_clear(10);
    nrf_delay_ms(5);
    nrf_gpio_pin_set(10);
    nrf_delay_ms(50);

    // Wake the touchpanel up
    uint8_t dummy;
    I2C_Read(0x15, 0x15, &dummy, 1);
    vTaskDelay(5);
    I2C_Read(0x15, 0xa7, &dummy, 1);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);*/
}