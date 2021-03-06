#include "displaymanager/displaymanager.h"
#include "lvgl/lvgl.h"
#include "platform/includes/backlight.h"
#include "platform/includes/display.h"
#include "platform/includes/misc.h"
#include "platform/includes/thread.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

void RADIO_IRQHandler(void) {}
void UARTE0_UART0_IRQHandler(void) {}
void NFCT_IRQHandler(void) {}
void TIMER0_IRQHandler(void) {}
void TIMER1_IRQHandler(void) {}
void TIMER2_IRQHandler(void) {}
void RTC0_IRQHandler(void) {}
void TEMP_IRQHandler(void) {}
void RNG_IRQHandler(void) {}
void ECB_IRQHandler(void) {}
void CCM_AAR_IRQHandler(void) {}
void QDEC_IRQHandler(void) {}
void COMP_LPCOMP_IRQHandler(void) {}
void SWI0_EGU0_IRQHandler(void) {}
void SWI1_EGU1_IRQHandler(void) {}
void SWI2_EGU2_IRQHandler(void) {}
void SWI3_EGU3_IRQHandler(void) {}
void SWI4_EGU4_IRQHandler(void) {}
void SWI5_EGU5_IRQHandler(void) {}
void TIMER3_IRQHandler(void) {}
void TIMER4_IRQHandler(void) {}
void PWM0_IRQHandler(void) {}
void PDM_IRQHandler(void) {}
void MWU_IRQHandler(void) {}
void PWM1_IRQHandler(void) {}
void PWM2_IRQHandler(void) {}
void SPIM2_SPIS2_SPI2_IRQHandler(void) {}
void RTC2_IRQHandler(void) {}
void I2S_IRQHandler(void) {}
void FPU_IRQHandler(void) {}
void vApplicationIdleHook(void) { lv_tick_inc(1); }

#ifndef __linux__
THREAD_TYPE lv_event_handler_thread_handle;
static void lv_event_handler_thread(void *pvParameter) {
  platform_setThreadName("lv_handle\0");
  while (true) {
    platform_sleep(5);
    lv_task_handler();
  }
}
#endif

THREAD_TYPE display_manager_thread_handle;

static void displayManagerWrapper(void *pvParameter) { displaymanager_start(); }

int main(void) {
  platform_initMisc();
  platform_initThreading();
  platform_initBacklight();
  lv_init();

  platform_createThread(&display_manager_thread_handle, 1, "disp_man",
                        displayManagerWrapper, NULL);

#ifndef __linux__
  platform_createThread(&lv_event_handler_thread_handle, 2, "lv_handle",
                        lv_event_handler_thread, NULL);
#endif
  platform_startAllThreads();
}
