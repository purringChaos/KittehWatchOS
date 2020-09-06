#include <stdio.h>
void platform_initBacklight()
{
}

void platform_setBacklight(short level)
{
	switch (level) {
	default:
	case 3:
		printf("Backlight: High\n");
		break;
	case 2:
		printf("Backlight: Medium\n");
		break;
	case 1:
		printf("Backlight: Low\n");
		break;
	case 0:
		printf("Backlight: Off\n");

		break;
	}
}
