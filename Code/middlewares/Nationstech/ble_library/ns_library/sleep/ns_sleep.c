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
 * @file ns_sleep.c
 * @author Nations Firmware Team
 * @version v1.0.4
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

/** @addtogroup 
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "ns_sleep.h"
#include "ns_ble.h"
#include "app_ble.h"
#include "app_gpio.h"
#include "app_power.h"
#include "bat.h"
/* Private typedef -----------------------------------------------------------*/
enum ns_bb_status_t
{
    BB_ACTIVE = 0,
    BB_SLEEP,
    BB_WAKEUP_OSEN,
    BB_WAKEUP_EXIT,    
    BB_WAKEUP_USER,
    BB_WAKEUP_ERROR,   
//    BB_WAKEUP_RTC, // 低功耗定时器唤醒状态。
};
/* Private define ------------------------------------------------------------*/
#ifndef NS_WAKEUP_CONFIG
#define NS_WAKEUP_CONFIG()   
#endif
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t ns_sleep_lock = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef SLEEP_LP_TIMER_ENABLE
/**
 * @brief  低功耗定时器初始化
 * @param  period_ms: 1 - 0x7fff
 * @param  func: 低功耗定时器回调函数，建议执行时间不超过 2ms
 * @return void
 * @note   
 */
void ns_sleep_lp_timer_config(uint16_t period_ms, IRQ_HANNDLE_FUN func)
{
    /* 使能 RTC 时钟 */
    RCC_EnableRtcClk(ENABLE);
    RTC_WaitForSynchro();
    RTC_EnableWakeUp(DISABLE);
    RTC_ConfigInt(RTC_INT_WUT, DISABLE);  
    /* 设置唤醒回调函数 */
    extern IRQ_HANNDLE_FUN lpt_wakeup_hdl ;
    lpt_wakeup_hdl = func;
    if(lpt_wakeup_hdl == NULL)
    {
        // 禁用低功耗定时器
        return;
    }
    /* 初始化唤醒定时器 */
    if((RTC->INITSTS&RTC_INITSTS_INITSF) == 0)
    {
        RTC_InitType RTC_InitStructure;
        RTC_InitStructure.RTC_AsynchPrediv = 0x7D; // 0x7F:23768, 0x7D:32k
        RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
        RTC_InitStructure.RTC_HourFormat   = RTC_24HOUR_FORMAT;
        RTC_Init(&RTC_InitStructure);
    }
    /* 配置唤醒定时器 */
    RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV16); // 32k/16 = 2kHz
    RTC_SetWakeUpCounter(period_ms*2); 
    
    /* 使能 RTC 唤醒中断 */
    EXTI_InitType EXTI_InitStructure;
    EXTI_ClrITPendBit(EXTI_LINE9);
    EXTI_InitStructure.EXTI_Line                = EXTI_LINE9;
    EXTI_InitStructure.EXTI_Mode                = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger             = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd             = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel          = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority  = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd       = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_SetPriority(EXTI4_12_IRQn, 3); 

    
    RTC_ConfigInt(RTC_INT_WUT, ENABLE);
    /* Enable wakeup timer*/
    RTC_EnableWakeUp(ENABLE);
}

/**
 * @brief  在唤醒回调中激活 rwip(bb)
 * @param  
 * @return void
 * @note   
 */
void ns_sleep_lp_timer_wake_ble(void)
{
    g_sleep_status_flag = BB_WAKEUP_USER;
}
#endif

/**
 * @brief  获取睡眠锁，阻止系统进入睡眠
 *         当条件满足后，需要调用 ns_sleep_lock_release 释放睡眠锁
 * @param  
 * @return 
 * @note   
 */
uint8_t ns_sleep_lock_acquire(void)
{
    if(ns_sleep_lock++ == 0)
    {
        // 计数溢出
        return false;
    }
    return true;
}

/**
 * @brief  释放睡眠锁；当所有睡眠锁都释放后，系统可在空闲时进入睡眠
 * @param  
 * @return 
 * @note   
 */
uint8_t ns_sleep_lock_release(void)
{
    if(ns_sleep_lock)
    {
        ns_sleep_lock--;
        return true;
    }
    return false;
}


/**
 * @brief  进入深度睡眠流程
 * @param  
 * @return 
 * @note   
 */
void entry_sleep(void)
{
    uint32_t reg_rt_0 = REG32(0x40011004);
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR; // 使能 PWR 模块
    REG32(0x40028030) |= 0x07;
    REG32(0x40011004)  = 0x00; 
    EXTI_PA11_Configuration();  
    g_sleep_status_flag = BB_SLEEP;
    while(PWR->CR1&0x20); // 等待 BLE 进入睡眠
    while(1)
    {
        __set_PRIMASK(1);// 关闭中断
        volatile uint8_t *p_flag = &g_sleep_status_flag;
        if( (*p_flag) != BB_SLEEP )
        {
            __set_PRIMASK(0);// 打开中断
            // 唤醒 rwip
            break;
        }
        NS_LOG_DEINIT();  
        
        PWR->CR1  = 0x0A;
        SCB->SCR |= 0x04;
        __WFI(); 
        
        RCC->LSCTRL |= 1;   // 重新写入 LSCTRL
        RCC->CFG |=  RCC_HCLK_DIV2; // APB1 = HCLK/2，APB1 最大 32MHz
        RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR; // 使能 PWR 模块
        RCC->APB2PCLKEN |= RCC_APB2_PERIPH_GPIOA|RCC_APB2_PERIPH_GPIOB|RCC_APB2_PERIPH_AFIO; 
        REG32(0x40011004) = reg_rt_0; 
        NS_WAKEUP_CONFIG();
        NS_LOG_INIT();
        
        #ifdef SLEEP_LP_TIMER_ENABLE 
        if( (EXTI->PEND)&(~EXTI_LINE9) )
        {
            // RTC 额外唤醒
            g_sleep_status_flag = BB_WAKEUP_EXIT;
        }
        __set_PRIMASK(0);// 打开中断
        // 等待 IRQ 处理结束
        uint8_t exit_wait = 0xff;
        while((EXTI->PEND&EXTI->IMASK) != 0)
        {
            exit_wait--;
            if(exit_wait == 0)
            {
                // 超时异常，强制唤醒 BLE
                g_sleep_status_flag = BB_WAKEUP_ERROR;
                break;
            }
        }
        #else
        // 任意情况都唤醒 rwip
        break;
        #endif
    }
    // 通过唤醒请求拉起 BLE
    PWR->CR2 |= 0x100; // bit8: 唤醒 BLE
    while(!(PWR->CR1&0x20));// 等待 BLE 恢复活动
}

/**
 * @brief  进入 Idle 模式
 * @param  
 * @return 
 * @note   
 */
void entry_idle(void)
{
    EXTI_PA11_Configuration(); 
    PWR->CR1 &= 0xF0;
    SCB->SCR &= 0xFB;
    __WFI();
}

/**
 * @brief  睡眠任务函数，通常在主循环 rwip_schedule 之后调用
 * @return 
 * @note   
 */
void ns_sleep(void)
{
    /* 参考 NQ25015A：优先根据硬件电平判断是否允许睡眠，避免软件状态滞后。 */
    /* DCIN 为低表示外部电源接入，此时禁止进入低功耗。 */
    if((ns_sleep_lock != 0) || app_power_is_dcin_connected())
    {
        return;
    }

    /* 睡前准备阶段可能会下发 BLE 命令并跑调度，这里保持中断开启。 */
    app_sleep_prepare_proc();

    if((ns_sleep_lock != 0) || app_power_is_dcin_connected())
    {
        return;
    }

    /* 断电待机场景优先走原生深睡流程，确保 BLE 和日志按既有路径收尾。 */
    if(power.sleepAllow)
    {
        GLOBAL_INT_DISABLE();
        entry_sleep();
        GLOBAL_INT_RESTORE();
        app_sleep_resume_proc();
        return;
    }

    GLOBAL_INT_DISABLE();
		

    switch(rwip_sleep())
    {
        case RWIP_DEEP_SLEEP:
        {
            entry_sleep();
        }
        break;
        case RWIP_CPU_SLEEP:
        {
            entry_idle();
        }
        break;
        case RWIP_ACTIVE:
        default:
        {

        }
        break;
    }
    GLOBAL_INT_RESTORE();
    
    volatile uint16_t *p_prevent_sleep = &rwip_env.prevent_sleep;
    /* 检查 BLE 是否已退出睡眠 */
    if(((*p_prevent_sleep) & (RW_WAKE_UP_ONGOING|RW_DEEP_SLEEP) )) 
    {
        uint32_t wait_sleep = 12800; // 2*800us 超时保护
        while((*p_prevent_sleep) & (RW_WAKE_UP_ONGOING|RW_DEEP_SLEEP))
        {
            wait_sleep--;
            if(wait_sleep == 0)
            {
                break;
            }
        }
        rwip_time_t current_time = rwip_time_get();
        // 检查 1ms 定时器是否仍在运行，并确保 ke timer 事件被拉起`r`n        if(rwip_env.timer_1ms_target.hs != RWIP_INVALID_TARGET_TIME)
        {
            int32_t duration = CLK_DIFF(current_time.hs, rwip_env.timer_1ms_target.hs);
            if(duration < 0 )
            {
                // 标记 1ms 定时器到期`r`n                ke_event_set(KE_EVENT_KE_TIMER);
            }
        }
    }
    
		
    app_sleep_resume_proc(); 
		


}

/**
 * @brief  用户进入睡眠前的钩子函数
 * @param  
 * @return 
 * @note   
 */
__weak void app_sleep_prepare_proc(void)
{
}

/**
 * @brief  用户退出睡眠后的钩子函数`r`n *         若睡眠期间有中断挂起，本函数会在对应中断处理后执行
 * @param  
 * @return 
 * @note   
 */
__weak void app_sleep_resume_proc(void)
{
    
}

/**
 * @}
 */



