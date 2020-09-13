#pragma once
#include <FreeRTOS.h>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <semphr.h>
#include <task.h>

#include "BufferProvider.h"
#include <semphr.h>

const uint8_t SpiMaster_SPI0 = 0;
const uint8_t SpiMaster_SPI1 = 1;

const uint8_t SpiMaster_Msb_Lsb = 0;
const uint8_t SpiMaster_Lsb_Msb = 1;

const uint8_t SpiMaster_Mode0 = 0;
const uint8_t SpiMaster_Mode1 = 1;
const uint8_t SpiMaster_Mode2 = 2;
const uint8_t SpiMaster_Mode3 = 3;

struct SpiMaster {
  NRF_SPIM_Type *spiBaseAddress;
  uint8_t pinCsn;

  uint8_t spi;
  uint8_t bitOrder;
  uint8_t mode;
  uint8_t pinSCK;
  uint8_t pinMOSI;
  uint8_t pinMISO;

  volatile uint32_t currentBufferAddr  = 0;
  volatile size_t currentBufferSize = 0;
  volatile TaskHandle_t taskToNotify = NULL;
  SemaphoreHandle_t mutex = NULL;
};

bool SpiMaster_Init(struct SpiMaster *self);
bool SpiMaster_Write(struct SpiMaster *self, uint8_t pinCsn, const uint8_t *data,
                     size_t size);
bool SpiMaster_Read(struct SpiMaster *self, uint8_t pinCsn, uint8_t *cmd,
                    size_t cmdSize, uint8_t *data, size_t dataSize);

bool SpiMaster_WriteCmdAndBuffer(struct SpiMaster *self, uint8_t pinCsn,
                                 const uint8_t *cmd, size_t cmdSize,
                                 const uint8_t *data, size_t dataSize);

void SpiMaster_OnStartedEvent(struct SpiMaster *self);
void SpiMaster_OnEndEvent(struct SpiMaster *self);

void SpiMaster_Sleep(struct SpiMaster *self);
void SpiMaster_Wakeup(struct SpiMaster *self);

void SpiMaster_SetupWorkaroundForFtpan58(struct SpiMaster *self,
                                         NRF_SPIM_Type *spim,
                                         uint32_t ppi_channel,
                                         uint32_t gpiote_channel);
void SpiMaster_DisableWorkaroundForFtpan58(struct SpiMaster *self,
                                           NRF_SPIM_Type *spim,
                                           uint32_t ppi_channel,
                                           uint32_t gpiote_channel);
void SpiMaster_PrepareTx(struct SpiMaster *self,
                         const volatile uint32_t bufferAddress,
                         const volatile size_t size);
void SpiMaster_PrepareRx(struct SpiMaster *self,
                         const volatile uint32_t cmdAddress,
                         const volatile size_t cmdSize,
                         const volatile uint32_t bufferAddress,
                         const volatile size_t size);