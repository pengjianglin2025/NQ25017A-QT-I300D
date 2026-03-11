#include "alarm.h"

volatile uint8_t alarmMode = BUZZER_MODE_NONE;  // 事件触发：外部写这个变量

static BuzzerMode_t s_mode = BUZZER_MODE_NONE;
static uint8_t s_step = 0;
static uint8_t s_tick = 0;

/* 配置参数 */
#define BUZZER_PWM_FREQ         4000    // PWM频率 4kHz
#define BUZZER_TIMER            TIM1    // 使用TIM1
#define BUZZER_TIMER_CLK        RCC_APB2_PERIPH_TIM1  // TIM1在APB2上
#define BUZZER_TIMER_CHANNEL    TIM_CH_2  // 通道2 (PB1对应TIM1_CH2)
#define BUZZER_GPIO_AF          GPIO_MODE_AF_PP  // 复用推挽输出
/* 私有变量 */
static uint8_t buzzer_state = 0;  // 蜂鸣器状态：0-关闭，1-开启

static void Buzzer_Pin_OutputLow(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStructure.Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_LOW;
    GPIO_InitPeripheral(BUZZER_GPIO, &GPIO_InitStructure);

    GPIO_ResetBits(BUZZER_GPIO, BUZZER_PIN);
}

static void Buzzer_Pin_OutputPwm(void)
{
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStructure.Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = BUZZER_GPIO_AF;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_LOW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1_TIM1;
    GPIO_InitPeripheral(BUZZER_GPIO, &GPIO_InitStructure);
}

/**
 * @brief 定时器PWM初始化（4kHz）
 * @param None
 * @retval None
 */
static void Buzzer_TIM_Init(void)
{
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    OCInitType TIM_OCInitStructure;
    uint16_t PrescalerValue;
    uint16_t Period;

    /* 使能定时器和GPIO时钟 */
    RCC_EnableAPB2PeriphClk(BUZZER_TIMER_CLK, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);

    /* 先将蜂鸣器引脚保持为低电平，避免初始化瞬态造成杂音。 */
    Buzzer_Pin_OutputLow();

    /* Compute the prescaler value */
    PrescalerValue = (uint16_t)(SystemCoreClock / 16000000) - 1;
    Period = (SystemCoreClock / (PrescalerValue + 1) / BUZZER_PWM_FREQ) - 1;

    /* Time base configuration */
    TIM_TimeBaseStructure.Period    = Period;
    TIM_TimeBaseStructure.Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(BUZZER_TIMER, &TIM_TimeBaseStructure);

    /* PWM模式配置 - 通道2 */
    TIM_OCInitStructure.OcMode = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse = 0;  // 初始化时默认关闭输出
    TIM_OCInitStructure.OcPolarity = TIM_OC_POLARITY_HIGH;
    TIM_InitOc2(BUZZER_TIMER, &TIM_OCInitStructure);
    TIM_ConfigOc2Preload(BUZZER_TIMER, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(BUZZER_TIMER, ENABLE);

    /* TIM1是高级定时器，需要使能主输出 */
    TIM_EnableCtrlPwmOutputs(BUZZER_TIMER, ENABLE);

    /* 启动定时器 */
    TIM_Enable(BUZZER_TIMER, ENABLE);

    /* 默认关闭PWM输出 */
    TIM_SetCmp2(BUZZER_TIMER, 0);
}

/**
 * @brief 蜂鸣器初始化
 * @param None
 * @retval None
 */
void Buzzer_Init(void)
{
    Buzzer_TIM_Init();
    Buzzer_Off();
}

/**
 * @brief 蜂鸣器开启（启动4kHz PWM）
 * @param None
 * @retval None
 */
void Buzzer_On(void)
{
    uint16_t Period = BUZZER_TIMER->AR;  // 直接读取自动重装载寄存器

    if (!buzzer_state)
    {
        Buzzer_Pin_OutputPwm();
    }

    TIM_SetCmp2(BUZZER_TIMER, Period / 2);  // 设置50%占空比
    buzzer_state = 1;
}

/**
 * @brief 蜂鸣器关闭（停止PWM）
 * @param None
 * @retval None
 */
void Buzzer_Off(void)
{
    TIM_SetCmp2(BUZZER_TIMER, 0);  // CCR=0，先关闭PWM占空比
    Buzzer_Pin_OutputLow();        // 空闲时将引脚强制拉低，避免BLE状态切换带来瞬态杂音
    buzzer_state = 0;
}

void Buzzer_Task10ms(void)
{
    /* 读取全局触发，读取后清零，避免重复触发 */
    if (alarmMode != BUZZER_MODE_NONE) {
        s_mode = (BuzzerMode_t)alarmMode;
        s_step = 0;
        s_tick = 0;
        alarmMode = BUZZER_MODE_NONE;
    }

    switch (s_mode)
    {
    case BUZZER_MODE_KEY:
        /* 按键音：响30ms */
        switch (s_step) {
        case 0: Buzzer_On();  s_tick = 3; s_step = 1; break;
        case 1:
            if (s_tick > 0) s_tick--;
            else { Buzzer_Off(); s_mode = BUZZER_MODE_NONE; }
            break;
        default:
            Buzzer_Off(); s_mode = BUZZER_MODE_NONE; break;
        }
        break;

    case BUZZER_MODE_RESET:
        /* 复位音：响80ms -> 停50ms -> 响80ms */
        switch (s_step) {
        case 0: Buzzer_On();  s_tick = 8; s_step = 1; break;
        case 1:
            if (s_tick > 0) s_tick--;
            else { Buzzer_Off(); s_tick = 5; s_step = 2; }
            break;
        case 2:
            if (s_tick > 0) s_tick--;
            else { Buzzer_On(); s_tick = 8; s_step = 3; }
            break;
        case 3:
            if (s_tick > 0) s_tick--;
            else { Buzzer_Off(); s_mode = BUZZER_MODE_NONE; }
            break;
        default:
            Buzzer_Off(); s_mode = BUZZER_MODE_NONE; break;
        }
        break;

    case BUZZER_MODE_NONE:
    default:
        break;
    }
}
