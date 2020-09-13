#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif

const uint8_t SpiMaster_SPI0 = 0;
const uint8_t SpiMaster_SPI1 = 1;

const uint8_t SpiMaster_Msb_Lsb = 0;
const uint8_t SpiMaster_Lsb_Msb = 1;

const uint8_t SpiMaster_Mode0 = 0;
const uint8_t SpiMaster_Mode1 = 1;
const uint8_t SpiMaster_Mode2 = 2;
const uint8_t SpiMaster_Mode3 = 3;

typedef struct {
  NRF_SPIM_Type *spiBaseAddress;
  uint8_t pinCsn;

  uint8_t spi;
  uint8_t bitOrder;
  uint8_t mode;
  uint8_t pinSCK;
  uint8_t pinMOSI;
  uint8_t pinMISO;

  volatile uint32_t currentBufferAddr;
  volatile size_t currentBufferSize;
  volatile TaskHandle_t taskToNotify;
  SemaphoreHandle_t mutex;
} SpiMaster;

bool SpiMaster_Init(SpiMaster *self);
bool SpiMaster_Write(SpiMaster *self, uint8_t pinCsn, const uint8_t *data,
                     size_t size);
bool SpiMaster_Read(SpiMaster *self, uint8_t pinCsn, uint8_t *cmd,
                    size_t cmdSize, uint8_t *data, size_t dataSize);

bool SpiMaster_WriteCmdAndBuffer(SpiMaster *self, uint8_t pinCsn,
                                 const uint8_t *cmd, size_t cmdSize,
                                 const uint8_t *data, size_t dataSize);

void SpiMaster_OnStartedEvent(SpiMaster *self);
void SpiMaster_OnEndEvent(SpiMaster *self);

void SpiMaster_Sleep(SpiMaster *self);
void SpiMaster_Wakeup(SpiMaster *self);

void SpiMaster_SetupWorkaroundForFtpan58(SpiMaster *self, NRF_SPIM_Type *spim,
                                         uint32_t ppi_channel,
                                         uint32_t gpiote_channel);
void SpiMaster_DisableWorkaroundForFtpan58(SpiMaster *self, NRF_SPIM_Type *spim,
                                           uint32_t ppi_channel,
                                           uint32_t gpiote_channel);
void SpiMaster_PrepareTx(SpiMaster *self, const volatile uint32_t bufferAddress,
                         const volatile size_t size);
void SpiMaster_PrepareRx(SpiMaster *self, const volatile uint32_t cmdAddress,
                         const volatile size_t cmdSize,
                         const volatile uint32_t bufferAddress,
                         const volatile size_t size);

#ifdef __cplusplus
}
#endif