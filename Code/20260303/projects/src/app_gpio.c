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
 * @brief  Configures key port.
 * @param GPIOx x can be A to G to select the GPIO port.
 * @param Pin This parameter can be GPIO_PIN_0~GPIO_PIN_15.
 */
void KeyInputExtiInit(GPIO_Module* GPIOx, uint16_t Pin, FunctionalState Cmd)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* Enable the GPIO Clock */
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

    /*Configure the GPIO pin as input floating*/
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin          = Pin;
        GPIO_InitStructure.GPIO_Pull    = GPIO_PULL_UP;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }

    /*Configure key EXTI Line to key input Pin*/
    GPIO_ConfigEXTILine(KEY_INPUT_PORT_SOURCE, KEY_INPUT_PIN_SOURCE);

    /*Configure key EXTI line*/
    EXTI_InitStructure.EXTI_Line    = KEY_INPUT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = Cmd;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /*Set key input interrupt priority*/
    NVIC_InitStructure.NVIC_IRQChannel                   = KEY_INPUT_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority           = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = Cmd;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief LCD相关GPIO初始化
 * 连接VK1621B LCD驱动芯片
 * CS - 片选
 * WR - 写信号
 * DATD - 数据
 */
void LCD_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOA时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    
    /* CS、WR、DATD引脚配置为推挽输出 */
    GPIO_InitStructure.Pin = LCD_CS_PIN | LCD_WR_PIN | LCD_DATD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(LCD_CS_GPIO, &GPIO_InitStructure);
    
    /* 设置初始电平 */
    LCD_CS_HIGH();      // CS默认高电平
    LCD_WR_HIGH();      // WR默认高电平
    LCD_DATD_LOW();     // DATD默认低电平
}

/**
 * @brief 触摸按键GPIO初始化
 * AI05触摸芯片控制4个按键：MODE、SET、UP、DOWN
 */
void TouchKey_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOA时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    
    /* 配置按键引脚为上拉输入 */
    GPIO_InitStructure.Pin = KEY_MODE_PIN | KEY_SET_PIN | KEY_UP_PIN | KEY_DOWN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
//    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(KEY_MODE_GPIO, &GPIO_InitStructure);
}

/**
 * @brief LED指示灯GPIO初始化
 * LEDSW: LCD背光控制(控制LED1-5白色LED) - 高电平开启
 * LED3: 红色LED - 低电平点亮
 * LED6: 蓝色LED - 低电平点亮
 */
void LED_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOB时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    
    /* 配置LED引脚为推挽输出 */
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pin = LCD_BACKLIGHT_PIN | LED_RED_PIN | LED_BLUE_PIN;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
    
    /* 默认关闭所有LED */
    LCD_BACKLIGHT_OFF();  // 背光：低电平关闭
    LED_RED_OFF();        // 红灯：高电平熄灭
    LED_BLUE_OFF();       // 蓝灯：高电平熄灭
}

/**
 * @brief 电机控制GPIO初始化
 * 使用RZ7889驱动芯片控制MT1电机
 * DC-MT: 电机使能/PWM控制
 * IN1、IN2: 方向控制信号
 */
void Motor_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOB时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    
    /* DC-MT、IN1、IN2配置为推挽输出 */
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pin = MOTOR_DC_MT_PIN | MOTOR_IN1_PIN | MOTOR_IN2_PIN;
    GPIO_InitPeripheral(MOTOR_DC_MT_GPIO, &GPIO_InitStructure);
    
    /* 默认停止电机 */
    MOTOR_DISABLE();
    MOTOR_IN1_LOW();
    MOTOR_IN2_LOW();
}

/**
 * @brief 风扇控制GPIO初始化
 * 使用Q4(3400 MOSFET)控制风扇
 */
void Fan_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOB时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    
    /* FAN控制引脚 */
    GPIO_InitStructure.Pin = FAN_CTRL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(FAN_CTRL_GPIO, &GPIO_InitStructure);
    
    /* 默认关闭风扇 */
    FAN_OFF();
}

/**
 * @brief 蜂鸣器控制GPIO初始化
 * 使用Q5(SS8050)控制蜂鸣器BUZ1(HY9055)
 */
void Buzzer_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOB时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    
    /* BUZ控制引脚 - PB1配置为复用推挽输出（TIM1_CH2） */
    GPIO_InitStructure.Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;  // 复用推挽输出
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(BUZZER_GPIO, &GPIO_InitStructure);
    
    /* 默认关闭蜂鸣器 - 通过TIM初始化完成 */
}

/**
 * @brief 精油检测GPIO初始化
 * DET引脚用于检测精油存在
 */
void OilDetect_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOB时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    
    /* DET引脚配置为输入 */
    GPIO_InitStructure.Pin = OIL_DETECT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(OIL_DETECT_GPIO, &GPIO_InitStructure);
}

/**
 * @brief 电源相关GPIO初始化
 * DCIN: 12V电源输入
 * 3V6: 3.6V电源
 */
void Dcin_GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    /* 使能GPIOB时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    
    /* 如果有电源使能控制引脚 */
    GPIO_InitStructure.Pin = DC_IN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_LOW;
    GPIO_InitPeripheral(DC_IN_GPIO, &GPIO_InitStructure);
    
    /* 使能电源 */
    GPIO_SetBits(DC_IN_GPIO, DC_IN_PIN);
}

/**
 * @brief 调试接口GPIO初始化
 * SWDATA、SWCLK用于SWD调试
 */
void Debug_GPIO_Init(void)
{
    /* PA4(SWDCLK)和PA5(SWDIO)默认已配置为调试功能
     * 通常不需要额外初始化
     * 如果需要释放这些引脚作为普通GPIO，需要禁用调试功能
     */
}

/**
 * @brief 所有GPIO初始化总函数
 */
void All_GPIO_Init(void)
{
    /* 使能所有GPIO时钟 */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | 
                           RCC_APB2_PERIPH_AFIO, ENABLE);
    
    /* 初始化各功能模块GPIO */
    LCD_GPIO_Init();        // LCD显示
    TouchKey_GPIO_Init();   // 触摸按键
    LED_GPIO_Init();        // LED指示灯
    Motor_GPIO_Init();      // 电机控制
    Fan_GPIO_Init();        // 风扇控制
    Buzzer_GPIO_Init();     // 蜂鸣器
    OilDetect_GPIO_Init();  // 精油检测
    Dcin_GPIO_Init();      // 电源控制
}