#ifndef PINETIME_I2C_H
#define PINETIME_I2C_H
#include "types/numbers.h"

void I2C_Init();
void _I2C_AdvancedRead(u8 dev, u8 *buffer, size_t size, bool waitUntilFinished);
void _I2C_AdvancedWrite(u8 dev, const u8 *data, size_t size,
                        bool waitUntilFinished);
void I2C_Read(u8 dev, u8 reg, u8 *data, size_t size);
void I2C_Write(u8 dev, u8 reg, const u8 *data, size_t size);

#endif