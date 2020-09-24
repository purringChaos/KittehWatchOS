#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lvgl/lvgl.h"

void flush_wrap(lv_disp_drv_t *driver, const lv_area_t *area,
                  lv_color_t *colour_data) {
                    printf("x1: %d, x2: %d, y1: %d, y2: %d\n", area->x1, area->x2, area->y1, area->y2);
                    monitor_flush(driver, area, colour_data);
}

void platform_initDisplay() {
  static lv_disp_buf_t disp_buf;
  static lv_color_t buffer[240 * 2];
  static lv_color_t buffer2[240 * 2];
  lv_disp_buf_init(&disp_buf, buffer, buffer2, 240 * 2);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.buffer = &disp_buf;
  disp_drv.flush_cb = flush_wrap;


  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);

  monitor_init();
  mouse_init();


  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = mouse_read;

  lv_disp_drv_register(&disp_drv);
  lv_indev_drv_register(&indev_drv);
}
