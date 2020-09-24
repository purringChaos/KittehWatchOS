#ifndef PORTMACRO_CMSIS_H
#define PORTMACRO_CMSIS_H
#include "nrf.h"

#define portCHAR char
#define portFLOAT float
#define portDOUBLE double
#define portLONG long
#define portSHORT short
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

typedef uint32_t TickType_t;
#define portMAX_DELAY (TickType_t)0xffffffffUL
#define portTICK_TYPE_IS_ATOMIC 1

#define portSTACK_GROWTH (-1)
#define portTICK_PERIOD_MS ((TickType_t)1000 / configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT 8

#define portNRF_RTC_REG NRF_RTC1
#define portNRF_RTC_IRQn RTC1_IRQn
#define portNRF_RTC_PRESCALER                                                  \
  ((uint32_t)(ROUNDED_DIV(configSYSTICK_CLOCK_HZ, configTICK_RATE_HZ) - 1))
#define portNRF_RTC_MAXTICKS ((1U << 24) - 1U)

#define portYIELD()                                                            \
  do {                                                                         \
    /* Set a PendSV to request a context switch. */                            \
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;                                        \
    __SEV();                                                                   \
    /* Barriers are normally not required but do ensure the code is completely \
within the specified behaviour for the architecture. */                        \
    __DSB();                                                                   \
    __ISB();                                                                   \
  } while (0)

#define portEND_SWITCHING_ISR(xSwitchRequired)                                 \
  if ((xSwitchRequired) != pdFALSE)                                            \
  portYIELD()
#define portYIELD_FROM_ISR(x) portEND_SWITCHING_ISR(x)
/*-----------------------------------------------------------*/

/* Critical section management. */
extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);
#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortRaiseBASEPRI()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
#define portDISABLE_INTERRUPTS() __asm volatile(" cpsid i " ::: "memory")
#define portENABLE_INTERRUPTS() __asm volatile(" cpsie i " ::: "memory")
#define portENTER_CRITICAL() vPortEnterCritical()
#define portEXIT_CRITICAL() vPortExitCritical()

#define portTASK_FUNCTION_PROTO(vFunction, pvParameters)                       \
  void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters)                             \
  void vFunction(void *pvParameters)

#ifndef portSUPPRESS_TICKS_AND_SLEEP
extern void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime)                        \
  vPortSuppressTicksAndSleep(xExpectedIdleTime)
#endif

#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

/* Count leading zeros helper. */
#define ucPortCountLeadingZeros(bits) __CLZ(bits)

/* Store/clear the ready priorities in a bit map. */
#define portRECORD_READY_PRIORITY(uxPriority, uxReadyPriorities)               \
  (uxReadyPriorities) |= (1UL << (uxPriority))
#define portRESET_READY_PRIORITY(uxPriority, uxReadyPriorities)                \
  (uxReadyPriorities) &= ~(1UL << (uxPriority))

/*-----------------------------------------------------------*/

#define portGET_HIGHEST_PRIORITY(uxTopPriority, uxReadyPriorities)             \
  uxTopPriority = (31 - ucPortCountLeadingZeros((uxReadyPriorities)))

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/*-----------------------------------------------------------*/

#ifdef configASSERT
void vPortValidateInterruptPriority(void);
#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID()                             \
  vPortValidateInterruptPriority()
#endif

/*-----------------------------------------------------------*/

#define vPortSetBASEPRI(ulNewMaskValue) __set_BASEPRI(ulNewMaskValue)

/*-----------------------------------------------------------*/

#define vPortRaiseBASEPRI()                                                    \
  vPortSetBASEPRI(configMAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/*-----------------------------------------------------------*/

__STATIC_INLINE uint32_t ulPortRaiseBASEPRI(void) {
  uint32_t ulOriginalBASEPRI = __get_BASEPRI();
  vPortRaiseBASEPRI();
  return ulOriginalBASEPRI;
}

/*-----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_CMSIS_H */
