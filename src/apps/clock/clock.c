#include "apps/clock/clock.h"
#include "displaymanager/displaymanager.h"

#include "lvgl/lvgl.h"

const char *apps_clock_name() { return "Clock"; }

lv_obj_t *clockTitleLabel;

void apps_clock_init() {
  lv_obj_t *titleBar = lv_obj_get_child(lv_layer_top(), NULL);

  clockTitleLabel = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(clockTitleLabel, "Clock");
  lv_obj_set_auto_realign(clockTitleLabel, true);
  lv_obj_align(clockTitleLabel, child, LV_ALIGN_OUT_BOTTOM_MID, 0, 00);
}

void apps_clock_refresh() {}
void apps_clock_deinit() { lv_obj_clean(lv_scr_act()); }