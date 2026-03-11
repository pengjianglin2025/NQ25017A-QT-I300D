
#include "led.h"


Light_t Light;


void Led_Task(void)
{
	if(airpump.SW)
	{
		LED_RED_OFF(); LED_BLUE_ON();
	}
	else
	{
		LED_BLUE_OFF(); LED_RED_ON(); 
	}
}
void LedSleepConfig(void)
{
	LED_BLUE_OFF();
	LED_RED_OFF();
}



