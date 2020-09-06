#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lvgl/lvgl.h"

void platform_initDisplay()
{
	lv_init();
	static lv_disp_buf_t disp_buf;
	static lv_color_t buffer[240 * 240];
	lv_disp_buf_init(&disp_buf, buffer, NULL, 240 * 64);

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.buffer = &disp_buf;

	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);

	monitor_init();
	mouse_init();

	disp_drv.flush_cb = monitor_flush;

	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = mouse_read;

	lv_disp_drv_register(&disp_drv);
	lv_indev_drv_register(&indev_drv);
}
