/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software. If you wish to use our
 * Amazon FreeRTOS name, please do so in a fair use way that does not cause
 * confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "app_util.h"
#include "nrf_drv_clock.h"
#include "nrf_rtc.h"
#include "task.h"

/*-----------------------------------------------------------*/

void xPortSysTickHandler(void) {
  nrf_rtc_event_clear(portNRF_RTC_REG, NRF_RTC_EVENT_COMPARE_0);

  BaseType_t switch_req = pdFALSE;
  uint32_t isrstate = portSET_INTERRUPT_MASK_FROM_ISR();

  uint32_t systick_counter = nrf_rtc_counter_get(portNRF_RTC_REG);
  nrf_rtc_event_clear(portNRF_RTC_REG, NRF_RTC_EVENT_TICK);

  if (configUSE_DISABLE_TICK_AUTO_CORRECTION_DEBUG == 0) {
    TickType_t diff;
    diff = (systick_counter - xTaskGetTickCount()) & portNRF_RTC_MAXTICKS;
    if ((diff > 1) && (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING)) {
      diff = 1;
    }
    while ((diff--) > 0) {
      switch_req |= xTaskIncrementTick();
    }
  } else {
    switch_req = xTaskIncrementTick();
  }
  if (switch_req != pdFALSE) {
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __SEV();
  }

  portCLEAR_INTERRUPT_MASK_FROM_ISR(isrstate);
}

void vPortSetupTimerInterrupt(void) {
  nrf_drv_clock_lfclk_request(NULL);
  nrf_rtc_prescaler_set(portNRF_RTC_REG, portNRF_RTC_PRESCALER);
  nrf_rtc_int_enable(portNRF_RTC_REG, RTC_INTENSET_TICK_Msk);
  nrf_rtc_task_trigger(portNRF_RTC_REG, NRF_RTC_TASK_CLEAR);
  nrf_rtc_task_trigger(portNRF_RTC_REG, NRF_RTC_TASK_START);
  nrf_rtc_event_enable(portNRF_RTC_REG, RTC_EVTEN_OVRFLW_Msk);
  NVIC_SetPriority(portNRF_RTC_IRQn, configKERNEL_INTERRUPT_PRIORITY);
  NVIC_EnableIRQ(portNRF_RTC_IRQn);
}

void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime) {
  TickType_t enterTime;
  if (xExpectedIdleTime >
      portNRF_RTC_MAXTICKS - configEXPECTED_IDLE_TIME_BEFORE_SLEEP) {
    xExpectedIdleTime =
        portNRF_RTC_MAXTICKS - configEXPECTED_IDLE_TIME_BEFORE_SLEEP;
  }

  __disable_irq();

  enterTime = nrf_rtc_counter_get(portNRF_RTC_REG);

  if (eTaskConfirmSleepModeStatus() != eAbortSleep) {
    TickType_t xModifiableIdleTime;
    TickType_t wakeupTime =
        (enterTime + xExpectedIdleTime) & portNRF_RTC_MAXTICKS;

    nrf_rtc_int_disable(portNRF_RTC_REG, NRF_RTC_INT_TICK_MASK);

    nrf_rtc_cc_set(portNRF_RTC_REG, 0, wakeupTime);
    nrf_rtc_event_clear(portNRF_RTC_REG, NRF_RTC_EVENT_COMPARE_0);
    nrf_rtc_int_enable(portNRF_RTC_REG, NRF_RTC_INT_COMPARE0_MASK);
    __DSB();
    xModifiableIdleTime = xExpectedIdleTime;
    configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
    if (xModifiableIdleTime > 0) {
      {
        __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
        (void)__get_FPSCR();
        NVIC_ClearPendingIRQ(FPU_IRQn);
        do {
          __WFE();
        } while (0 == (NVIC->ISPR[0] | NVIC->ISPR[1]));
      }
    }
    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

    nrf_rtc_int_disable(portNRF_RTC_REG, NRF_RTC_INT_COMPARE0_MASK);
    nrf_rtc_event_clear(portNRF_RTC_REG, NRF_RTC_EVENT_COMPARE_0);

    {
      TickType_t diff;
      TickType_t exitTime;

      nrf_rtc_event_clear(portNRF_RTC_REG, NRF_RTC_EVENT_TICK);
      nrf_rtc_int_enable(portNRF_RTC_REG, NRF_RTC_INT_TICK_MASK);

      exitTime = nrf_rtc_counter_get(portNRF_RTC_REG);
      diff = (exitTime - enterTime) & portNRF_RTC_MAXTICKS;

      NVIC_ClearPendingIRQ(portNRF_RTC_IRQn);

      if ((configUSE_TICKLESS_IDLE_SIMPLE_DEBUG) &&
          (diff > xExpectedIdleTime)) {
        diff = xExpectedIdleTime;
      }

      if (diff > 0) {
        vTaskStepTick(diff);
      }
    }
  }
  __enable_irq();
}