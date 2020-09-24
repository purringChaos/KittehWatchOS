#include "FreeRTOS.h"
#include "task.h"
#define portFIRST_USER_INTERRUPT_NUMBER (16)
#define portMAX_8_BIT_VALUE ((uint8_t)0xff)
#define portTOP_BIT_OF_BYTE ((uint8_t)0x80)
#define portINITIAL_XPSR (((xPSR_Type){.b.T = 1}).w)
#define portINITIAL_EXEC_RETURN (0xfffffffd)
#define portTASK_RETURN_ADDRESS prvTaskExitError
static UBaseType_t uxCriticalNesting = 0;
extern void vPortSetupTimerInterrupt(void);
void xPortSysTickHandler(void);
extern void vPortStartFirstTask(void);
static void vPortEnableVFP(void);
static void prvTaskExitError(void);
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack,
                                   TaskFunction_t pxCode, void *pvParameters) {
  pxTopOfStack--;
  *pxTopOfStack = portINITIAL_XPSR;
  pxTopOfStack--;
  *pxTopOfStack = (StackType_t)pxCode;
  pxTopOfStack--;
  *pxTopOfStack = (StackType_t)portTASK_RETURN_ADDRESS;
  pxTopOfStack -= 5;
  *pxTopOfStack = (StackType_t)pvParameters;
  pxTopOfStack--;
  *pxTopOfStack = portINITIAL_EXEC_RETURN;
  pxTopOfStack -= 8;
  return pxTopOfStack;
}

static void prvTaskExitError(void) {
  configASSERT(uxCriticalNesting == ~0UL);
  portDISABLE_INTERRUPTS();
  for (;;)
    ;
}

BaseType_t xPortStartScheduler(void) {
  NVIC_SetPriority(PendSV_IRQn, configKERNEL_INTERRUPT_PRIORITY);
  vPortSetupTimerInterrupt();
  uxCriticalNesting = 0;
  vPortEnableVFP();
  FPU->FPCCR |= FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk;
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk;
  vPortStartFirstTask();
  prvTaskExitError();
  return 0;
}

void vPortEndScheduler(void) { configASSERT(uxCriticalNesting == 1000UL); }

void vPortEnterCritical(void) {
  portDISABLE_INTERRUPTS();
  uxCriticalNesting++;
  if (uxCriticalNesting == 1) {
    configASSERT((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) == 0);
  }
}

void vPortExitCritical(void) {
  configASSERT(uxCriticalNesting);
  uxCriticalNesting--;
  if (uxCriticalNesting == 0) {
    portENABLE_INTERRUPTS();
  }
}

static void vPortEnableVFP(void) { SCB->CPACR |= 0xf << 20; }