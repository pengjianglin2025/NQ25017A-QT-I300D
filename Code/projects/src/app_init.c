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
 * @file app_init.c
 * @author Nations Firmware Team
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "main.h"
#include "app_init.h"
#include "app_power.h"

#if  (CFG_APP_NS_IUS)
#include "ns_dfu_boot.h"
#endif

#define DEMO_STRING  "\r\n Nations raw data transfer server(16bit UUID) demo \r\n"

void app_system_full_init(void)
{
    bool dcin_connected = app_power_is_dcin_connected();

    NS_LOG_INIT();
    All_GPIO_Init();
    power.status = dcin_connected ? POWER_ON : POWER_OFF;
    power.sleepAllow = 0;
    power.offTime = 0;

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / SYSTICK_100US);

    RTC_DateAndTimeDefaultVale();
    RTC_CLKSourceConfig(1);
    RTC_PrescalerConfig();

    RTC_ConfigInt(RTC_INT_WUT, DISABLE);
    RTC_EnableWakeUp(DISABLE);
    if (RTC_GetFlagStatus(RTC_FLAG_INITSF) == RESET)
    {
        RTC_DateRegulate();
        RTC_TimeRegulate();
    }

    Buzzer_Init();
    WorkInit();
    Key_Init();
    oil_init(OIL_150ml, DEFAULT_OIL_CONSUME_SPEED_8);

#if  (CFG_APP_NS_IUS)
    if (CURRENT_APP_START_ADDRESS == NS_APP1_START_ADDRESS)
    {
        NS_LOG_INFO("application 1 start new ...\r\n");
    }
    else if (CURRENT_APP_START_ADDRESS == NS_APP2_START_ADDRESS)
    {
        NS_LOG_INFO("application 2 start new ...\r\n");
    }
#endif

    /* 始终初始化 BLE 协议栈，保证应用定时任务、按键和蜂鸣器调度正常；
     * 只有检测到外部电源时才会在 app_ble_init() 内启动广播。 */
    app_ble_init();
    if (dcin_connected)
    {
        NS_LOG_INFO(DEMO_STRING);
    }

    Qflash_Init();
    Iap_Read();

    function.CheckStyle = OIL_CALCULATE;
    function.moto = 0;
    function.atmosphereLight = 0;
    function.quantityOfElectricity = 0;
    function.humanBodyInduction = 0;
    function.fan = 0;
    function.keyLock = 1;
    function.concentrationNum = 3;

    vk1621_init();
    if (dcin_connected)
    {
        Lcd_Display_Check();
    }
}


