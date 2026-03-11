#ifndef __MKEY_H__
#define __MKEY_H__

#include "main.h"

#define BSET(x,y) x|=(1<<y)    // 位置 1
#define BCLR(x,y) x&=~(1<<y)   // 位置 0

typedef enum
{
    KEY_IDLE,
    KEY_ONLINE,
} keyS_t;

typedef enum
{
    UNLOCK,
    LOCK,
} keyLock_t;

typedef enum
{
    KEY_MMODE = 0x01,
    KEY_SET   = 0x02,
    KEY_UP    = 0x04,
    KEY_DOWN  = 0x08,
} data_t;

// BYTE 为 8 位按键位图，B0~B7 对应各个位标志。
typedef union
{
    data_t BYTE;
    struct
    {
        uint8_t B0 : 1;
        uint8_t B1 : 1;
        uint8_t B2 : 1;
        uint8_t B3 : 1;
        uint8_t B4 : 1;
        uint8_t B5 : 1;
        uint8_t B6 : 1;
        uint8_t B7 : 1;   // 最高位
    };
} keyData_t;

typedef struct
{
    keyS_t status;
    keyData_t scan, scanOld, keep;
    uint8To8_t type;
    keyLock_t lockStatus;
    bool pressFlag;
//  bool reset, onceFlag;
    uint16_t cntA, idleCnt, delayTime;
    uint8_t clickCnt;
} key_t;
extern key_t key;

typedef struct
{
    uint16_t clockTime;
    uint8_t clockSeg;
    uint16_t parameterTime;
    uint16_t eventSeg;
    uint8_t eventSeg_bai;
    uint8_t eventSeg_shi;
    uint8_t eventSeg_ge;
    uint8_t weekdayNum;
} set_t;
extern set_t set;

void Key_Init(void);
void keyLock_status_set(keyLock_t sta);
void Key_Scan(void);
void Key_Task(void);
void Key_Rset(void);
void KeySleepSet(void);

#endif
