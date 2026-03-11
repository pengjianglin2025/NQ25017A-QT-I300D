#include "iap.h"

Iap_t IapRead, IapWrite;
uint8_t IapWriteEn = 0;

#define FLASH_TEST_ADDRESS       0x1020000
#define BUFFER_SIZE              IAP_DATA_MAX

static uint8_t iap_event_data_is_valid(void)
{
    uint8_t i;

    for (i = 0; i < 5; i++)
    {
        if (IapRead.Data.EventData[i].startTimeHour > 23) return 0;
        if (IapRead.Data.EventData[i].stopTimeHour > 23) return 0;
        if (IapRead.Data.EventData[i].startTimeMinutes > 59) return 0;
        if (IapRead.Data.EventData[i].stopTimeMinutes > 59) return 0;
    }

    return 1;
}

void Iap_Read(void)
{
    uint8_t i;
    uint8_t oil_param_valid = 1;

    memset(IapRead.Buffer, 0, sizeof(IapRead.Buffer));
    Qflash_Read(FLASH_TEST_ADDRESS, IapRead.Buffer, BUFFER_SIZE);

    if ((IapRead.Data.writeOk == IAP_WRITE_OK) && iap_event_data_is_valid())
    {
        for (i = 0; i < 5; i++)
        {
            runEvent[i].startTimeHour = IapRead.Data.EventData[i].startTimeHour;
            runEvent[i].startTimeMinutes = IapRead.Data.EventData[i].startTimeMinutes;
            runEvent[i].stopTimeHour = IapRead.Data.EventData[i].stopTimeHour;
            runEvent[i].stopTimeMinutes = IapRead.Data.EventData[i].stopTimeMinutes;
            runEvent[i].workTime = IapRead.Data.EventData[i].workTime.WORD;
            runEvent[i].pauseTime = IapRead.Data.EventData[i].pauseTime.WORD;
            runEvent[i].workPer = IapRead.Data.EventData[i].workGear;
            runEvent[i].weekEn.BYTE = IapRead.Data.EventData[i].workWeek;
            runEvent[i].en = IapRead.Data.EventData[i].eventEN;
        }

        fan.en = IapRead.Data.FanEN;
        aroma.en = IapRead.Data.workState;
        key.lockStatus = IapRead.Data.keyLockState;

        oil.totalVolume = IapRead.Data.totalVolume.WORD;
        oil.curretVolume = IapRead.Data.curretVolume.WORD;
        oil.actualConsumeSpeed = IapRead.Data.consumeSpeed.WORD;

        if (oil.totalVolume < 50 || oil.totalVolume > 1000) oil_param_valid = 0;
        if (oil.curretVolume > oil.totalVolume) oil_param_valid = 0;
        if (oil.actualConsumeSpeed == 0 || oil.actualConsumeSpeed > 2000) oil_param_valid = 0;

        if (!oil_param_valid)
        {
            oil_reset();
            fan.en = ON;
            aroma.en = ON;
            key.lockStatus = UNLOCK;
            Iap_Write();
        }
    }
    else
    {
        event_date_init();
        oil_reset();
        fan.en = ON;
        aroma.en = ON;
        key.lockStatus = UNLOCK;
        Iap_Write();
    }
}

void Iap_Write(void)
{
    uint8_t i, k;

    memset(IapWrite.Buffer, 0, sizeof(IapWrite.Buffer));

    IapWrite.Data.writeOk = IAP_WRITE_OK;
    for (i = 0; i < 5; i++)
    {
        IapWrite.Data.EventData[i].startTimeHour = runEvent[i].startTimeHour;
        IapWrite.Data.EventData[i].startTimeMinutes = runEvent[i].startTimeMinutes;
        IapWrite.Data.EventData[i].stopTimeHour = runEvent[i].stopTimeHour;
        IapWrite.Data.EventData[i].stopTimeMinutes = runEvent[i].stopTimeMinutes;
        IapWrite.Data.EventData[i].workTime.WORD = runEvent[i].workTime;
        IapWrite.Data.EventData[i].pauseTime.WORD = runEvent[i].pauseTime;
        IapWrite.Data.EventData[i].workGear = runEvent[i].workPer;
        IapWrite.Data.EventData[i].workWeek = runEvent[i].weekEn.BYTE;
        IapWrite.Data.EventData[i].eventEN = runEvent[i].en;
    }

    IapWrite.Data.FanEN = fan.en;
    IapWrite.Data.workState = aroma.en;
    IapWrite.Data.keyLockState = key.lockStatus;
    IapWrite.Data.totalVolume.WORD = oil.totalVolume;
    IapWrite.Data.curretVolume.WORD = oil.curretVolume;
    IapWrite.Data.consumeSpeed.WORD = oil.actualConsumeSpeed;

    for (k = 0; k < 3; k++)
    {
        Qflash_Erase_Sector(FLASH_TEST_ADDRESS);
        Qflash_Write(FLASH_TEST_ADDRESS, IapWrite.Buffer, BUFFER_SIZE);

        memset(IapRead.Buffer, 0, sizeof(IapRead.Buffer));
        Qflash_Read(FLASH_TEST_ADDRESS, IapRead.Buffer, BUFFER_SIZE);

        IapWriteEn = 0;
        for (i = 1; i < BUFFER_SIZE; i++)
        {
            if (IapRead.Buffer[i] != IapWrite.Buffer[i])
            {
                IapWriteEn = 1;
                break;
            }
        }

        if (!IapWriteEn)
        {
            break;
        }
    }

    IapWriteEn = 0;
}

void Iap_Data_Comparison(void)
{
    uint8_t i;

    if (globalWorkState == FULL_WORKING)
    {
        memset(IapRead.Buffer, 0, sizeof(IapRead.Buffer));
        Qflash_Read(FLASH_TEST_ADDRESS, IapRead.Buffer, BUFFER_SIZE);

        memset(IapWrite.Buffer, 0, sizeof(IapWrite.Buffer));
        IapWrite.Data.writeOk = IAP_WRITE_OK;
        for (i = 0; i < 5; i++)
        {
            IapWrite.Data.EventData[i].startTimeHour = runEvent[i].startTimeHour;
            IapWrite.Data.EventData[i].startTimeMinutes = runEvent[i].startTimeMinutes;
            IapWrite.Data.EventData[i].stopTimeHour = runEvent[i].stopTimeHour;
            IapWrite.Data.EventData[i].stopTimeMinutes = runEvent[i].stopTimeMinutes;
            IapWrite.Data.EventData[i].workTime.WORD = runEvent[i].workTime;
            IapWrite.Data.EventData[i].pauseTime.WORD = runEvent[i].pauseTime;
            IapWrite.Data.EventData[i].workGear = runEvent[i].workPer;
            IapWrite.Data.EventData[i].workWeek = runEvent[i].weekEn.BYTE;
            IapWrite.Data.EventData[i].eventEN = runEvent[i].en;
        }

        IapWrite.Data.FanEN = fan.en;
        IapWrite.Data.workState = aroma.en;
        IapWrite.Data.keyLockState = key.lockStatus;
        IapWrite.Data.totalVolume.WORD = oil.totalVolume;
        IapWrite.Data.curretVolume.WORD = oil.curretVolume;
        IapWrite.Data.consumeSpeed.WORD = oil.actualConsumeSpeed;

        for (i = 1; i < BUFFER_SIZE; i++)
        {
            if (IapRead.Buffer[i] != IapWrite.Buffer[i])
            {
                IapWriteEn = 1;
                if (!upData.DPID018Back) upData.DPID018Back = 1;
                if (i < 56) aroma.startTime = 0;
                break;
            }
        }

        if (IapRead.Data.FanEN != IapWrite.Data.FanEN) { IapWriteEn = 1; if (!upData.DPID004Back) upData.DPID004Back = 1; }
        if (IapRead.Data.workState != IapWrite.Data.workState) { IapWriteEn = 1; if (!upData.DPID001Back) upData.DPID001Back = 1; }
        if (IapRead.Data.keyLockState != IapWrite.Data.keyLockState) { IapWriteEn = 1; if (!upData.DPID005Back) upData.DPID005Back = 1; }
        if (IapRead.Data.curretVolume.WORD != IapWrite.Data.curretVolume.WORD) { IapWriteEn = 1; if (!upData.DPID020Back) upData.DPID020Back = 1; }
        if (IapRead.Data.consumeSpeed.WORD != IapWrite.Data.consumeSpeed.WORD) { IapWriteEn = 1; if (!upData.DPID020Back) upData.DPID020Back = 1; }

        if (IapWriteEn)
        {
            IapWriteEn = 0;
            IapWrite.Data.writeOk = IAP_WRITE_OK;
            Qflash_Erase_Sector(FLASH_TEST_ADDRESS);
            Qflash_Write(FLASH_TEST_ADDRESS, IapWrite.Buffer, BUFFER_SIZE);
        }
    }
    else
    {
        IapWriteEn = 0;
    }
}

void Iap_Data_Rest(void)
{
}

