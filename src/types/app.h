#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H
#include <stdbool.h>

typedef struct Application {
  bool valid;
  const char *(*name)();
  void (*init)();
  void (*refresh)();
  void (*deinit)();
} Application;
#endif