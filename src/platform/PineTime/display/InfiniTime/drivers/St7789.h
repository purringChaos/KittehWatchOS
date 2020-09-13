#pragma once
#include "SpiMaster.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  SpiMaster *spiMaster;
  uint8_t pinCsm;
  uint8_t pinDataCommand;
  uint8_t verticalScrollingStartAddress;
} St7789;

void St7789_Init(St7789 *self);
void St7789_Uninit(St7789 *self);
void St7789_DrawPixel(St7789 *self, uint16_t x, uint16_t y, uint32_t color);
void St7789_VerticalScrollDefinition(St7789 *self, uint16_t topFixedLines,
          uint16_t scrollLines,
          uint16_t bottomFixedLines);
void St7789_VerticalScrollStartAddress(St7789 *self, uint16_t line);
void St7789_BeginDrawBuffer(St7789 *self, uint16_t x, uint16_t y, uint16_t width,
                            uint16_t height);
void St7789_NextDrawBuffer(St7789 *self, const uint8_t *data, size_t size);
void St7789_DisplayOn(St7789 *self);
void St7789_DisplayOff(St7789 *self);
void St7789_Sleep(St7789 *self);
void St7789_Wakeup(St7789 *self);
void St7789_HardwareReset(St7789 *self);
void St7789_SoftwareReset(St7789 *self);
void St7789_SleepOut(St7789 *self);
void St7789_SleepIn(St7789 *self);
void St7789_ColMod(St7789 *self);
void St7789_MemoryDataAccessControl(St7789 *self);
void St7789_DisplayInversionOn(St7789 *self);
void St7789_NormalModeOn(St7789 *self);
void St7789_WriteToRam(St7789 *self);
void St7789_SetAddrWindow(St7789 *self, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void St7789_WriteCommand(St7789 *self, uint8_t cmd);
void St7789_WriteSpi(St7789 *self, const uint8_t *data, size_t size);
void St7789_WriteData(St7789 *self, uint8_t data);
void St7789_ColumnAddressSet(St7789 *self);
void St7789_RowAddressSet(St7789 *self);

static uint16_t Width = 240;
static uint16_t Height = 240;



#ifdef __cplusplus
}
#endif