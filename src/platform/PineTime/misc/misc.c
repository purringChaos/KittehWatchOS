#include "SEGGER_RTT.h"
#include <platform/includes/misc.h>
#include <stdio.h>

int _write(int iFileHandle, char *pcBuffer, int iLength)

{

  return SEGGER_RTT_Write(0, pcBuffer, iLength);
}

void platform_initMisc() { printf("H.\n"); }