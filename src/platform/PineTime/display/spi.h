#ifndef PINETIME_SPI_H
#define PINETIME_SPI_H
#include "types/numbers.h"
#include <stdbool.h>
#include <stddef.h>
extern bool SPI_Init();
extern bool SPI_Write(u8 newChipSelectPin, const u8 *data, size_t size);
extern bool SPI_Read(u8 newChipSelectPin, u8 *cmd, size_t cmdSize, u8 *data,
                     size_t dataSize);

extern void FinishedTransferCallback();
#endif