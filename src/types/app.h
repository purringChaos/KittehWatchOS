#ifndef TYPES_APP_H
#define TYPES_APP_H
#include <stdbool.h>

typedef struct Application {
  bool valid;
  const char *(*name)();
  void (*init)();
  void (*refresh)();
  void (*deinit)();
} Application;
#endif