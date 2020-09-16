#ifndef PLATFORM_THREAD_H
#define PLATFORM_THREAD_H
#include "includes/common.h"

DisableWarnings
#ifdef __linux__
#include <pthread.h>
#define THREAD_TYPE pthread_t
#else
#include "FreeRTOS.h"
#include <task.h>
#define THREAD_TYPE TaskHandle_t
#endif
    EnableWarnings

    extern void
    platform_initThreading();
extern void platform_sleep(int);
extern void platform_createThread(THREAD_TYPE *thread, int priority,
                                  const char *const name, void (*func)(void *),
                                  void *arg);
extern void platform_startAllThreads();
extern void platform_setThreadName(const char *name);

#endif
