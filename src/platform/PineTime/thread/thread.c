#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <stdbool.h>
#include <unistd.h>


void platform_initThreading() {}
void platform_sleep(int t) { vTaskDelay(500 / portTICK_PERIOD_MS); }

void platform_setThreadName(const char *name) {
}

void platform_createThread(TaskHandle_t *thread, int priority,
                                  const char *const name, void (*func)(void *),
                                  void *arg){
xTaskCreate(func, name, configMINIMAL_STACK_SIZE + 200, arg, priority, &thread);
                                  }

void platform_startAllThreads() {
    vTaskStartScheduler();

    while (true)
    {
        /* FreeRTOS should not be here... FreeRTOS goes back to the start of stack
         * in vTaskStartScheduler function. */
    }
}