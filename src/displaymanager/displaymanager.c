#include "displaymanager/displaymanager.h"
#include "apps/appmenu/appmenu.h"
#include "apps/clock/clock.h"
#include "apps/test/test.h"
#include "lvgl/lvgl.h"
#include "platform/includes/display.h"
#include "platform/includes/thread.h"
#include "types/app.h"
#include <stdio.h>

typedef struct {
  unsigned int size;
  Application apps[];
} APPLIST;

static APPLIST displaymanager_apps = {3,
                                      {
                                          {.valid = true,
                                           .name = apps_appmenu_name,
                                           .init = apps_appmenu_init,
                                           .refresh = apps_appmenu_refresh,
                                           .deinit = apps_appmenu_deinit},

                                          {.valid = true,
                                           .name = apps_clock_name,
                                           .init = apps_clock_init,
                                           .refresh = apps_clock_refresh,
                                           .deinit = apps_clock_deinit},
                                          {.valid = true,
                                           .name = apps_test_name,
                                           .init = apps_test_init,
                                           .refresh = apps_test_refresh,
                                           .deinit = apps_test_deinit},
                                      }};

Application current_application = {.valid = false};

void displaymanager_go_back_button_event_handler(lv_obj_t *obj,
                                                 lv_event_t event) {
  if (event == LV_EVENT_CLICKED || event == LV_EVENT_VALUE_CHANGED) {
    displaymanager_switch_application("AppMenu");
  }
}

lv_obj_t *top_bar_container;
lv_obj_t *app_title_label;

void displaymanager_make_top_bar() {

  top_bar_container = lv_cont_create(lv_layer_top(), NULL);
  lv_obj_set_auto_realign(top_bar_container, true);
  lv_obj_align_origo(top_bar_container, lv_layer_top(), LV_ALIGN_OUT_TOP_MID, 0,
                     20);
  lv_cont_set_fit2(top_bar_container, LV_FIT_MAX, LV_FIT_TIGHT);
  lv_cont_set_layout(top_bar_container, LV_LAYOUT_ROW_MID);

  static lv_style_t back_btn_style;

  lv_style_init(&back_btn_style);
  lv_style_set_radius(&back_btn_style, LV_STATE_DEFAULT, 0);
  lv_style_set_pad_top(&back_btn_style, LV_STATE_DEFAULT, 4);
  lv_style_set_pad_bottom(&back_btn_style, LV_STATE_DEFAULT, 4);
  lv_style_set_pad_left(&back_btn_style, LV_STATE_DEFAULT, 24);
  lv_style_set_pad_right(&back_btn_style, LV_STATE_DEFAULT, 24);

  lv_obj_t *backButton = lv_btn_create(top_bar_container, NULL);

  lv_obj_add_style(backButton, LV_BTN_PART_MAIN, &back_btn_style);

  lv_btn_set_fit2(backButton, LV_FIT_TIGHT, LV_FIT_TIGHT);

  lv_obj_set_event_cb(backButton, displaymanager_go_back_button_event_handler);
  lv_obj_t *label = lv_label_create(backButton, NULL);

  lv_label_set_text(label, LV_SYMBOL_CLOSE);

  lv_btn_set_checkable(backButton, false);

  app_title_label = lv_label_create(top_bar_container, NULL);
  lv_label_set_text(app_title_label, "Current Application");
}

void displaymanager_switch_application(const char *name) {
  if (!current_application.valid) {
    // printf("Current App Invalid.\n");
    current_application = displaymanager_apps.apps[0];
  }
  // printf("Deinit App.\n");

  current_application.deinit();

  for (unsigned int i = 0; i < displaymanager_apps.size; i++) {
    // printf("Apps: %s\n", displaymanager_apps.apps[i].name());

    if (strcmp(displaymanager_apps.apps[i].name(), name) == 0) {
      // printf("Found it!\n");

      current_application = displaymanager_apps.apps[i];
    }
  }

  lv_label_set_text(app_title_label, current_application.name());
  current_application.init();
}

#ifdef __linux__
THREAD_TYPE lv_ticker_thread_handle;
static void lv_ticker_thread(void *pvParameter) {
  platform_setThreadName("lv_tick\0");
  while (true) {
    lv_tick_inc(1);
    platform_sleep(1);
  }
}
#endif

void displaymanager_start() {
  platform_initDisplay();
#ifdef __linux__
  platform_createThread(&lv_ticker_thread_handle, 1, "lv_tick",
                        lv_ticker_thread, NULL);
#endif

  displaymanager_make_top_bar();

  displaymanager_switch_application("Clock");
  while (true) {
    current_application.refresh();
    lv_task_handler();
    platform_sleep(5);
  }
};
