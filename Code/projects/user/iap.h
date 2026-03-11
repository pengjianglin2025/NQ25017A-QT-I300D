#ifndef _IAP_H_
#define _IAP_H_

#include "main.h"

#define IAP_WRITE_OK   0x55
#define IAP_START_ADDR 0x500200

extern uint32_t Iap_Read_Temp[10];

#pragma pack(push, 1)
typedef struct {
    uint8_t startTimeHour;
    uint8_t startTimeMinutes;
    uint8_t stopTimeHour;
    uint8_t stopTimeMinutes;
    uint16To2_t workTime;
    uint16To2_t pauseTime;
    uint8_t workGear;
    uint8_t workWeek;
    uint8_t eventEN;
} EventIap_t;

typedef struct
{
    uint8_t writeOk;
    EventIap_t EventData[5];
    uint8_t FanEN;
    uint8_t workState;
    uint8_t keyLockState;
    uint8_t lightEn;
    uint16To2_t totalVolume;
    uint16To2_t curretVolume;
    uint16To2_t consumeSpeed;
    uint8_t rollingCode1;
    uint8_t rollingCode2;
} IapPayload_t;
#pragma pack(pop)

#define IAP_DATA_MAX   ((uint16_t)sizeof(IapPayload_t))

typedef union
{
    uint8_t Buffer[IAP_DATA_MAX];
    IapPayload_t Data;
} Iap_t;

void Iap_Read(void);
void Iap_Write(void);
void Iap_Data_Rest(void);
void Iap_Data_Comparison(void);

#endif
