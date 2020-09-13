#include "platform/includes/time.h"
#include <time.h>

u16 time_year() {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return 1900 + timeinfo->tm_year;
};
u8 time_month() {
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return timeinfo->tm_mon + 1;
};
u8 time_day() {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return timeinfo->tm_mday;
};
u8 time_dayOfWeek() {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return timeinfo->tm_wday;
};
u8 time_hour() {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return timeinfo->tm_hour;
};
u8 time_minute() {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return timeinfo->tm_min;
};
u8 time_second() {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return timeinfo->tm_sec;
};
