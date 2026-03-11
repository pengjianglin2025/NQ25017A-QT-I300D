#include "main.h"

/*
 * 低功耗硬件排查测试说明：
 * 1) 默认先把所有中断源关掉，避免被无关中断频繁唤醒；
 * 2) 默认只保留DCIN作为唤醒源；
 * 3) 关键外设控制脚强制到“关闭电平”，其余脚尽量设为模拟输入。
 */
#define LPTEST_ENABLE_DCIN_WAKEUP 1
/* 上电调试连接窗口(10us为单位)。设置为0可关闭。 */
#define LPTEST_DEBUG_ATTACH_WINDOW_10US (300 * 1000)

power_t power;
WStatus_t globalWorkState = FULL_WORKING;
function_t function;

void app_dcin_power_manage_proc(void)
{
    /* LPTEST bypasses the normal app task/BLE power-management path. */
}

void app_sleep_prepare_proc(void)
{
    /* LPTEST enters PD directly from its private loop. */
}

void app_sleep_resume_proc(void)
{
    /* LPTEST restores clocks/GPIO locally after each wakeup. */
}

static void lp_disable_all_exti(void)
{
    /* 先全部屏蔽，避免历史配置残留导致WFI立刻返回。 */
    EXTI->IMASK = 0x00000000;
    EXTI->EMASK = 0x00000000;
    EXTI->RT_CFG = 0x00000000;
    EXTI->FT_CFG = 0x00000000;
    EXTI->PEND = 0x00FFFFFF;
}

static void lp_disable_all_nvic(void)
{
    /* Cortex-M0外部中断寄存器清空。 */
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICPR[0] = 0xFFFFFFFF;
}

static void lp_force_outputs_to_off_state(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    /* 先将除SWD外的GPIO置模拟输入，避免关闭烧录/调试口。 */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.Pin = (uint16_t)(GPIO_PIN_ALL & (uint16_t)~(SWDCLK_PIN | SWDIO_PIN));
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* A口关键控制脚：LCD相关全部拉低，确保背光和接口静默。 */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = LCD_CS_PIN | LCD_WR_PIN | LCD_DATD_PIN | LCD_BACKLIGHT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_LOW;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    LCD_CS_LOW();
    LCD_WR_LOW();
    LCD_DATD_LOW();
    LCD_BACKLIGHT_OFF();

    /* B口关键控制脚：强制关闭电机、风扇、蜂鸣器，LED置灭。 */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = MOTOR_DC_MT_PIN | MOTOR_IN1_PIN | MOTOR_IN2_PIN |
                             FAN_CTRL_PIN | BUZZER_PIN | LED_RED_PIN | LED_BLUE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_LOW;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
    MOTOR_DISABLE();
    MOTOR_IN1_LOW();
    MOTOR_IN2_LOW();
    FAN_OFF();
    BUZZER_OFF();
    LED_RED_OFF();
    LED_BLUE_OFF();

    /* 未使用PB8/PB9固定为模拟输入，避免悬空干扰。 */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

static void lp_config_dcin_wakeup(void)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* DCIN(PB13)作为唯一唤醒输入。 */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = DC_IN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(DC_IN_GPIO, &GPIO_InitStructure);

    GPIO_ConfigEXTILine(DCIN_PORT_SOURCE, DCIN_PIN_SOURCE);
    EXTI_ClrITPendBit(DCIN_EXTI_LINE);

    EXTI_InitStructure.EXTI_Line = DCIN_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DCIN_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void lp_enter_pd_mode_wfi(void)
{
    uint32_t tmpreg = PWR->CR1;
    volatile uint32_t* const ble_cfg_reg = (volatile uint32_t*)0x40011004u;
    uint32_t ble_cfg_backup = *ble_cfg_reg;

    tmpreg &= ~PWR_CR1_MODE_SEL;
    tmpreg |= (PWR_CR1_MODE_PD | PWR_CR1_MODE_EN);
    PWR->CR1 = tmpreg;

    SCB->SCR |= SCB_SCR_SLEEPDEEP;
#if defined(__CC_ARM)
    __force_stores();
#endif
    __WFI();

    RCC->LSCTRL |= 1u;
    RCC->CFG |= RCC_HCLK_DIV2;
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);
    *ble_cfg_reg = ble_cfg_backup;

    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);
}

int main(void)
{
    /* 不启BLE、不启业务、不启SysTick，只做最低功耗路径验证。 */
    RCC->APB1PCLKEN |= RCC_APB1_PERIPH_PWR;

    /* 始终预留SWD连接窗口，避免测试固件上电即睡眠后无法重新烧录。 */
    if (LPTEST_DEBUG_ATTACH_WINDOW_10US > 0)
    {
        delay_n_10us(LPTEST_DEBUG_ATTACH_WINDOW_10US);
    }

    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    lp_disable_all_nvic();
    lp_disable_all_exti();
    lp_force_outputs_to_off_state();

#if LPTEST_ENABLE_DCIN_WAKEUP
    lp_config_dcin_wakeup();
#endif

    while (1)
    {
        lp_enter_pd_mode_wfi();
    }
}
