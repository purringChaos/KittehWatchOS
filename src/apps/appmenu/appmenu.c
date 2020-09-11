#include "apps/appmenu/appmenu.h"
#include "displaymanager/displaymanager.h"

#include "lvgl/lvgl.h"

const char *apps_appmenu_name() { return "AppMenu"; }

lv_obj_t *buttonMatrix;

static void appmenu_on_click_app(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    const char *txt = lv_btnmatrix_get_active_btn_text(obj);
    displaymanager_switch_application(txt);
    printf("%s was pressed\n", txt);
  }
}

static const char *appsList[] = {"AppMenu", "\n", "Clock", "\n", "Test", ""};

void apps_appmenu_init() {
  printf("Starting AppMenu!\n");

  lv_obj_t *titleBar = lv_obj_get_child(lv_layer_top(), NULL);

  buttonMatrix = lv_btnmatrix_create(lv_scr_act(), NULL);
  lv_obj_align(buttonMatrix, titleBar, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
  lv_btnmatrix_set_map(buttonMatrix, appsList);
  lv_btnmatrix_set_btn_ctrl_all(buttonMatrix, LV_BTNMATRIX_CTRL_NO_REPEAT);

  lv_obj_set_event_cb(buttonMatrix, appmenu_on_click_app);
}

void apps_appmenu_refresh() {}
void apps_appmenu_deinit() { lv_obj_clean(lv_scr_act()); }