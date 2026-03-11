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
 * @file app_power.c
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "main.h"
#include "app_ble.h"
#include "app_power.h"

/* DCIN 多次采样滤波参数。 */
#define DCIN_FILTER_SAMPLES      5
#define DCIN_FILTER_THRESHOLD    4

static bool app_dcin_is_connected_raw(void);
static void app_gpio_sleep_lowpower_cfg(void);
static void app_gpio_sleep_restore_cfg(void);
static void app_dcin_wakeup_enable(void);
static void app_dcin_wakeup_disable(void);
static void app_display_resume_restore(void);
static volatile bool app_dcin_resume_pending = false;

bool app_power_is_dcin_connected(void)
{
    /* 连续采样后多数表决，避免单次误读。 */
    uint8_t connected_votes = 0;
    uint8_t sample = 0;

    for (sample = 0; sample < DCIN_FILTER_SAMPLES; ++sample)
    {
        if (app_dcin_is_connected_raw())
        {
            ++connected_votes;
        }
    }

    return (connected_votes >= DCIN_FILTER_THRESHOLD);
}

void app_power_handle_dcin_irq(void)
{
    if (app_dcin_is_connected_raw())
    {
        app_dcin_resume_pending = true;
    }
}

void app_power_process_pending(void)
{
    if (!app_dcin_resume_pending)
    {
        return;
    }

    app_dcin_resume_pending = false;

    if (power.sleepAllow)
    {
        app_sleep_resume_proc();
        return;
    }

    if (app_power_is_dcin_connected())
    {
        if (!app_ble_is_initialized())
        {
            app_ble_init();
        }
        app_gpio_sleep_restore_cfg();
        app_display_resume_restore();
        power.status = POWER_ON;
    }
}

void app_sleep_prepare_proc(void)
{
    /* 未插外部电源且当前不在睡眠态时，进入低功耗流程。 */
    if (power.sleepAllow || app_power_is_dcin_connected())
    {
        return;
    }

    ke_timer_clear(APP_5MS_EVT, TASK_APP);
    ke_timer_clear(APP_10MS_EVT, TASK_APP);
    ke_timer_clear(APP_20MS_EVT, TASK_APP);
    ke_timer_clear(APP_100MS_EVT, TASK_APP);
    ke_timer_clear(APP_500MS_EVT, TASK_APP);
    ke_timer_clear(APP_1S_EVT, TASK_APP);
    aroma.en = OFF;
    airpump.SW = 0;
    fan.SW = OFF;
    MOTOR_DISABLE();
    FAN_OFF();
    LCD_BACKLIGHT_OFF();
    LedSleepConfig();
    SleepLcd_Config();
    app_gpio_sleep_lowpower_cfg();
    Buzzer_Off();
    TIM_Enable(TIM1, DISABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_TIM1, DISABLE);

    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;
    TIM_Enable(TIM3, DISABLE);

    if (app_ble_is_initialized())
    {
        ns_ble_disconnect();
        for (uint8_t index = 0; index < 80; index++)
        {
            rwip_schedule();
        }

        ns_ble_adv_stop();
        for (uint8_t index = 0; index < 80; index++)
        {
            rwip_schedule();
        }
    }

    /* 睡眠期间保留 DCIN 作为唤醒源。 */
    app_dcin_wakeup_enable();
    power.sleepAllow = 1;
}

void app_sleep_resume_proc(void)
{
    bool ble_was_initialized;

    /* 只有在睡眠态下才允许执行恢复流程。 */
    if (!power.sleepAllow)
    {
        return;
    }

    if (!app_power_is_dcin_connected())
    {
        app_dcin_wakeup_enable();
        power.sleepAllow = 1;
        aroma.en = OFF;
        return;
    }

    app_dcin_wakeup_disable();
    app_gpio_sleep_restore_cfg();
    app_display_resume_restore();
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_TIM1, ENABLE);
    Buzzer_Init();

    ble_was_initialized = app_ble_is_initialized();
    if (!ble_was_initialized)
    {
        app_ble_init();
    }
    else
    {
        /* 恢复系统节拍和应用周期定时器。 */
        ke_timer_set(APP_5MS_EVT, TASK_APP, 5);
        ke_timer_set(APP_10MS_EVT, TASK_APP, 10);
        ke_timer_set(APP_20MS_EVT, TASK_APP, 20);
        ke_timer_set(APP_100MS_EVT, TASK_APP, 100);
        ke_timer_set(APP_500MS_EVT, TASK_APP, 500);
        ke_timer_set(APP_1S_EVT, TASK_APP, 1000);
    }

    SysTick_Config(SystemCoreClock / SYSTICK_100US);
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    TIM_Enable(TIM3, ENABLE);

    globalWorkState = FULL_WORKING;
    power.status = POWER_ON;
    aroma.en = ON;
    power.sleepAllow = 0;
    power.offTime = 0;

    /* 只有睡前已启动过协议栈时，恢复后才重新开广播。 */
    if (ble_was_initialized)
    {
        ns_ble_adv_start();
        for (uint8_t index = 0; index < 10; index++)
        {
            rwip_schedule();
        }
    }
}

static void app_dcin_wakeup_enable(void)
{
    DcinInputExtiInit(ENABLE);
    EXTI->RT_CFG &= ~DCIN_EXTI_LINE;
    EXTI->FT_CFG |= DCIN_EXTI_LINE;
}

static void app_dcin_wakeup_disable(void)
{
    DcinInputExtiInit(DISABLE);
}

static bool app_dcin_is_connected_raw(void)
{
    return (DCIN_READ() == RESET);
}

static void app_gpio_sleep_lowpower_cfg(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;

    GPIO_InitStructure.Pin = LCD_CS_PIN | LCD_WR_PIN | LCD_DATD_PIN | LCD_BACKLIGHT_PIN | OIL_DETECT_PIN;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED_RED_PIN | LED_BLUE_PIN |
                             MOTOR_DC_MT_PIN | MOTOR_IN1_PIN | MOTOR_IN2_PIN |
                             FAN_CTRL_PIN | BUZZER_PIN |
                             KEY_MODE_PIN | KEY_SET_PIN | KEY_UP_PIN | KEY_DOWN_PIN;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

static void app_gpio_sleep_restore_cfg(void)
{
    All_GPIO_Init();
}

static void app_display_resume_restore(void)
{
    /* SleepLcd_Config() 会关闭 VK1621B，恢复时必须主动重新初始化，
     * 不能只等 LCD 周期任务后续慢慢拉起。 */
    vk1621_init();
    lcd.status = true;
    lcd.backOnTime = 0;
    LCD_BACKLIGHT_ON();
    Write_1621_RAM(0, DplyData.Buffer, 16);
}
