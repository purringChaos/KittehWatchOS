#pragma once
#include "types/numbers.h"
#include <lvgl/lvgl.h>
#include <stddef.h>
void St7789_Init();
void St7789_Uninit();
void St7789_WriteCommand(u8 cmd);
void St7789_WriteSpi(const u8 *data, size_t size);
void St7789_WriteData(u8 *data, size_t size);
void St7789_WriteSingleData(u8 data);
void St7789_Flush(lv_disp_drv_t *driver, const lv_area_t *area,
                  lv_color_t *colour_data);