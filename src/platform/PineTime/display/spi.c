#include "display/spi.h"
#include "types/numbers.h"
#include <FreeRTOS.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_spim.h>
#include <semphr.h>
#include <task.h>

volatile u8 currentChipSelectPin = 0;
volatile u32 currentDataAddress = 0;
volatile size_t currentDataSize = 0;
SemaphoreHandle_t mutex = NULL;

// Docs and References

// START REF1
// Chip select must be held low while driving the display.
// It must be high when using other SPI devices on the same bus so that the
// display controller won't respond to the wrong commands.
// - Pinetime Wiki
// END REF1

#define WAIT_TASK_END                                                          \
  while (NRF_SPIM0->EVENTS_END == 0)  { platform_setBacklight(0; nrf_delay_ms(200);  platform_setBacklight(3); nrf_delay_ms(200)  }
#define START_TASKS NRF_SPIM0->TASKS_START = 1;
#define MINIMUM(x, y) ((y > x) ? x : y)

void enable_workaround_for_ftpan_58() {
  // Create an event when SCK toggles.
  NRF_GPIOTE->CONFIG[0] =
      (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
      (NRF_SPIM0->PSEL.SCK << GPIOTE_CONFIG_PSEL_Pos) |
      (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);

  // Stop the spim instance when SCK toggles.
  NRF_PPI->CH[0].EEP = (u32)&NRF_GPIOTE->EVENTS_IN[0];
  NRF_PPI->CH[0].TEP = (u32)&NRF_SPIM0->TASKS_STOP;
  NRF_PPI->CHENSET = 1U << 0;
  NRF_SPIM0->EVENTS_END = 0;
  NRF_SPIM0->INTENCLR = (1 << 6);
  NRF_SPIM0->INTENCLR = (1 << 1);
  NRF_SPIM0->INTENCLR = (1 << 19);
}

void disable_workaround_for_ftpan_58() {
  NRF_GPIOTE->CONFIG[0] = 0;
  NRF_PPI->CH[0].EEP = 0;
  NRF_PPI->CH[0].TEP = 0;
  NRF_PPI->CHENSET = 0;
  NRF_SPIM0->EVENTS_END = 0;
  NRF_SPIM0->INTENSET = (1 << 6);
  NRF_SPIM0->INTENSET = (1 << 1);
  NRF_SPIM0->INTENSET = (1 << 19);
}

void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void) {
  if (((NRF_SPIM0->INTENSET & 6) != 0) && NRF_SPIM0->EVENTS_END == 1) {
    NRF_SPIM0->EVENTS_END = 0;
    FinishedTransferCallback();
  }

  if (((NRF_SPIM0->INTENSET & 1) != 0) && NRF_SPIM0->EVENTS_STARTED == 1) {
    NRF_SPIM0->EVENTS_STARTED = 0;
  }

  if (((NRF_SPIM0->INTENSET & 19) != 0) && NRF_SPIM0->EVENTS_STOPPED == 1) {
    NRF_SPIM0->EVENTS_STOPPED = 0;
  }
}

bool SPI_Init() {
  currentDataAddress = 0;
  currentDataSize = 0;
  mutex = xSemaphoreCreateBinary();

  // Configure Pins
  // Pin documentation taken from
  // https://wiki.pine64.org/index.php/PineTime#Display Naming Convention is at
  // https://www.oshwa.org/a-resolution-to-redefine-spi-signal-names

  // Serial Clock is on GPIO 2
  nrf_gpio_pin_set(2);
  nrf_gpio_cfg_output(2);
  NRF_SPIM0->PSELSCK = 2;
  // Serial Data Out is on GPIO 3
  nrf_gpio_pin_clear(3);
  nrf_gpio_cfg_output(3);
  NRF_SPIM0->PSELMOSI = 3;
  // Serial Data In is on GPIO 4
  // Disable Sense level for input.
  nrf_gpio_cfg_input(4, NRF_GPIO_PIN_NOPULL);
  NRF_SPIM0->PSELMISO = 4;

  // 8MHz should be enough to drive the display fast.
  NRF_SPIM0->FREQUENCY = 0x80000000;

  // SPI must be used in mode 3. Mode 0 (the default) won't work.
  // - PineTime Wiki
  nrf_spim_configure(NRF_SPIM0, NRF_SPIM_MODE_3, NRF_SPIM_BIT_ORDER_MSB_FIRST);

  // Set events counter to 0.
  NRF_SPIM0->EVENTS_END = 0;

  // We need IRQs to fire so we know when events stop, start and end.
  NRF_SPIM0->INTENSET = (1 << 6);
  NRF_SPIM0->INTENSET = (1 << 1);
  NRF_SPIM0->INTENSET = (1 << 19);
  // We need IRQs to fire so we know when events stop, start and end.
  NRFX_IRQ_PRIORITY_SET(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, 2);
  NRFX_IRQ_ENABLE(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);

  // Finally we can do a UwU and enable SPI!! Wheeeeeee~ *gir noises*
  nrf_spim_enable(NRF_SPIM0);

  // oh, now that its started we can also free mutex so floofs can use stuff uwu
  xSemaphoreGive(mutex);
  return true;
}

void FinishedTransferCallback() {
  if (currentDataAddress == 0) {
    // This bitch empty. Yeet!
    return;
  }
  // If we have data in our buffer, we can send it!
  if (currentDataSize > 0) {
    // We can only transfer 255 bytes at a time so,
    u32 currentSize = MINIMUM(255, currentDataSize);
    nrf_spim_tx_buffer_set(NRF_SPIM0, (u8 const *)currentDataAddress,
                           currentSize);
    // We transfer up to 255 bytes then increase pointer.
    currentDataAddress += currentSize;
    currentDataSize -= currentSize;

    START_TASKS
  } else {
    // Now that we don't have any data left in the buffer
    // We can set the SPI pin back to high waiting for events.
    // See REF1
    nrf_gpio_pin_set(currentChipSelectPin);
    currentDataAddress = 0;
    // If we were woken up from a higher priority thread, make sure to switch
    // context back to it quickly. Otherise scheduling can be wack.
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(mutex, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

bool SPI_Write(u8 newChipSelectPin, const u8 *data, size_t size) {
  if (data == NULL) {
    // If you aint got data, why u even here
    return false;
  }
  
  xSemaphoreTake(mutex, portMAX_DELAY);

  currentChipSelectPin = newChipSelectPin;

  if (size == 1) {
    enable_workaround_for_ftpan_58();
  } else {
    disable_workaround_for_ftpan_58();
  }

  nrf_gpio_pin_clear(currentChipSelectPin);

  currentDataAddress = (u32)data;
  currentDataSize = size;
  u32 currentSize = MINIMUM(255, currentDataSize);
  NRF_SPIM0->TXD.PTR = currentDataAddress;
  NRF_SPIM0->TXD.MAXCNT = currentSize;
  NRF_SPIM0->TXD.LIST = 0;
  NRF_SPIM0->RXD.PTR = 0;
  NRF_SPIM0->RXD.MAXCNT = 0;
  NRF_SPIM0->RXD.LIST = 0;
  NRF_SPIM0->EVENTS_END = 0;
  currentDataSize -= currentSize;
  currentDataAddress += currentSize;
  START_TASKS

  if (size == 1) {
    WAIT_TASK_END
    nrf_gpio_pin_set(currentChipSelectPin);
    currentDataAddress = 0;
    xSemaphoreGive(mutex);
  }

  return true;
}
