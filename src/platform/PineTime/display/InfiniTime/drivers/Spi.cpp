#include "Spi.h"
#include <hal/nrf_gpio.h>

using namespace Pinetime::Drivers;

Spi::Spi(SpiMaster *spiMaster, uint8_t pinCsn)
    : spiMaster{spiMaster}, pinCsn{pinCsn} {
  nrf_gpio_cfg_output(pinCsn);
  nrf_gpio_pin_set(pinCsn);
}

bool Spi::Write(const uint8_t *data, size_t size) {
  return SpiMaster_Write(spiMaster, pinCsn, data, size);
}

bool Spi::Read(uint8_t *cmd, size_t cmdSize, uint8_t *data, size_t dataSize) {
  return SpiMaster_Read(spiMaster, pinCsn, cmd, cmdSize, data, dataSize);
}

void Spi::Sleep() {
  // TODO sleep spi
  nrf_gpio_cfg_default(pinCsn);
}

bool Spi::Init() {
  nrf_gpio_pin_set(pinCsn); /* disable Set slave select (inactive high) */
  return true;
}

bool Spi::WriteCmdAndBuffer(const uint8_t *cmd, size_t cmdSize,
                            const uint8_t *data, size_t dataSize) {
  return SpiMaster_WriteCmdAndBuffer(spiMaster, pinCsn, cmd, cmdSize, data,
                                     dataSize);
}
