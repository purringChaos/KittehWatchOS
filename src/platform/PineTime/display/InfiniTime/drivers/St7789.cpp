#include "St7789.h"
#include "Spi.h"
#include <hal/nrf_gpio.h>
#include <libraries/delay/nrf_delay.h>

using namespace Pinetime::Drivers;


static uint8_t    St7789_Command_SoftwareReset = 0x01;
static uint8_t    St7789_Command_SleepIn = 0x10;
static uint8_t    St7789_Command_SleepOut = 0x11;
static uint8_t    St7789_Command_NormalModeOn = 0x13;
static uint8_t    St7789_Command_DisplayInversionOn = 0x21;
static uint8_t    St7789_Command_DisplayOff = 0x28;
static uint8_t    St7789_Command_DisplayOn = 0x29;
static uint8_t    St7789_Command_ColumnAddressSet = 0x2a;
static uint8_t    St7789_Command_RowAddressSet = 0x2b;
static uint8_t    St7789_Command_WriteToRam = 0x2c;
static uint8_t    St7789_Command_MemoryDataAccessControl = 036;
static uint8_t    St7789_Command_VerticalScrollDefinition = 0x33;
static uint8_t    St7789_Command_VerticalScrollStartAddress = 0x37;
static uint8_t    St7789_Command_ColMod = 0x3a;

St7789::St7789(SpiMaster *spiMaster, uint8_t pinCsm, uint8_t pinDataCommand)
    : spiMaster{spiMaster}, pinCsm{pinCsm}, pinDataCommand{pinDataCommand} {}

void St7789::Init() {
  nrf_gpio_cfg_output(pinCsm);
  nrf_gpio_pin_set(pinCsm);
  nrf_gpio_cfg_output(pinDataCommand);
  nrf_gpio_cfg_output(26);
  nrf_gpio_pin_set(26);
  HardwareReset();
  SoftwareReset();
  SleepOut();
  ColMod();
  MemoryDataAccessControl();
  ColumnAddressSet();
  RowAddressSet();
  DisplayInversionOn();
  NormalModeOn();
  DisplayOn();
}

void St7789::WriteCommand(uint8_t cmd) {
  nrf_gpio_pin_clear(pinDataCommand);
  WriteSpi(&cmd, 1);
}

void St7789::WriteData(uint8_t data) {
  nrf_gpio_pin_set(pinDataCommand);
  WriteSpi(&data, 1);
}

void St7789::WriteSpi(const uint8_t *data, size_t size) {
  SpiMaster_Write(spiMaster, pinCsm, data, size);
}

void St7789::SoftwareReset() {
  WriteCommand(St7789_Command_SoftwareReset);
  nrf_delay_ms(150);
}

void St7789::SleepOut() {
  WriteCommand(St7789_Command_SleepOut);
}

void St7789::SleepIn() {
  WriteCommand(St7789_Command_SleepIn);
}

void St7789::ColMod() {
  WriteCommand(St7789_Command_ColMod);
  WriteData(0x55);
  nrf_delay_ms(10);
}

void St7789::MemoryDataAccessControl() {
  WriteCommand(St7789_Command_MemoryDataAccessControl);
  WriteData(0x00);
}

void St7789::ColumnAddressSet() {
  WriteCommand(St7789_Command_ColumnAddressSet);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(Width >> 8u);
  WriteData(Width & 0xffu);
}

void St7789::RowAddressSet() {
  WriteCommand(St7789_Command_RowAddressSet);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(320u >> 8u);
  WriteData(320u & 0xffu);
}

void St7789::DisplayInversionOn() {
  WriteCommand(St7789_Command_DisplayInversionOn);
  nrf_delay_ms(10);
}

void St7789::NormalModeOn() {
  WriteCommand(St7789_Command_NormalModeOn);
  nrf_delay_ms(10);
}

void St7789::DisplayOn() {
  WriteCommand(St7789_Command_DisplayOn);
}

void St7789::SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  WriteCommand(St7789_Command_ColumnAddressSet);
  WriteData(x0 >> 8);
  WriteData(x0 & 0xff);
  WriteData(x1 >> 8);
  WriteData(x1 & 0xff);

  WriteCommand(St7789_Command_RowAddressSet);
  WriteData(y0 >> 8);
  WriteData(y0 & 0xff);
  WriteData(y1 >> 8);
  WriteData(y1 & 0xff);

  WriteToRam();
}

void St7789::WriteToRam() {
  WriteCommand(St7789_Command_WriteToRam);
}

void St7789::DisplayOff() {
  WriteCommand(St7789_Command_DisplayOff);
  nrf_delay_ms(500);
}

void St7789::VerticalScrollDefinition(uint16_t topFixedLines,
                                      uint16_t scrollLines,
                                      uint16_t bottomFixedLines) {
  WriteCommand(St7789_Command_VerticalScrollDefinition);
  WriteData(topFixedLines >> 8u);
  WriteData(topFixedLines & 0x00ffu);
  WriteData(scrollLines >> 8u);
  WriteData(scrollLines & 0x00ffu);
  WriteData(bottomFixedLines >> 8u);
  WriteData(bottomFixedLines & 0x00ffu);
}

void St7789::VerticalScrollStartAddress(uint16_t line) {
  verticalScrollingStartAddress = line;
  WriteCommand(St7789_Command_VerticalScrollStartAddress);
  WriteData(line >> 8u);
  WriteData(line & 0x00ffu);
}

void St7789::Uninit() {}

void St7789::DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
  if ((x < 0) || (x >= Width) || (y < 0) || (y >= Height))
    return;

  SetAddrWindow(x, y, x + 1, y + 1);

  nrf_gpio_pin_set(pinDataCommand);
  WriteSpi(reinterpret_cast<const uint8_t *>(&color), 2);
}

void St7789::BeginDrawBuffer(uint16_t x, uint16_t y, uint16_t width,
                             uint16_t height) {
  if ((x >= Width) || (y >= Height))
    return;
  if ((x + width - 1) >= Width)
    width = Width - x;
  if ((y + height - 1) >= Height)
    height = Height - y;

  SetAddrWindow(0 + x, ST7789_ROW_OFFSET + y, x + width - 1, y + height - 1);
  nrf_gpio_pin_set(pinDataCommand);
}

void St7789::NextDrawBuffer(const uint8_t *data, size_t size) {
  WriteSpi(data, size);
}

void St7789::HardwareReset() {
  nrf_gpio_pin_clear(26);
  nrf_delay_ms(10);
  nrf_gpio_pin_set(26);
}

void St7789::Sleep() {
  SleepIn();
  nrf_gpio_cfg_default(pinDataCommand);
  //  spi.Sleep(); // TODO sleep SPI
}

void St7789::Wakeup() {
  //  spi.Wakeup(); // TODO wake up SPI

  nrf_gpio_cfg_output(pinDataCommand);
  // TODO why do we need to reset the controller?
  HardwareReset();
  SoftwareReset();
  SleepOut();
  ColMod();
  MemoryDataAccessControl();
  ColumnAddressSet();
  RowAddressSet();
  DisplayInversionOn();
  NormalModeOn();
  VerticalScrollStartAddress(verticalScrollingStartAddress);
  DisplayOn();
}
