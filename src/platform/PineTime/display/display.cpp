#include "lvgl/lvgl.h"
#include "platform/includes/backlight.h"

#include <FreeRTOS.h>
#include <cstdint>
#include <cstring>

#include <drivers/Spi.h>
#include <drivers/SpiMaster.h>
#include <drivers/SpiNorFlash.h>
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

#include <Components/Gfx/Gfx.h>
#include <drivers/St7789.h>

static constexpr uint8_t pinSpiSck = 2;
static constexpr uint8_t pinSpiMosi = 3;
static constexpr uint8_t pinSpiMiso = 4;
static constexpr uint8_t pinSpiFlashCsn = 5;
static constexpr uint8_t pinLcdCsn = 25;
static constexpr uint8_t pinLcdDataCommand = 18;

Pinetime::Drivers::SpiMaster spi{
    Pinetime::Drivers::SpiMaster::SpiModule::SPI0,
    {Pinetime::Drivers::SpiMaster::BitOrder::Msb_Lsb,
     Pinetime::Drivers::SpiMaster::Modes::Mode3,
     Pinetime::Drivers::SpiMaster::Frequencies::Freq8Mhz, pinSpiSck, pinSpiMosi,
     pinSpiMiso}};
Pinetime::Drivers::Spi flashSpi{spi, pinSpiFlashCsn};

Pinetime::Drivers::Spi lcdSpi{spi, pinLcdCsn};
Pinetime::Drivers::St7789 lcd{lcdSpi, pinLcdDataCommand};

Pinetime::Components::Gfx gfx{lcd};

extern "C" {
void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void) {
  if (((NRF_SPIM0->INTENSET & (1 << 6)) != 0) && NRF_SPIM0->EVENTS_END == 1) {
    NRF_SPIM0->EVENTS_END = 0;
    spi.OnEndEvent();
  }

  if (((NRF_SPIM0->INTENSET & (1 << 19)) != 0) &&
      NRF_SPIM0->EVENTS_STARTED == 1) {
    NRF_SPIM0->EVENTS_STARTED = 0;
    spi.OnStartedEvent();
  }

  if (((NRF_SPIM0->INTENSET & (1 << 1)) != 0) &&
      NRF_SPIM0->EVENTS_STOPPED == 1) {
    NRF_SPIM0->EVENTS_STOPPED = 0;
  }
}
}

static lv_disp_buf_t disp_buf;

extern "C" void my_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                            lv_color_t *color_p) {

  int32_t x, y;
  for (y = area->y1; y <= area->y2; y++) {
    for (x = area->x1; x <= area->x2; x++) {
      lcd.DrawPixel(x + 1, y + 1, static_cast<uint8_t>(lv_color_to8(*color_p)));
      color_p++;
    }
  }

  lv_disp_flush_ready(disp_drv);
}

extern "C" void platform_initDisplay() {
  APP_GPIOTE_INIT(2);

  spi.Init();
  lcd.Init();
  gfx.Init();

  lv_init();
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
