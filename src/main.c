#include "lvgl/lvgl.h"
#include <stdint.h>
#include <time.h>

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

#include "displaymanager/displaymanager.h"
#include "platform/includes/backlight.h"
#include "platform/includes/display.h"
#include "platform/includes/thread.h"

THREAD_TYPE lv_tick_task_handle;
THREAD_TYPE lv_handle_task_handle;
THREAD_TYPE led_toggle_task_handle;

void vApplicationIdleHook(void) { lv_tick_inc(1); }

static void lv_handle_task_function(void *pvParameter) {
  platform_setThreadName("lv_handle\0");
  while (true) {
    platform_sleep(5);
    lv_task_handler();
  }
}

static void lv_tick_task_function(void *pvParameter) {
  platform_setThreadName("lv_tick\0");
  while (true) {
    lv_tick_inc(5);
    platform_sleep(5);
  }
}

int main(void) {
#ifndef __linux__
  ret_code_t err_code;
  /* Initialize clock driver for better time accuracy in FREERTOS */
  err_code = nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);
#endif

  platform_initThreading();
  platform_initBacklight();
  platform_initDisplay();

  displaymanager_start();


#ifdef __linux__
  platform_createThread(&lv_tick_task_handle, 1, "lv_tick",
                        lv_tick_task_function, NULL);
#else
  platform_createThread(&lv_handle_task_handle, 2, "lv_handle",
                        lv_handle_task_function, NULL);
#endif

#ifndef __linux__
  /* Activate deep sleep mode */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#endif

#ifdef __linux__
  const struct timespec period = {
      .tv_sec = 0,
      .tv_nsec = 5000000UL,
  };

  struct timespec remaining;

  while (1) {
    lv_task_handler();
    nanosleep(&period, &remaining); // trigger every 5ms.
  }
#else
  platform_startAllThreads();
#endif
}
