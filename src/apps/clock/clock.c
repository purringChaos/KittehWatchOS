#include "apps/clock/clock.h"
#include "displaymanager/displaymanager.h"
#include "lvgl/lvgl.h"
#include "platform/includes/time.h"
#include <stdio.h>
#include <time.h>

const char *apps_clock_name() { return "Clock"; }

lv_obj_t *clockTitleLabel;

lv_task_t *clockTask;

char *ordinal(u8 n) {
  switch (n % 100) {
  case 11:
  case 12:
  case 13:
    return "th";
  default:
    switch (n % 10) {
    case 1:
      return "st";
    case 2:
      return "nd";
    case 3:
      return "rd";
    default:
      return "th";
    }
  }
}
static inline const char *dayOfWeekToString(u8 dayOfWeek) {
  static const char *strings[] = {"Unknown",  "Monday", "Tuesday",  "Wednesday",
                                  "Thursday", "Friday", "Saturday", "Sunday"};

  return strings[dayOfWeek];
}
static inline const char *monthToString(u8 month) {
  static const char *strings[] = {"Unknown", "January",   "Febuary", "March",
                                  "April",   "May",       "June",    "July",
                                  "August",  "September", "October", "November",
                                  "December"};

  return strings[month];
}

static void update_clock_task(lv_task_t *param) {
  char buffer[512];
  //lv_label_set_recolor(clockTitleLabel, true);
  lv_label_set_align(clockTitleLabel, LV_LABEL_ALIGN_CENTER);
  u16 year = time_year();
  u8 month = time_month();
  u8 day = time_day();
  u8 dayOfWeek = time_dayOfWeek();
  u8 hour = time_hour();
  u8 minute = time_minute();
  u8 second = time_second();

  char *ending = hour >= 12 ? "pm" : "am";
  if (hour >= 13) {
    hour = hour - 12;
  }
  const char *dayOfWeekString = dayOfWeekToString(dayOfWeek);
  const char *monthString = monthToString(month);

  snprintf(buffer, 512,
           "#A3BE8C %s#\n#B48EAD the#\n#EBCB8B %d##81A1C1 %s#\n#B48EAD "
           "of#\n#BF616A %s#\n#B48EAD in#\n#88C0D0 %d#\n#B48EAD at#\n#BF616A "
           "%02d##88C0D0 :##D08770 %02d##81A1C1 :##EBCB8B %02d##5E81AC %s#",
           dayOfWeekString, day, ordinal(day), monthString, year, hour, minute,
           second, ending);
  lv_label_set_text(clockTitleLabel, buffer);
}

void apps_clock_init() {
  lv_obj_t *titleBar = lv_obj_get_child(lv_layer_top(), NULL);

  clockTitleLabel = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(clockTitleLabel, "Clock");
  lv_obj_set_auto_realign(clockTitleLabel, true);
  lv_obj_align(clockTitleLabel, titleBar, LV_ALIGN_OUT_BOTTOM_MID, 0, 00);

  clockTask = lv_task_create(update_clock_task, 200, LV_TASK_PRIO_MID, NULL);
}

void apps_clock_refresh() {}
void apps_clock_deinit() {
  lv_task_del(clockTask);
  lv_obj_clean(lv_scr_act());
}