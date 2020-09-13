#include "SpiMaster.h"
#include <FreeRTOS.h>
#include <algorithm>
#include <hal/nrf_gpio.h>
#include <hal/nrf_spim.h>
#include <task.h>

using namespace Pinetime::Drivers;

SpiMaster::SpiMaster(const uint8_t spi, const uint8_t bitOrder,
                     const uint8_t mode, const uint8_t pinSCK,
                     const uint8_t pinMOSI, const uint8_t pinMISO)
    : spi{spi}, bitOrder{bitOrder}, mode{mode}, pinSCK{pinSCK},
      pinMOSI{pinMOSI}, pinMISO{pinMISO} {}

bool SpiMaster::Init() {
  if (mutex == NULL) {
    currentBufferAddr = 0;
    currentBufferSize = 0;
    mutex = xSemaphoreCreateBinary();
  }
  nrf_gpio_pin_set(pinSCK);
  nrf_gpio_cfg_output(pinSCK);
  nrf_gpio_pin_clear(pinMOSI);
  nrf_gpio_cfg_output(pinMOSI);
  nrf_gpio_cfg_input(pinMISO, NRF_GPIO_PIN_NOPULL);
  //  nrf_gpio_cfg_output(pinCSN);
  //  pinCsn = pinCSN;

  switch (spi) {
  case SpiMaster_SPI0:
    spiBaseAddress = NRF_SPIM0;
    break;
  case SpiMaster_SPI1:
    spiBaseAddress = NRF_SPIM1;
    break;
  default:
    return false;
  }

  /* Configure pins, frequency and mode */
  spiBaseAddress->PSELSCK = pinSCK;
  spiBaseAddress->PSELMOSI = pinMOSI;
  spiBaseAddress->PSELMISO = pinMISO;
  spiBaseAddress->FREQUENCY = 0x80000000;

  uint32_t regConfig = 0;
  switch (bitOrder) {
  case SpiMaster_Msb_Lsb:
    break;
  case SpiMaster_Lsb_Msb:
    regConfig = 1;
  default:
    return false;
  }
  switch (mode) {
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

  spiBaseAddress->CONFIG = regConfig;
  spiBaseAddress->EVENTS_ENDRX = 0;
  spiBaseAddress->EVENTS_ENDTX = 0;
  spiBaseAddress->EVENTS_END = 0;

  spiBaseAddress->INTENSET = ((unsigned)1 << (unsigned)6);
  spiBaseAddress->INTENSET = ((unsigned)1 << (unsigned)1);
  spiBaseAddress->INTENSET = ((unsigned)1 << (unsigned)19);

  spiBaseAddress->ENABLE =
      (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);

  NRFX_IRQ_PRIORITY_SET(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, 2);
  NRFX_IRQ_ENABLE(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);

  xSemaphoreGive(mutex);
  return true;
}

void SpiMaster::SetupWorkaroundForFtpan58(NRF_SPIM_Type *spim,
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
  spiBaseAddress->EVENTS_END = 0;

  // Disable IRQ
  spim->INTENCLR = (1 << 6);
  spim->INTENCLR = (1 << 1);
  spim->INTENCLR = (1 << 19);
}

void SpiMaster::DisableWorkaroundForFtpan58(NRF_SPIM_Type *spim,
                                            uint32_t ppi_channel,
                                            uint32_t gpiote_channel) {
  NRF_GPIOTE->CONFIG[gpiote_channel] = 0;
  NRF_PPI->CH[ppi_channel].EEP = 0;
  NRF_PPI->CH[ppi_channel].TEP = 0;
  NRF_PPI->CHENSET = ppi_channel;
  spiBaseAddress->EVENTS_END = 0;
  spim->INTENSET = (1 << 6);
  spim->INTENSET = (1 << 1);
  spim->INTENSET = (1 << 19);
}

void SpiMaster::OnEndEvent() {
  if (currentBufferAddr == 0) {
    return;
  }

  auto s = currentBufferSize;
  if (s > 0) {
    auto currentSize = std::min((size_t)255, s);
    PrepareTx(currentBufferAddr, currentSize);
    currentBufferAddr += currentSize;
    currentBufferSize -= currentSize;

    spiBaseAddress->TASKS_START = 1;
  } else {
    uint8_t *buffer = nullptr;
    size_t size = 0;
    if (taskToNotify != nullptr) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      vTaskNotifyGiveFromISR(taskToNotify, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    nrf_gpio_pin_set(this->pinCsn);
    currentBufferAddr = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(mutex, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void SpiMaster::OnStartedEvent() {}

void SpiMaster::PrepareTx(const volatile uint32_t bufferAddress,
                          const volatile size_t size) {
  spiBaseAddress->TXD.PTR = bufferAddress;
  spiBaseAddress->TXD.MAXCNT = size;
  spiBaseAddress->TXD.LIST = 0;
  spiBaseAddress->RXD.PTR = 0;
  spiBaseAddress->RXD.MAXCNT = 0;
  spiBaseAddress->RXD.LIST = 0;
  spiBaseAddress->EVENTS_END = 0;
}

void SpiMaster::PrepareRx(const volatile uint32_t cmdAddress,
                          const volatile size_t cmdSize,
                          const volatile uint32_t bufferAddress,
                          const volatile size_t size) {
  spiBaseAddress->TXD.PTR = 0;
  spiBaseAddress->TXD.MAXCNT = 0;
  spiBaseAddress->TXD.LIST = 0;
  spiBaseAddress->RXD.PTR = bufferAddress;
  spiBaseAddress->RXD.MAXCNT = size;
  spiBaseAddress->RXD.LIST = 0;
  spiBaseAddress->EVENTS_END = 0;
}

bool SpiMaster::Write(uint8_t pinCsn, const uint8_t *data, size_t size) {
  if (data == nullptr)
    return false;
  auto ok = xSemaphoreTake(mutex, portMAX_DELAY);
  ASSERT(ok == true);
  taskToNotify = xTaskGetCurrentTaskHandle();

  this->pinCsn = pinCsn;

  if (size == 1) {
    SetupWorkaroundForFtpan58(spiBaseAddress, 0, 0);
  } else {
    DisableWorkaroundForFtpan58(spiBaseAddress, 0, 0);
  }

  nrf_gpio_pin_clear(this->pinCsn);

  currentBufferAddr = (uint32_t)data;
  currentBufferSize = size;

  auto currentSize = std::min((size_t)255, (size_t)currentBufferSize);
  PrepareTx(currentBufferAddr, currentSize);
  currentBufferSize -= currentSize;
  currentBufferAddr += currentSize;
  spiBaseAddress->TASKS_START = 1;

  if (size == 1) {
    while (spiBaseAddress->EVENTS_END == 0)
      ;
    nrf_gpio_pin_set(this->pinCsn);
    currentBufferAddr = 0;
    xSemaphoreGive(mutex);
  }

  return true;
}

bool SpiMaster::Read(uint8_t pinCsn, uint8_t *cmd, size_t cmdSize,
                     uint8_t *data, size_t dataSize) {
  xSemaphoreTake(mutex, portMAX_DELAY);

  taskToNotify = nullptr;

  this->pinCsn = pinCsn;
  DisableWorkaroundForFtpan58(spiBaseAddress, 0, 0);
  spiBaseAddress->INTENCLR = (1 << 6);
  spiBaseAddress->INTENCLR = (1 << 1);
  spiBaseAddress->INTENCLR = (1 << 19);

  nrf_gpio_pin_clear(this->pinCsn);

  currentBufferAddr = 0;
  currentBufferSize = 0;

  PrepareTx((uint32_t)cmd, cmdSize);
  spiBaseAddress->TASKS_START = 1;
  while (spiBaseAddress->EVENTS_END == 0)
    ;

  PrepareRx((uint32_t)cmd, cmdSize, (uint32_t)data, dataSize);
  spiBaseAddress->TASKS_START = 1;

  while (spiBaseAddress->EVENTS_END == 0)
    ;
  nrf_gpio_pin_set(this->pinCsn);

  xSemaphoreGive(mutex);

  return true;
}

void SpiMaster::Sleep() {
  while (spiBaseAddress->ENABLE != 0) {
    spiBaseAddress->ENABLE =
        (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
  }
  nrf_gpio_cfg_default(pinSCK);
  nrf_gpio_cfg_default(pinMOSI);
  nrf_gpio_cfg_default(pinMISO);
}

void SpiMaster::Wakeup() { Init(); }

bool SpiMaster::WriteCmdAndBuffer(uint8_t pinCsn, const uint8_t *cmd,
                                  size_t cmdSize, const uint8_t *data,
                                  size_t dataSize) {
  xSemaphoreTake(mutex, portMAX_DELAY);

  taskToNotify = nullptr;

  this->pinCsn = pinCsn;
  DisableWorkaroundForFtpan58(spiBaseAddress, 0, 0);
  spiBaseAddress->INTENCLR = (1 << 6);
  spiBaseAddress->INTENCLR = (1 << 1);
  spiBaseAddress->INTENCLR = (1 << 19);

  nrf_gpio_pin_clear(this->pinCsn);

  currentBufferAddr = 0;
  currentBufferSize = 0;

  PrepareTx((uint32_t)cmd, cmdSize);
  spiBaseAddress->TASKS_START = 1;
  while (spiBaseAddress->EVENTS_END == 0)
    ;

  PrepareTx((uint32_t)data, dataSize);
  spiBaseAddress->TASKS_START = 1;

  while (spiBaseAddress->EVENTS_END == 0)
    ;
  nrf_gpio_pin_set(this->pinCsn);

  xSemaphoreGive(mutex);

  return true;
}
