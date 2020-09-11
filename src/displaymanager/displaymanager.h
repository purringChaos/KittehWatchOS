#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include "lvgl/lvgl.h"

extern void displaymanager_start();
extern void displaymanager_go_back_button_event_handler(lv_obj_t *obj,
                                                        lv_event_t event);
extern void displaymanager_switch_application(const char *name);
#endif