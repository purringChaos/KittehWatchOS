#include "lvgl/lvgl.h"
#include "platform/includes/backlight.h"

#include <FreeRTOS.h>

#include <drivers/SpiMaster.h>
#include <drivers/St7789.h>
#include <hal/nrf_wdt.h>
#include <legacy/nrf_drv_clock.h>
#include <legacy/nrf_drv_gpiote.h>
#include <libraries/gpiote/app_gpiote.h>
#include <libraries/log/nrf_log.h>
#include <softdevice/common/nrf_sdh.h>
#include <task.h>

#include <hal/nrf_rtc.h>
#include <legacy/nrf_drv_clock.h>
#include <softdevice/common/nrf_sdh.h>
#include <drivers/St7789.h>


SpiMaster spi;
St7789 lcd;


void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void) {
  if (((NRF_SPIM0->INTENSET & (1 << 6)) != 0) && NRF_SPIM0->EVENTS_END == 1) {
    NRF_SPIM0->EVENTS_END = 0;
    SpiMaster_OnEndEvent(&spi);
  }

  if (((NRF_SPIM0->INTENSET & (1 << 19)) != 0) &&
      NRF_SPIM0->EVENTS_STARTED == 1) {
    NRF_SPIM0->EVENTS_STARTED = 0;
    SpiMaster_OnStartedEvent(&spi);
  }

  if (((NRF_SPIM0->INTENSET & (1 << 1)) != 0) &&
      NRF_SPIM0->EVENTS_STOPPED == 1) {
    NRF_SPIM0->EVENTS_STOPPED = 0;
  }
}


static lv_disp_buf_t disp_buf;

void my_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                            lv_color_t *color_p) {

  int32_t x, y;
  for (y = area->y1; y <= area->y2; y++) {
    for (x = area->x1; x <= area->x2; x++) {
      St7789_DrawPixel(&lcd, x + 1, y + 1, lv_color_to32(*color_p));
      color_p++;
    }
  }

  lv_disp_flush_ready(disp_drv);
}

void platform_initDisplay() {
  APP_GPIOTE_INIT(2);

  spi.spiBaseAddress = NULL;
  spi.pinCsn = 0;
  spi.spi = SpiMaster_SPI0;
  spi.bitOrder = SpiMaster_Msb_Lsb;
  spi.mode = SpiMaster_Mode3;
  spi.pinSCK = 2;
  spi.pinMOSI = 3;
  spi.pinMISO = 4;
  spi.currentBufferAddr = 0;
  spi.currentBufferSize = 0;
  spi.taskToNotify = NULL;
  spi.mutex = NULL;

  SpiMaster_Init(&spi);


  lcd.spiMaster = &spi;
  lcd.pinCsm = 25;
  lcd.pinDataCommand = 18;
  lcd.verticalScrollingStartAddress = 0;
  St7789_Init(&lcd);

  static lv_color_t buffer[240 * 4];

  lv_disp_buf_init(&disp_buf, buffer, NULL, 240 * 4);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.buffer = &disp_buf;

  disp_drv.flush_cb = my_flush_cb;

  /*indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = mouse_read;*/

  lv_disp_drv_register(&disp_drv);
  // lv_indev_drv_register(&indev_drv);
}