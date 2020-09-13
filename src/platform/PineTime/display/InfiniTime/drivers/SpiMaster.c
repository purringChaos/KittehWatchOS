#include "SpiMaster.h"
#include <FreeRTOS.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_spim.h>
#include <task.h>

bool SpiMaster_Init(SpiMaster *self) {
  if (self->mutex == NULL) {
    self->currentBufferAddr = 0;
    self->currentBufferSize = 0;
    self->mutex = xSemaphoreCreateBinary();
  }
  nrf_gpio_pin_set(self->pinSCK);
  nrf_gpio_cfg_output(self->pinSCK);
  nrf_gpio_pin_clear(self->pinMOSI);
  nrf_gpio_cfg_output(self->pinMOSI);
  nrf_gpio_cfg_input(self->pinMISO, NRF_GPIO_PIN_NOPULL);
  //  nrf_gpio_cfg_output(pinCSN);
  //  pinCsn = pinCSN;

  switch (self->spi) {
  case SpiMaster_SPI0:
    self->spiBaseAddress = NRF_SPIM0;
    break;
  case SpiMaster_SPI1:
    self->spiBaseAddress = NRF_SPIM1;
    break;
  default:
    return false;
  }

  self->spiBaseAddress->PSELSCK = self->pinSCK;
  self->spiBaseAddress->PSELMOSI = self->pinMOSI;
  self->spiBaseAddress->PSELMISO = self->pinMISO;
  self->spiBaseAddress->FREQUENCY = 0x80000000;

  uint32_t regConfig = 0;
  switch (self->bitOrder) {
  case SpiMaster_Msb_Lsb:
    break;
  case SpiMaster_Lsb_Msb:
    regConfig = 1;
  default:
    return false;
  }
  switch (self->mode) {
  case SpiMaster_Mode0:
    break;
  case SpiMaster_Mode1:
    regConfig |= (0x01 << 1);
    break;
  case SpiMaster_Mode2:
    regConfig |= (0x02 << 1);
    break;
  case SpiMaster_Mode3:
    regConfig |= (0x03 << 1);
    break;
  default:
    return false;
  }

  self->spiBaseAddress->CONFIG = regConfig;
  self->spiBaseAddress->EVENTS_ENDRX = 0;
  self->spiBaseAddress->EVENTS_ENDTX = 0;
  self->spiBaseAddress->EVENTS_END = 0;

  self->spiBaseAddress->INTENSET = ((unsigned)1 << (unsigned)6);
  self->spiBaseAddress->INTENSET = ((unsigned)1 << (unsigned)1);
  self->spiBaseAddress->INTENSET = ((unsigned)1 << (unsigned)19);

  self->spiBaseAddress->ENABLE =
      (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);

  NRFX_IRQ_PRIORITY_SET(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, 2);
  NRFX_IRQ_ENABLE(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);

  xSemaphoreGive(self->mutex);
  return true;
}

void SpiMaster_SetupWorkaroundForFtpan58(SpiMaster *self, NRF_SPIM_Type *spim,
                                         uint32_t ppi_channel,
                                         uint32_t gpiote_channel) {
  // Create an event when SCK toggles.
  NRF_GPIOTE->CONFIG[gpiote_channel] =
      (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
      (spim->PSEL.SCK << GPIOTE_CONFIG_PSEL_Pos) |
      (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);

  // Stop the spim instance when SCK toggles.
  NRF_PPI->CH[ppi_channel].EEP =
      (uint32_t)&NRF_GPIOTE->EVENTS_IN[gpiote_channel];
  NRF_PPI->CH[ppi_channel].TEP = (uint32_t)&spim->TASKS_STOP;
  NRF_PPI->CHENSET = 1U << ppi_channel;
  self->spiBaseAddress->EVENTS_END = 0;

  // Disable IRQ
  spim->INTENCLR = (1 << 6);
  spim->INTENCLR = (1 << 1);
  spim->INTENCLR = (1 << 19);
}

void SpiMaster_DisableWorkaroundForFtpan58(SpiMaster *self, NRF_SPIM_Type *spim,
                                           uint32_t ppi_channel,
                                           uint32_t gpiote_channel) {
  NRF_GPIOTE->CONFIG[gpiote_channel] = 0;
  NRF_PPI->CH[ppi_channel].EEP = 0;
  NRF_PPI->CH[ppi_channel].TEP = 0;
  NRF_PPI->CHENSET = ppi_channel;
  self->spiBaseAddress->EVENTS_END = 0;
  spim->INTENSET = (1 << 6);
  spim->INTENSET = (1 << 1);
  spim->INTENSET = (1 << 19);
}

void SpiMaster_OnEndEvent(SpiMaster *self) {
  if (self->currentBufferAddr == 0) {
    return;
  }

  uint32_t s = self->currentBufferSize;
  if (s > 0) {
    uint32_t currentSize = (s < (size_t)255) ? (size_t)255 : s;
    SpiMaster_PrepareTx(self, self->currentBufferAddr, currentSize);
    self->currentBufferAddr += currentSize;
    self->currentBufferSize -= currentSize;

    self->spiBaseAddress->TASKS_START = 1;
  } else {
    if (self->taskToNotify != NULL) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      vTaskNotifyGiveFromISR(self->taskToNotify, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    nrf_gpio_pin_set(self->pinCsn);
    self->currentBufferAddr = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(self->mutex, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void SpiMaster_OnStartedEvent(SpiMaster *self) {}

void SpiMaster_PrepareTx(SpiMaster *self, const volatile uint32_t bufferAddress,
                         const volatile size_t size) {
  self->spiBaseAddress->TXD.PTR = bufferAddress;
  self->spiBaseAddress->TXD.MAXCNT = size;
  self->spiBaseAddress->TXD.LIST = 0;
  self->spiBaseAddress->RXD.PTR = 0;
  self->spiBaseAddress->RXD.MAXCNT = 0;
  self->spiBaseAddress->RXD.LIST = 0;
  self->spiBaseAddress->EVENTS_END = 0;
}

void SpiMaster_PrepareRx(SpiMaster *self, const volatile uint32_t cmdAddress,
                         const volatile size_t cmdSize,
                         const volatile uint32_t bufferAddress,
                         const volatile size_t size) {
  self->spiBaseAddress->TXD.PTR = 0;
  self->spiBaseAddress->TXD.MAXCNT = 0;
  self->spiBaseAddress->TXD.LIST = 0;
  self->spiBaseAddress->RXD.PTR = bufferAddress;
  self->spiBaseAddress->RXD.MAXCNT = size;
  self->spiBaseAddress->RXD.LIST = 0;
  self->spiBaseAddress->EVENTS_END = 0;
}

bool SpiMaster_Write(SpiMaster *self, uint8_t pinCsn, const uint8_t *data,
                     size_t size) {
  if (data == NULL)
    return false;
  bool ok = xSemaphoreTake(self->mutex, portMAX_DELAY);
  ASSERT(ok == true);
  self->taskToNotify = xTaskGetCurrentTaskHandle();

  self->pinCsn = pinCsn;

  if (size == 1) {
    SpiMaster_SetupWorkaroundForFtpan58(self, self->spiBaseAddress, 0, 0);
  } else {
    SpiMaster_DisableWorkaroundForFtpan58(self, self->spiBaseAddress, 0, 0);
  }

  nrf_gpio_pin_clear(self->pinCsn);

  self->currentBufferAddr = (uint32_t)data;
  self->currentBufferSize = size;

  uint32_t currentSize = ((size_t)self->currentBufferSize < (size_t)255)
                             ? (size_t)255
                             : (size_t)self->currentBufferSize;
  SpiMaster_PrepareTx(self, self->currentBufferAddr, currentSize);
  self->currentBufferSize -= currentSize;
  self->currentBufferAddr += currentSize;
  self->spiBaseAddress->TASKS_START = 1;

  if (size == 1) {
    while (self->spiBaseAddress->EVENTS_END == 0)
      ;
    nrf_gpio_pin_set(self->pinCsn);
    self->currentBufferAddr = 0;
    xSemaphoreGive(self->mutex);
  }

  return true;
}

bool SpiMaster_Read(SpiMaster *self, uint8_t pinCsn, uint8_t *cmd,
                    size_t cmdSize, uint8_t *data, size_t dataSize) {
  xSemaphoreTake(self->mutex, portMAX_DELAY);

  self->taskToNotify = NULL;

  self->pinCsn = pinCsn;
  SpiMaster_DisableWorkaroundForFtpan58(self, self->spiBaseAddress, 0, 0);
  self->spiBaseAddress->INTENCLR = (1 << 6);
  self->spiBaseAddress->INTENCLR = (1 << 1);
  self->spiBaseAddress->INTENCLR = (1 << 19);

  nrf_gpio_pin_clear(self->pinCsn);

  self->currentBufferAddr = 0;
  self->currentBufferSize = 0;

  SpiMaster_PrepareTx(self, (uint32_t)cmd, cmdSize);
  self->spiBaseAddress->TASKS_START = 1;
  while (self->spiBaseAddress->EVENTS_END == 0)
    ;

  SpiMaster_PrepareRx(self, (uint32_t)cmd, cmdSize, (uint32_t)data, dataSize);
  self->spiBaseAddress->TASKS_START = 1;

  while (self->spiBaseAddress->EVENTS_END == 0)
    ;
  nrf_gpio_pin_set(self->pinCsn);

  xSemaphoreGive(self->mutex);

  return true;
}

void SpiMaster_Sleep(SpiMaster *self) {
  while (self->spiBaseAddress->ENABLE != 0) {
    self->spiBaseAddress->ENABLE =
        (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
  }
  nrf_gpio_cfg_default(self->pinSCK);
  nrf_gpio_cfg_default(self->pinMOSI);
  nrf_gpio_cfg_default(self->pinMISO);
}

void SpiMaster_Wakeup(SpiMaster *self) { SpiMaster_Init(self); }

bool SpiMaster_WriteCmdAndBuffer(SpiMaster *self, uint8_t pinCsn,
                                 const uint8_t *cmd, size_t cmdSize,
                                 const uint8_t *data, size_t dataSize) {
  xSemaphoreTake(self->mutex, portMAX_DELAY);

  self->taskToNotify = NULL;

  self->pinCsn = pinCsn;
  SpiMaster_DisableWorkaroundForFtpan58(self, self->spiBaseAddress, 0, 0);
  self->spiBaseAddress->INTENCLR = (1 << 6);
  self->spiBaseAddress->INTENCLR = (1 << 1);
  self->spiBaseAddress->INTENCLR = (1 << 19);

  nrf_gpio_pin_clear(self->pinCsn);

  self->currentBufferAddr = 0;
  self->currentBufferSize = 0;

  SpiMaster_PrepareTx(self, (uint32_t)cmd, cmdSize);
  self->spiBaseAddress->TASKS_START = 1;
  while (self->spiBaseAddress->EVENTS_END == 0)
    ;

  SpiMaster_PrepareTx(self, (uint32_t)data, dataSize);
  self->spiBaseAddress->TASKS_START = 1;

  while (self->spiBaseAddress->EVENTS_END == 0)
    ;
  nrf_gpio_pin_set(self->pinCsn);

  xSemaphoreGive(self->mutex);

  return true;
}
