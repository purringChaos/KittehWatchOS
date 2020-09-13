#include "St7789.h"
#include <hal/nrf_gpio.h>
#include <libraries/delay/nrf_delay.h>

static uint8_t St7789_Command_SoftwareReset = 0x01;
static uint8_t St7789_Command_SleepIn = 0x10;
static uint8_t St7789_Command_SleepOut = 0x11;
static uint8_t St7789_Command_NormalModeOn = 0x13;
static uint8_t St7789_Command_DisplayInversionOn = 0x21;
static uint8_t St7789_Command_DisplayOff = 0x28;
static uint8_t St7789_Command_DisplayOn = 0x29;
static uint8_t St7789_Command_ColumnAddressSet = 0x2a;
static uint8_t St7789_Command_RowAddressSet = 0x2b;
static uint8_t St7789_Command_WriteToRam = 0x2c;
static uint8_t St7789_Command_MemoryDataAccessControl = 036;
static uint8_t St7789_Command_VerticalScrollDefinition = 0x33;
static uint8_t St7789_Command_VerticalScrollStartAddress = 0x37;
static uint8_t St7789_Command_ColMod = 0x3a;

void St7789_Init(St7789 *self) {
  nrf_gpio_cfg_output(self->pinCsm);
  nrf_gpio_pin_set(self->pinCsm);
  nrf_gpio_cfg_output(self->pinDataCommand);
  nrf_gpio_cfg_output(26);
  nrf_gpio_pin_set(26);
  St7789_HardwareReset(self);
  St7789_SoftwareReset(self);
  St7789_SleepOut(self);
  St7789_ColMod(self);
  St7789_MemoryDataAccessControl(self);
  St7789_ColumnAddressSet(self);
  St7789_RowAddressSet(self);
  St7789_DisplayInversionOn(self);
  St7789_NormalModeOn(self);
  St7789_DisplayOn(self);
}

void St7789_WriteCommand(St7789 *self, uint8_t cmd) {
  nrf_gpio_pin_clear(self->pinDataCommand);
  St7789_WriteSpi(self, &cmd, 1);
}

void St7789_WriteData(St7789 *self, uint8_t data) {
  nrf_gpio_pin_set(self->pinDataCommand);
  St7789_WriteSpi(self, &data, 1);
}

void St7789_WriteSpi(St7789 *self, const uint8_t *data, size_t size) {
  SpiMaster_Write(self->spiMaster, self->pinCsm, data, size);
}

void St7789_SoftwareReset(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_SoftwareReset);
  nrf_delay_ms(150);
}

void St7789_SleepOut(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_SleepOut);
}

void St7789_SleepIn(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_SleepIn);
}

void St7789_ColMod(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_ColMod);
  St7789_WriteData(self, 0x55);
  nrf_delay_ms(10);
}

void St7789_MemoryDataAccessControl(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_MemoryDataAccessControl);
  St7789_WriteData(self, 0x00);
}

void St7789_ColumnAddressSet(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_ColumnAddressSet);
  St7789_WriteData(self, 0x00);
  St7789_WriteData(self, 0x00);
  St7789_WriteData(self, Width >> 8u);
  St7789_WriteData(self, Width & 0xffu);
}

void St7789_RowAddressSet(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_RowAddressSet);
  St7789_WriteData(self, 0x00);
  St7789_WriteData(self, 0x00);
  St7789_WriteData(self, 320u >> 8u);
  St7789_WriteData(self, 320u & 0xffu);
}

void St7789_DisplayInversionOn(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_DisplayInversionOn);
  nrf_delay_ms(10);
}

void St7789_NormalModeOn(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_NormalModeOn);
  nrf_delay_ms(10);
}

void St7789_DisplayOn(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_DisplayOn);
}

void St7789_SetAddrWindow(St7789 *self, uint16_t x0, uint16_t y0, uint16_t x1,
                          uint16_t y1) {
  St7789_WriteCommand(self, St7789_Command_ColumnAddressSet);
  St7789_WriteData(self, x0 >> 8);
  St7789_WriteData(self, x0 & 0xff);
  St7789_WriteData(self, x1 >> 8);
  St7789_WriteData(self, x1 & 0xff);
  St7789_WriteCommand(self, St7789_Command_RowAddressSet);
  St7789_WriteData(self, y0 >> 8);
  St7789_WriteData(self, y0 & 0xff);
  St7789_WriteData(self, y1 >> 8);
  St7789_WriteData(self, y1 & 0xff);
  St7789_WriteToRam(self);
}

void St7789_WriteToRam(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_WriteToRam);
}

void St7789_DisplayOff(St7789 *self) {
  St7789_WriteCommand(self, St7789_Command_DisplayOff);
  nrf_delay_ms(500);
}

void St7789_VerticalScrollDefinition(St7789 *self, uint16_t topFixedLines,
                                     uint16_t scrollLines,
                                     uint16_t bottomFixedLines) {
  St7789_WriteCommand(self, St7789_Command_VerticalScrollDefinition);
  St7789_WriteData(self, topFixedLines >> 8u);
  St7789_WriteData(self, topFixedLines & 0x00ffu);
  St7789_WriteData(self, scrollLines >> 8u);
  St7789_WriteData(self, scrollLines & 0x00ffu);
  St7789_WriteData(self, bottomFixedLines >> 8u);
  St7789_WriteData(self, bottomFixedLines & 0x00ffu);
}

void St7789_VerticalScrollStartAddress(St7789 *self, uint16_t line) {
  self->verticalScrollingStartAddress = line;
  St7789_WriteCommand(self, St7789_Command_VerticalScrollStartAddress);
  St7789_WriteData(self, line >> 8u);
  St7789_WriteData(self, line & 0x00ffu);
}

void St7789_Uninit(St7789 *self) {}

void St7789_DrawPixel(St7789 *self, uint16_t x, uint16_t y, uint32_t color) {
  if ((x < 0) || (x >= Width) || (y < 0) || (y >= Height))
    return;

  St7789_SetAddrWindow(self, x, y, x + 1, y + 1);

  nrf_gpio_pin_set(self->pinDataCommand);
  St7789_WriteSpi(self, (const uint8_t *)&color, 2);
}

void St7789_BeginDrawBuffer(St7789 *self, uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height) {
  if ((x >= Width) || (y >= Height))
    return;
  if ((x + width - 1) >= Width)
    width = Width - x;
  if ((y + height - 1) >= Height)
    height = Height - y;

  St7789_SetAddrWindow(self, 0 + x, ST7789_ROW_OFFSET + y, x + width - 1,
                       y + height - 1);
  nrf_gpio_pin_set(self->pinDataCommand);
}

void St7789_NextDrawBuffer(St7789 *self, const uint8_t *data, size_t size) {
  St7789_WriteSpi(self, data, size);
}

void St7789_HardwareReset(St7789 *self) {
  nrf_gpio_pin_clear(26);
  nrf_delay_ms(10);
  nrf_gpio_pin_set(26);
}

void St7789_Sleep(St7789 *self) {
  St7789_SleepIn(self);
  nrf_gpio_cfg_default(self->pinDataCommand);
  //  spi.Sleep(); // TODO sleep SPI
}

void St7789_Wakeup(St7789 *self) {
  //  spi.Wakeup(); // TODO wake up SPI

  nrf_gpio_cfg_output(self->pinDataCommand);
  // TODO why do we need to reset the controller?
  St7789_HardwareReset(self);
  St7789_SoftwareReset(self);
  St7789_SleepOut(self);
  St7789_ColMod(self);
  St7789_MemoryDataAccessControl(self);
  St7789_ColumnAddressSet(self);
  St7789_RowAddressSet(self);
  St7789_DisplayInversionOn(self);
  St7789_NormalModeOn(self);
  St7789_VerticalScrollStartAddress(self, self->verticalScrollingStartAddress);
  St7789_DisplayOn(self);
}
