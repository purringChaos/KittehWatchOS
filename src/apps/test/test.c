#include "apps/test/test.h"
#include "displaymanager/displaymanager.h"
#include "lvgl/lvgl.h"
#include "platform/includes/time.h"
#include <stdio.h>
#include <time.h>
#include "apps/test/rick.h"


const char *apps_test_name() { return "Test"; }

void apps_test_init() {
  lv_obj_t *titleBar = lv_obj_get_child(lv_layer_top(), NULL);
    lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &rick);
    lv_obj_align(img1, titleBar, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

void apps_test_refresh() {}
void apps_test_deinit() {
  lv_obj_clean(lv_scr_act());
}