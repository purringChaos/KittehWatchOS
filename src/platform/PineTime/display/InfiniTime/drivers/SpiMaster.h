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

namespace Pinetime {
namespace Drivers {
class SpiMaster {
public:
  ;

  SpiMaster(const uint8_t spi, const uint8_t bitOrder, const uint8_t mode,
            const uint8_t pinSCK, const uint8_t pinMOSI, const uint8_t pinMISO);
  SpiMaster(const SpiMaster &) = delete;
  SpiMaster &operator=(const SpiMaster &) = delete;
  SpiMaster(SpiMaster &&) = delete;
  SpiMaster &operator=(SpiMaster &&) = delete;

  bool Init();
  bool Write(uint8_t pinCsn, const uint8_t *data, size_t size);
  bool Read(uint8_t pinCsn, uint8_t *cmd, size_t cmdSize, uint8_t *data,
            size_t dataSize);

  bool WriteCmdAndBuffer(uint8_t pinCsn, const uint8_t *cmd, size_t cmdSize,
                         const uint8_t *data, size_t dataSize);

  void OnStartedEvent();
  void OnEndEvent();

  void Sleep();
  void Wakeup();

    void SetupWorkaroundForFtpan58(NRF_SPIM_Type *spim, uint32_t ppi_channel,
                                 uint32_t gpiote_channel);
  void DisableWorkaroundForFtpan58(NRF_SPIM_Type *spim, uint32_t ppi_channel,
                                   uint32_t gpiote_channel);
  void PrepareTx(const volatile uint32_t bufferAddress,
                 const volatile size_t size);
  void PrepareRx(const volatile uint32_t cmdAddress,
                 const volatile size_t cmdSize,
                 const volatile uint32_t bufferAddress,
                 const volatile size_t size);


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
  SemaphoreHandle_t mutex = NULL;
};
} // namespace Drivers
} // namespace Pinetime
