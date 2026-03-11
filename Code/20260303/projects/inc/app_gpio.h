/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAYS OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file app_gpio.h
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#ifndef __APP_GPIO_H__
#define __APP_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32wb03x.h"
    
#define LED1_PORT GPIOB
#define LED1_PIN  GPIO_PIN_0
#define LED2_PORT GPIOA
#define LED2_PIN  GPIO_PIN_5


#define KEY_INPUT_PORT        GPIOB
#define KEY_INPUT_PIN         GPIO_PIN_1
#define KEY_INPUT_EXTI_LINE   EXTI_LINE1
#define KEY_INPUT_PORT_SOURCE GPIOB_PORT_SOURCE
#define KEY_INPUT_PIN_SOURCE  GPIO_PIN_SOURCE1
#define KEY_INPUT_IRQn        EXTI0_1_IRQn

//#define AIRPUMP_OUT_ON   GPIO_SetBits(GPIOB, GPIO_PIN_5)
//#define AIRPUMP_OUT_OFF  GPIO_ResetBits(GPIOB, GPIO_PIN_5)


/* ==================== LCD相关引脚定义 ==================== */
// VK1621B LCD驱动芯片控制引脚
#define LCD_CS_GPIO         GPIOA
#define LCD_CS_PIN          GPIO_PIN_0

#define LCD_WR_GPIO         GPIOA
#define LCD_WR_PIN          GPIO_PIN_1

#define LCD_DATD_GPIO       GPIOA
#define LCD_DATD_PIN        GPIO_PIN_2

// LCD背光控制
#define LCD_BACKLIGHT_GPIO  GPIOA
#define LCD_BACKLIGHT_PIN   GPIO_PIN_6
//#define GPIO_PIN_LEDSW      GPIO_PIN_6  // LEDSW控制白色LED1-5

/* ==================== 触摸按键引脚定义 ==================== */
// AI05触摸芯片连接的按键
#define KEY_MODE_GPIO       GPIOB
#define KEY_MODE_PIN        GPIO_PIN_7

#define KEY_SET_GPIO        GPIOB
#define KEY_SET_PIN         GPIO_PIN_10

#define KEY_UP_GPIO         GPIOB
#define KEY_UP_PIN          GPIO_PIN_12

#define KEY_DOWN_GPIO       GPIOB
#define KEY_DOWN_PIN        GPIO_PIN_11

/* ==================== LED指示灯引脚定义 ==================== */
// 红色LED
#define LED_RED_GPIO        GPIOB
#define LED_RED_PIN         GPIO_PIN_5
//#define GPIO_PIN_LED_R      GPIO_PIN_5

// 蓝色LED
#define LED_BLUE_GPIO       GPIOB
#define LED_BLUE_PIN        GPIO_PIN_6
//#define GPIO_PIN_LED_B      GPIO_PIN_6

/* ==================== 电机控制引脚定义 ==================== */
// RZ7889驱动芯片控制MT1电机
#define MOTOR_DC_MT_GPIO    GPIOB
#define MOTOR_DC_MT_PIN     GPIO_PIN_0
#define GPIO_PIN_DC_MT      GPIO_PIN_0

#define MOTOR_IN1_GPIO      GPIOB
#define MOTOR_IN1_PIN       GPIO_PIN_3
#define GPIO_PIN_IN1        GPIO_PIN_3

#define MOTOR_IN2_GPIO      GPIOB
#define MOTOR_IN2_PIN       GPIO_PIN_4
#define GPIO_PIN_IN2        GPIO_PIN_4

/* ==================== 风扇控制引脚定义 ==================== */
// Q4(3400 MOSFET)控制风扇
#define FAN_CTRL_GPIO       GPIOB
#define FAN_CTRL_PIN        GPIO_PIN_2
#define GPIO_PIN_FAN        GPIO_PIN_2

/* ==================== 蜂鸣器控制引脚定义 ==================== */
// Q5(SS8050)控制蜂鸣器BUZ1(HY9055)
#define BUZZER_GPIO         GPIOB
#define BUZZER_PIN          GPIO_PIN_1
#define GPIO_PIN_BUZ        GPIO_PIN_1

/* ==================== 精油检测引脚定义 ==================== */
// 精油存在检测
#define OIL_DETECT_GPIO     GPIOA
#define OIL_DETECT_PIN      GPIO_PIN_3
#define GPIO_PIN_DET        GPIO_PIN_3

/* ==================== 电源控制引脚定义 ==================== */
// 电源使能控制（如果有）
#define DC_IN_GPIO       GPIOB
#define DC_IN_PIN        GPIO_PIN_11
#define GPIO_PIN_DC_IN     GPIO_PIN_11

/* ==================== 调试接口引脚定义 ==================== */
// SWD调试接口
#define SWDCLK_GPIO         GPIOA
#define SWDCLK_PIN          GPIO_PIN_4  // PA4默认为SWDCLK

#define SWDIO_GPIO          GPIOA
#define SWDIO_PIN           GPIO_PIN_5  // PA5默认为SWDIO

/* ==================== 复用定义简化 ==================== */
// 为了代码兼容性，定义通用的GPIOX宏
// 实际使用时根据具体引脚选择对应的GPIO组

/* ==================== 引脚控制宏定义 ==================== */
// LCD控制
#define LCD_CS_HIGH()       GPIO_SetBits(LCD_CS_GPIO, LCD_CS_PIN)
#define LCD_CS_LOW()        GPIO_ResetBits(LCD_CS_GPIO, LCD_CS_PIN)

#define LCD_WR_HIGH()       GPIO_SetBits(LCD_WR_GPIO, LCD_WR_PIN)
#define LCD_WR_LOW()        GPIO_ResetBits(LCD_WR_GPIO, LCD_WR_PIN)

#define LCD_DATD_HIGH()     GPIO_SetBits(LCD_DATD_GPIO, LCD_DATD_PIN)
#define LCD_DATD_LOW()      GPIO_ResetBits(LCD_DATD_GPIO, LCD_DATD_PIN)

// LCD背光控制 - 高电平开启
#define LCD_BACKLIGHT_ON()  GPIO_SetBits(LCD_BACKLIGHT_GPIO, LCD_BACKLIGHT_PIN)
#define LCD_BACKLIGHT_OFF() GPIO_ResetBits(LCD_BACKLIGHT_GPIO, LCD_BACKLIGHT_PIN)

// LED控制 - 低电平点亮（共阳接法）
#define LED_RED_ON()        GPIO_ResetBits(LED_RED_GPIO, LED_RED_PIN)
#define LED_RED_OFF()       GPIO_SetBits(LED_RED_GPIO, LED_RED_PIN)
#define LED_RED_TOGGLE()    GPIO_TogglePin(LED_RED_GPIO, LED_RED_PIN)

#define LED_BLUE_ON()       GPIO_ResetBits(LED_BLUE_GPIO, LED_BLUE_PIN)
#define LED_BLUE_OFF()      GPIO_SetBits(LED_BLUE_GPIO, LED_BLUE_PIN)
#define LED_BLUE_TOGGLE()   GPIO_TogglePin(LED_BLUE_GPIO, LED_BLUE_PIN)

// 电机控制
#define MOTOR_ENABLE()      GPIO_SetBits(MOTOR_DC_MT_GPIO, MOTOR_DC_MT_PIN)
#define MOTOR_DISABLE()     GPIO_ResetBits(MOTOR_DC_MT_GPIO, MOTOR_DC_MT_PIN)

#define MOTOR_IN1_HIGH()    GPIO_SetBits(MOTOR_IN1_GPIO, MOTOR_IN1_PIN)
#define MOTOR_IN1_LOW()     GPIO_ResetBits(MOTOR_IN1_GPIO, MOTOR_IN1_PIN)

#define MOTOR_IN2_HIGH()    GPIO_SetBits(MOTOR_IN2_GPIO, MOTOR_IN2_PIN)
#define MOTOR_IN2_LOW()     GPIO_ResetBits(MOTOR_IN2_GPIO, MOTOR_IN2_PIN)

// 风扇控制
#define FAN_ON()            GPIO_SetBits(FAN_CTRL_GPIO, FAN_CTRL_PIN)
#define FAN_OFF()           GPIO_ResetBits(FAN_CTRL_GPIO, FAN_CTRL_PIN)

// 蜂鸣器控制
#define BUZZER_ON()         GPIO_SetBits(BUZZER_GPIO, BUZZER_PIN)
#define BUZZER_OFF()        GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN)
#define BUZZER_TOGGLE()     GPIO_TogglePin(BUZZER_GPIO, BUZZER_PIN)

// 按键读取
#define KEY_MODE_READ()     GPIO_ReadInputDataBit(KEY_MODE_GPIO, KEY_MODE_PIN)
#define KEY_SET_READ()      GPIO_ReadInputDataBit(KEY_SET_GPIO, KEY_SET_PIN)
#define KEY_UP_READ()       GPIO_ReadInputDataBit(KEY_UP_GPIO, KEY_UP_PIN)
#define KEY_DOWN_READ()     GPIO_ReadInputDataBit(KEY_DOWN_GPIO, KEY_DOWN_PIN)
// DCIN读取
#define DCIN_READ()     GPIO_ReadInputDataBit(DC_IN_GPIO, DC_IN_PIN)

// 精油检测读取
#define OIL_DETECT_READ()   GPIO_ReadInputDataBit(OIL_DETECT_GPIO, OIL_DETECT_PIN)


void All_GPIO_Init(void);
void KeyInputExtiInit(GPIO_Module* GPIOx, uint16_t Pin, FunctionalState Cmd);

#ifdef __cplusplus
}
#endif

#endif /* __APP_GPIO_H__ */

