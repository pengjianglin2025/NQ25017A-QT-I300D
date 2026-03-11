#ifndef _LED_H_
#define _LED_H_

#include "main.h"

#define Respiratory_Rate  0.015   //ºôÎüËÙ¶È


typedef struct
{
	bool en;
	uint8_t mode;
	uint8_t modeOld;
	uint8_t redValue;
	uint8_t greenValue;
	uint8_t blueValue;
}Light_t;
extern Light_t Light;

void Led_Init(void);
void Led_Task(void);
void LedSleepConfig(void);


#endif

