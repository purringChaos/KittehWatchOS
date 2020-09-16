#include "types/numbers.h"
#include <FreeRTOS.h>
#include <nrf_gpio.h>
#include <nrfx_twi.h>
#include <semphr.h>
#include <string.h>
SemaphoreHandle_t i2c_mutex;

#define WAIT(X)                                                                \
  while (X) {                                                                  \
  };

void I2C_Init() {
  i2c_mutex = xSemaphoreCreateBinary();

  NRF_GPIO->PIN_CNF[7] =
      ((u32)GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
      ((u32)GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
      ((u32)GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
      ((u32)GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) |
      ((u32)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);

  NRF_GPIO->PIN_CNF[6] =
      ((u32)GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
      ((u32)GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
      ((u32)GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
      ((u32)GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos) |
      ((u32)GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);

  NRF_TWIM1->FREQUENCY = 0x06200000;

  NRF_TWIM1->PSEL.SCL = 7;
  NRF_TWIM1->PSEL.SDA = 6;
  NRF_TWIM1->EVENTS_LASTRX = 0;
  NRF_TWIM1->EVENTS_STOPPED = 0;
  NRF_TWIM1->EVENTS_LASTTX = 0;
  NRF_TWIM1->EVENTS_ERROR = 0;
  NRF_TWIM1->EVENTS_RXSTARTED = 0;
  NRF_TWIM1->EVENTS_SUSPENDED = 0;
  NRF_TWIM1->EVENTS_TXSTARTED = 0;

  NRF_TWIM1->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
  xSemaphoreGive(i2c_mutex);
}

void _I2C_AdvancedRead(u8 dev, u8 *buffer, size_t size,
                       bool waitUntilFinished) {
  NRF_TWIM1->ADDRESS = dev;
  NRF_TWIM1->TASKS_RESUME = 0x1UL;
  NRF_TWIM1->RXD.PTR = (u32)buffer;
  NRF_TWIM1->RXD.MAXCNT = size;

  NRF_TWIM1->TASKS_STARTRX = 1;

  WAIT(!NRF_TWIM1->EVENTS_RXSTARTED && !NRF_TWIM1->EVENTS_ERROR)
  NRF_TWIM1->EVENTS_RXSTARTED = 0x0UL;

  WAIT(!NRF_TWIM1->EVENTS_LASTRX && !NRF_TWIM1->EVENTS_ERROR)

  NRF_TWIM1->EVENTS_LASTRX = 0x0UL;

  if (waitUntilFinished || NRF_TWIM1->EVENTS_ERROR) {
    NRF_TWIM1->TASKS_STOP = 0x1UL;
    WAIT(!NRF_TWIM1->EVENTS_STOPPED)

    NRF_TWIM1->EVENTS_STOPPED = 0x0UL;
  } else {
    NRF_TWIM1->TASKS_SUSPEND = 0x1UL;
    WAIT(!NRF_TWIM1->EVENTS_SUSPENDED)

    NRF_TWIM1->EVENTS_SUSPENDED = 0x0UL;
  }

  if (NRF_TWIM1->EVENTS_ERROR) {
    NRF_TWIM1->EVENTS_ERROR = 0x0UL;
  }
}

void _I2C_AdvancedWrite(u8 dev, const u8 *data, size_t size,
                        bool waitUntilFinished) {
  NRF_TWIM1->ADDRESS = dev;
  NRF_TWIM1->TASKS_RESUME = 0x1UL;
  NRF_TWIM1->TXD.PTR = (u32)data;
  NRF_TWIM1->TXD.MAXCNT = size;

  NRF_TWIM1->TASKS_STARTTX = 1;

  WAIT(!NRF_TWIM1->EVENTS_TXSTARTED && !NRF_TWIM1->EVENTS_ERROR)

  NRF_TWIM1->EVENTS_TXSTARTED = 0x0UL;

  WAIT(!NRF_TWIM1->EVENTS_LASTTX && !NRF_TWIM1->EVENTS_ERROR)

  NRF_TWIM1->EVENTS_LASTTX = 0x0UL;

  if (waitUntilFinished || NRF_TWIM1->EVENTS_ERROR) {
    NRF_TWIM1->TASKS_STOP = 0x1UL;
    WAIT(!NRF_TWIM1->EVENTS_STOPPED)

    NRF_TWIM1->EVENTS_STOPPED = 0x0UL;
  } else {
    NRF_TWIM1->TASKS_SUSPEND = 0x1UL;
    WAIT(!NRF_TWIM1->EVENTS_SUSPENDED)

    NRF_TWIM1->EVENTS_SUSPENDED = 0x0UL;
  }

  if (NRF_TWIM1->EVENTS_ERROR) {
    NRF_TWIM1->EVENTS_ERROR = 0x0UL;
    u32 error = NRF_TWIM1->ERRORSRC;
    NRF_TWIM1->ERRORSRC = error;
  }
}

void I2C_Read(u8 dev, u8 reg, u8 *data, size_t size) {
  xSemaphoreTake(i2c_mutex, portMAX_DELAY);
  _I2C_AdvancedWrite(dev, &reg, 1, false);
  _I2C_AdvancedRead(dev, data, size, true);
  xSemaphoreGive(i2c_mutex);
}

void I2C_Write(u8 dev, u8 reg, const u8 *data, size_t size) {
  xSemaphoreTake(i2c_mutex, portMAX_DELAY);
  u8 writeBuffer[9];
  writeBuffer[0] = reg;
  memcpy(writeBuffer + 1, data, size);
  _I2C_AdvancedWrite(dev, writeBuffer, size + 1, true);
  xSemaphoreGive(i2c_mutex);
}
