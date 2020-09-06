#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lvgl/lvgl.h"

typedef struct {
	lv_style_t one;
	lv_style_t two;
} my_styles_t;

static void *tick_thread(void *arg)
{
	const struct timespec period = {
		.tv_sec = 0,
		.tv_nsec = 5000000UL,
	};

	struct timespec remaining;

	while (1) {
		nanosleep(&period, &remaining);
		lv_tick_inc(5);
	}

	return arg;
}

void vApplicationIdleHook()
{
}

void dev_init(void)
{
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

static lv_obj_t *label;

static void slider_event_cb(lv_obj_t *slider, lv_event_t event)
{
	if (event == LV_EVENT_VALUE_CHANGED) {
		/*Refresh the text*/
		lv_label_set_text_fmt(label, "%d", lv_slider_get_value(slider));
	}
}

void ui_init(void)
{
	lv_obj_t *slider = lv_slider_create(lv_scr_act(), NULL);
	lv_obj_set_width(slider, 200); /*Set the width*/
	lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0,
		     0); /*Align to the center of the parent (screen)*/
	lv_obj_set_event_cb(slider,
			    slider_event_cb); /*Assign an event function*/

	/* Create a label below the slider */
	label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label, "0");
	lv_obj_set_auto_realign(
		slider,
		true); /*To keep center alignment when the width of the text changes*/
	lv_obj_align(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0,
		     20); /*Align below the slider*/
}

int main(int argc, char *argv[])
{
	lv_init();
	dev_init();
	ui_init();

	pthread_t pid;
	pthread_create(&pid, NULL, tick_thread, NULL);

	const struct timespec period = {
		.tv_sec = 0,
		.tv_nsec = 5000000UL,
	};

	struct timespec remaining;

	while (1) {
		lv_task_handler();
		nanosleep(&period, &remaining); // trigger every 5ms.
	}

	return 0;
}
