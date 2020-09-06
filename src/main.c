#include <stdint.h>
#include "lvgl/lvgl.h"
#include <time.h>

#include "FreeRTOS.h"
#include "mpu_wrappers.h"
#include "task.h"
#include "timers.h"
#ifndef __linux__
#include "app_error.h"
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "sdk_errors.h"
#include <nrf_log_backend_serial.h>
#include <nrf_log_ctrl_internal.h>
#include <nrfx_log.h>
#endif
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include "platform/includes/backlight.h"
#include "platform/includes/display.h"


void vApplicationIdleHook(void)
{
lv_task_handler();

}



#define TASK_DELAY 1000 

TaskHandle_t
	led_toggle_task_handle; 
TimerHandle_t
	led_toggle_timer_handle; 

static lv_obj_t * label;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        /*Refresh the text*/
        lv_label_set_text_fmt(label, "%d", lv_slider_get_value(slider));
    }
}

bool toggleLED(bool x) {
	if (x) {
		platform_setBacklight(0);
	} else {
		platform_setBacklight(3);
	}
	return !x;
}

static void led_toggle_task_function(void *pvParameter)
{
	bool x = true;
	while (true) {
		x = toggleLED(x);
    lv_tick_inc(TASK_DELAY);
		vTaskDelay(TASK_DELAY);
	}
}

int main(void)
{
#ifndef __linux__
	ret_code_t err_code;
	/* Initialize clock driver for better time accuracy in FREERTOS */
	err_code = nrf_drv_clock_init();
	APP_ERROR_CHECK(err_code);
#endif

	platform_initBacklight();
  platform_initDisplay();

    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_width(slider, 200);                        /*Set the width*/
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);    /*Align to the center of the parent (screen)*/
    lv_obj_set_event_cb(slider, slider_event_cb);         /*Assign an event function*/

    /* Create a label below the slider */
    label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, "0");
    lv_obj_set_auto_realign(slider, true);                          /*To keep center alignment when the width of the text changes*/
    lv_obj_align(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);    /*Align below the slider*/

	/* Create task for LED0 blinking with priority set to 2 */
	xTaskCreate(led_toggle_task_function, "LED0",
		    configMINIMAL_STACK_SIZE + 200, NULL, 1,
		    &led_toggle_task_handle);

#ifndef __linux__
	/* Activate deep sleep mode */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#endif

	/* Start FreeRTOS scheduler. */
	vTaskStartScheduler();



	while (true) {
	}
}
