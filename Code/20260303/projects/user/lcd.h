#ifndef _LCD_H_
#define _LCD_H_

#include "main.h"

typedef struct 
{
	bool status;
	uint16_t backOnTime;
	uint8_t weekdaySWCycle;
	bool displaySWFlg;
	bool bleStatus;
}lcd_t;
extern lcd_t lcd;

void Lcd_Display_Check(void);
void Lcd_Task(void);
void Display_Task(void);


#endif

