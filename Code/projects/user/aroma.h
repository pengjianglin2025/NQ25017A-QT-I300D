#ifndef __AROMA_H__
#define __AROMA_H__

#include "main.h"

typedef enum
{
    S_NORMAL,
    S_ERROR,
} operationCountS_t;

typedef enum
{
    TIMEING_OFF,
    TIMEING_2H,
    TIMEING_4H,
    TIMEING_8H,
    TIMEING_CUSTOMIZE,   // 自定义定时
} timeing_t;

typedef enum
{
    SIMPLE,         // 简单模式
    PROFESSIONAL,   // 专业模式
} parameterMode_t;

typedef enum
{
    STOP,   // 停止状态
    WORK,   // 工作状态
    PAUSE,  // 暂停状态
} aromaWorkStatus_t;

typedef struct
{
    bool en;
    parameterMode_t parameterMode;                  // 运行参数模式
    aromaWorkStatus_t workStatus, workStatusOld;    // 香氛当前工作状态
    uint16_t currentWorkStatusRemainingTime;        // 当前状态剩余时间，单位：秒
    uint8_t concentration, concentrationOld;        // 浓度档位
    bool timeingParameterUpdate;                    // 定时参数更新标志
    timeing_t timeingGear, timeingGearOld;          // 定时档位，0 表示关闭定时
    uint16_t timeing;                               // 自定义定时时长，单位：分钟
    uint16_t timeLeft;                              // 定时模式剩余时间，单位：分钟
    uint16_t simpleModeCnt;                         // 简单模式浓度计数
    uint16_t simpleModeWorkSec, simpleModePauseSec; // 简单模式工作/暂停时长，单位：秒
    uint16_t currentWorkSec, currentPauseSec;       // 当前模式工作/暂停时长
    uint16_t currentEventWorkTimeInTotal;
    uint16_t currentEventPauseTimeInTotal;
    uint16_t currentEventWorkTimeInTotalOld;
    uint16_t currentEventPauseTimeInTotalOld;
//  uint32_t currentPauseTime;
    uint32_t runTimeInTotal;
    uint32_t runTimeInTotal_Aweek;                  // 一周累计运行时间，单位：秒
    uint32_t runTimeInTotal_AweekOld;
    uint32_t startTime;
} aroma_t;
extern aroma_t aroma;

typedef struct
{
    bool en;
    bool SW;
    bool SWOld;
    uint32_t cnt;
    uint8_t status;
    uint8_t statusOld;
} airpump_t;
extern airpump_t airpump;

typedef struct
{
    bool en;
    bool SW;
    uint8_t status;
    uint8_t statusOld;
} fan_t;
extern fan_t fan;

typedef struct
{
    bool En;
    uint8_t Up;
    uint8_t Down;
} moto_t;
extern moto_t moto;

void WorkInit(void);
void aroma_work_task(void);
void airpump_gpio_out(void);
aromaWorkStatus_t aroma_run_status(uint8_t num);
operationCountS_t airpump_operation_count_status(uint16_t workTime, uint16_t pauseTime);
void Work_Sleep_Config(void);

#endif
