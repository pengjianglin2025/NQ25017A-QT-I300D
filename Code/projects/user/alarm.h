/* alarm.h */

#ifndef __ALARM_H__
#define __ALARM_H__

#include "main.h"
#include "n32wb03x_tim.h"

typedef enum
{
    BUZZER_MODE_NONE = 0,
    BUZZER_MODE_KEY,
    BUZZER_MODE_RESET
} BuzzerMode_t;

extern volatile uint8_t alarmMode;   // 全局触发变量

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);

/* 10ms调用一次，不带形参 */
void Buzzer_Task10ms(void);

#endif
