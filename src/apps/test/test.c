#include "apps/test/test.h"
#include "displaymanager/displaymanager.h"
#include "lvgl/lvgl.h"
#include "platform/includes/time.h"
#include <stdio.h>
#include <time.h>

#include "gears.h"
#include "gl.h"
#include "zbuffer.h"

#define CANVAS_WIDTH 50
#define CANVAS_HEIGHT 50

static void anim(lv_task_t *t);
static lv_obj_t *canvas;
static lv_color_t
    cbuf[LV_IMG_BUF_SIZE_TRUE_COLOR_ALPHA(CANVAS_WIDTH, CANVAS_HEIGHT)];

const char *apps_test_name() { return "Test"; }

void apps_test_init() {
  lv_obj_t *titleBar = lv_obj_get_child(lv_layer_top(), NULL);

  canvas = lv_canvas_create(lv_scr_act(), NULL);
  lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT,
                       LV_IMG_CF_TRUE_COLOR);
  lv_obj_align(canvas, titleBar, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  gears_init(CANVAS_WIDTH, CANVAS_HEIGHT, 1, cbuf);

  lv_task_create(anim, 30, LV_TASK_PRIO_LOW, NULL);
}

static void anim(lv_task_t *t) {
  gears_update();
  lv_obj_invalidate(canvas);
}

void apps_test_refresh() {}
void apps_test_deinit() { lv_obj_clean(lv_scr_act()); }
