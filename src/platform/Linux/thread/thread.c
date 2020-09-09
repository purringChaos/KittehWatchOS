#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/prctl.h>
#include <unistd.h>

static volatile bool platform_threading_keepRunning = true;

void platform_signalHandler(int x) { platform_threading_keepRunning = 0; }

void platform_initThreading() {}
void platform_sleep(int t) { usleep(t * 1000); }

void platform_setThreadName(const char *name) {
  prctl(PR_SET_NAME, name, NULL, NULL, NULL);
}

void platform_createThread(pthread_t *thread, const char *name, int priority,
                           void *(*func)(void *), void *arg) {
  pthread_create(thread, NULL, func, arg);
}

void platform_startAllThreads() {
  signal(SIGINT, platform_signalHandler);
  while (platform_threading_keepRunning) {
    platform_sleep(200);
  }
}