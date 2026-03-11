/****************************************************************************
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
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file app_gpio.c
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "app_gpio.h"

/** @addtogroup
 * @{
 */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  配置按键中断输入口。
 * @param GPIOx 可选 GPIOA 到 GPIOG。
 * @param Pin   可选 GPIO_PIN_0 到 GPIO_PIN_15。
 */
void KeyInputExtiInit(GPIO_Module* GPIOx, uint16_t Pin, FunctionalState Cmd)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    if (GPIOx == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else
    {
        return;
    }

    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin       = Pin;
        GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }

    GPIO_ConfigEXTILine(KEY_INPUT_PORT_SOURCE, KEY_INPUT_PIN_SOURCE);

    EXTI_InitStructure.EXTI_Line    = KEY_INPUT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = Cmd;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel         = KEY_INPUT_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = Cmd;
    NVIC_Init(&NVIC_InitStructure);
}

void DcinInputExtiInit(FunctionalState Cmd)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin       = DC_IN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(DC_IN_GPIO, &GPIO_InitStructure);

    GPIO_ConfigEXTILine(DCIN_PORT_SOURCE, DCIN_PIN_SOURCE);
    EXTI_ClrITPendBit(DCIN_EXTI_LINE);

    EXTI_InitStructure.EXTI_Line    = DCIN_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = Cmd;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel         = DCIN_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = Cmd;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief LCD GPIO 初始化
 * 连接 VK1621B LCD 驱动芯片
 * CS   - 片选
 * WR   - 写时钟
 * DATD - 数据
 */
void LCD_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStructure.Pin        = LCD_CS_PIN | LCD_WR_PIN | LCD_DATD_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(LCD_CS_GPIO, &GPIO_InitStructure);

    LCD_CS_HIGH();
    LCD_WR_HIGH();
    LCD_DATD_LOW();
}

/**
 * @brief 触摸按键 GPIO 初始化
 * AI05 触摸芯片连接 4 个按键：MODE、SET、UP、DOWN
 */
void TouchKey_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.Pin       = KEY_MODE_PIN | KEY_SET_PIN | KEY_UP_PIN | KEY_DOWN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(KEY_MODE_GPIO, &GPIO_InitStructure);
}

/**
 * @brief LED 指示灯 GPIO 初始化
 * LEDSW: LCD 背光控制（控制 LED1-5 白光）- 高电平开启
 * LED3  : 红色 LED - 低电平点亮
 * LED6  : 蓝色 LED - 低电平点亮
 */
void LED_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pin        = LCD_BACKLIGHT_PIN;
    GPIO_InitPeripheral(LCD_BACKLIGHT_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED_RED_PIN | LED_BLUE_PIN;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    LCD_BACKLIGHT_OFF();
    LED_RED_OFF();
    LED_BLUE_OFF();
}

/**
 * @brief 电机控制 GPIO 初始化
 * 使用 RZ7889 驱动芯片控制 MT1 电机
 * DC-MT: 电机使能/PWM 控制
 * IN1、IN2: 方向控制信号
 */
void Motor_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin        = MOTOR_DC_MT_PIN | MOTOR_IN1_PIN | MOTOR_IN2_PIN;
    GPIO_InitPeripheral(MOTOR_DC_MT_GPIO, &GPIO_InitStructure);

    MOTOR_DISABLE();
    MOTOR_IN1_LOW();
    MOTOR_IN2_LOW();
}

/**
 * @brief 风扇控制 GPIO 初始化
 * 使用 Q4(3400 MOSFET) 控制风扇
 */
void Fan_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.Pin        = FAN_CTRL_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(FAN_CTRL_GPIO, &GPIO_InitStructure);

    FAN_OFF();
}

/**
 * @brief 蜂鸣器 GPIO 初始化
 * 使用 Q5(SS8050) 控制蜂鸣器 BUZ1(HY9055)
 */
void Buzzer_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.Pin        = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(BUZZER_GPIO, &GPIO_InitStructure);

    /* 默认关闭蜂鸣器，后续由 TIM 初始化控制。 */
}

/**
 * @brief 精油检测 GPIO 初始化
 * DET 引脚用于检测精油盒状态
 */
void OilDetect_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.Pin       = OIL_DETECT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(OIL_DETECT_GPIO, &GPIO_InitStructure);
}

/**
 * @brief 外部电源检测 GPIO 初始化
 * DCIN: 外部 12V 电源输入检测
 */
void Dcin_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.Pin       = DC_IN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(DC_IN_GPIO, &GPIO_InitStructure);
}

/**
 * @brief 调试接口 GPIO 初始化
 * SWDIO 和 SWCLK 用于 SWD 调试
 */
void Debug_GPIO_Init(void)
{
    /* PA4(SWDCLK) 和 PA5(SWDIO) 默认复用为调试功能。
     * 通常不需要在这里额外初始化。
     * 如果要释放这些引脚作为普通 GPIO，需要关闭调试功能。
     */
}

/**
 * @brief 所有 GPIO 初始化总入口
 */
void All_GPIO_Init(void)
{
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB |
                            RCC_APB2_PERIPH_AFIO, ENABLE);

    LCD_GPIO_Init();
    TouchKey_GPIO_Init();
    LED_GPIO_Init();
    Motor_GPIO_Init();
    Fan_GPIO_Init();
    Buzzer_GPIO_Init();
    OilDetect_GPIO_Init();
    Dcin_GPIO_Init();
}
