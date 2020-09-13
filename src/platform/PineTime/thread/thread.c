#include "FreeRTOS.h"
#include "FreeRTOS/FreeRTOSConfig.h"
#include "portmacro_cmsis.h"
#include "task.h"
#include "timers.h"
#include <stdbool.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "FreeRTOS/FreeRTOSConfig.h"
#include "app_error.h"
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include <hal/nrf_wdt.h>

void WDT_IRQHandler(void) { nrf_wdt_event_clear(NRF_WDT_EVENT_TIMEOUT); }

void platform_initThreading() {
  nrf_drv_clock_init();
}
void platform_sleep(int t) { vTaskDelay(t); }

void platform_setThreadName(const char *name) {}

void platform_createThread(TaskHandle_t *thread, int priority,
                           const char *const name, void (*func)(void *),
                           void *arg) {
  xTaskCreate(func, name, configMINIMAL_STACK_SIZE + 200, arg, priority,
              thread);
}

void platform_startAllThreads() {
  vTaskStartScheduler();

  while (true) {
    /* FreeRTOS should not be here... FreeRTOS goes back to the start of stack
     * in vTaskStartScheduler function. */
  }
}
