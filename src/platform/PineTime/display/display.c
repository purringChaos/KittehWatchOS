#include "lvgl/lvgl.h"
  static lv_disp_buf_t disp_buf;


void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            put_px(x, y, *color_p)
            color_p++;
        }
    }*/

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

  static lv_color_t buffer[240 * 240];

void platform_initDisplay() {
  lv_init();
  static lv_disp_buf_t disp_buf;
  lv_disp_buf_init(&disp_buf, buffer, NULL, 120 * 64);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.buffer = &disp_buf;

  disp_drv.flush_cb = my_flush_cb;

  /*indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = mouse_read;*/

  //lv_disp_drv_register(&disp_drv);
  //lv_indev_drv_register(&indev_drv);
}
