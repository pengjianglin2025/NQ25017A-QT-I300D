
#include "mkey.h"


key_t key;
set_t set;

void Key_Init(void)
{
	key.keep.BYTE = 0;
	key.type.BYTE = 0;
	key.pressFlag = 0;
//	key.reset = 1;
}
void Key_Rset(void)
{
	key.keep.BYTE = 0;
	key.type.BYTE = 0;
	key.pressFlag = 0;
//	key.reset = 1;
}
/*************按键锁状态设置，必须清计数***************/
void keyLock_status_set(keyLock_t sta)
{
	key.lockStatus = sta;
	key.idleCnt = 0;
}
void Key_Scan(void)
{
	key.scan.BYTE = 0;
	
	if(KEY_MODE_READ()) {	 key.scan.B0 = 1; }
	if(KEY_SET_READ()) { key.scan.B1 = 1; }
	if(KEY_UP_READ()) { key.scan.B2 = 1; }
	if(KEY_DOWN_READ()) { key.scan.B3 = 1; }
	
	if(key.scan.BYTE)								//有按键						
	{
		key.idleCnt = 0;
		key.pressFlag = 1;
		key.status = KEY_ONLINE;
		set.clockTime = 0; 
		set.parameterTime = 0;
		lcd.backOnTime = 0;
		
		if(key.scanOld.BYTE == key.scan.BYTE)				//键值等于旧键值
		{
			if(++key.cntA > 65530) key.cntA = 65530;
			
			if((key.type.BYTE == 0x03) && (key.clickCnt == 0))				//有按键判断长按键
			{
        if((key.scan.BYTE == KEY_SET) && (key.cntA == 750))
				{
					key.keep.BYTE = KEY_SET;
					key.type.B2 = 1;		//长按键标志	
				}
				if((key.scan.BYTE == KEY_MMODE) && (key.cntA == 375))
				{
					key.keep.BYTE = KEY_MMODE;
					key.type.B2 = 1;		//长按键标志	
				}		
				
				if((key.scan.BYTE == KEY_UP) && (key.cntA == 250))
				{
					key.keep.BYTE = KEY_UP;
					key.type.B2 = 1;		//长按键标志	
				}	
				if((key.scan.BYTE == KEY_DOWN) && (key.cntA == 250))
				{
					key.keep.BYTE = KEY_DOWN;
					key.type.B2 = 1;		//长按键标志	
				}	
				if((key.scan.BYTE == KEY_DOWN) && (key.cntA == 500))
				{
					key.keep.BYTE = KEY_DOWN;
					key.type.B4 = 1;		//长按键标志	
				}	
			}
      else if((key.type.BYTE == 0x03) && (key.clickCnt == 1))	 //判断按键双击
			{
				if(key.scan.BYTE == KEY_MMODE)
				{
					key.keep.BYTE = KEY_MMODE;
					key.type.B3 = 1;		//双击按键标志
				}
			}				
			else if((key.type.BYTE == 0) && (key.cntA >= 3) && (key.clickCnt == 0))			//消抖动
			{
				key.type.B0 = 1;			//有按键标志
				key.type.B1 = 1;			//短按键标志
				if(key.scan.B0) key.keep.B0 = 1;
				if(key.scan.B1) key.keep.B1 = 1;
				if(key.scan.B2) key.keep.B2 = 1;
				if(key.scan.B3) key.keep.B3 = 1;
				if(key.scan.B4) key.keep.B4 = 1;
				if(key.scan.B5) key.keep.B5 = 1;
			}
		}
		else
		{
			key.scanOld.BYTE = key.scan.BYTE;
			key.cntA = 0;	
			key.idleCnt = 0;
		}		
	}
	else//无按键
	{
		key.cntA =0;
		if(++key.idleCnt > 65530)   key.idleCnt = 65530;
		
		if(key.idleCnt == 3)
		{
			if(key.keep.B4)  key.clickCnt++;
			else key.pressFlag = 0;
		}
		else if(key.idleCnt == 100)
		{
			key.pressFlag = 0;
		}
		
		if((key.pressFlag == 0) && (key.type.B0 == 0))
		{
			key.clickCnt = 0;
			key.keep.BYTE = 0;
			key.type.BYTE = 0; 
			key.scanOld.BYTE = 0;
		}
		
   if(key.idleCnt > 200)
	 { 
		 key.status = KEY_IDLE;
		 key.keep.BYTE = 0;
		 key.type.BYTE = 0; 
		 key.scanOld.BYTE = 0;
		 key.pressFlag = 0;
	 }
	 if(key.idleCnt > 5000)
		{
			if(key.lockStatus == UNLOCK)   //防止频繁写flash
			{
				key.lockStatus = LOCK;
			}
		}
	}	  
}

void Key_Task(void)
{
	static uint16_t CntA;
	uint8_t i;
	
	if(key.type.BYTE == 0x03)   //短按键操作
	{	
		switch(key.keep.BYTE)
		{
			case KEY_MMODE:   //MODE
			{
				if(key.pressFlag == 0)
				{
					if((power.status) && (key.lockStatus == UNLOCK))
					{
						if(globalWorkState == SET_CLOCK)
						{
							RTC_SetDate(RTC_FORMAT_BIN, &RTC_DateStructure);
							RTC_ConfigTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
							globalWorkState = FULL_WORKING;
						}
						else if(globalWorkState != FULL_WORKING)
						{
							globalWorkState = FULL_WORKING;
						}
						if(aroma.en == ON) alarmMode = 1;
					}
					key.type.B0 = 0;			
				}
			}break;	
			case KEY_SET:   //SET
			{
				if(key.pressFlag==0)
				{
					if((power.status) && (key.lockStatus==0))
					{
						if(globalWorkState == FULL_WORKING)
						{
							globalWorkState = SET_GEAR;
							if(aroma.concentration == 0) 
							{
								aroma.concentration = 1;
								for(i=0; i<5; i++)
								{
									runEvent[i].en = 0;
								}
								
								runEvent[0].en = 1;
								runEvent[0].weekEn.BYTE = 0xff;
								runEvent[0].startTimeHour = 0;     
								runEvent[0].startTimeMinutes= 0;
								runEvent[0].stopTimeHour = 23;     
								runEvent[0].stopTimeMinutes= 59;
								
								runEvent[0].workTime = 15;
								runEvent[0].pauseTime = 120;
								aroma.startTime = 0;
							}
						}
						else if(globalWorkState == SET_GEAR)
						{
							if(++aroma.concentration > 5) aroma.concentration = 1;
							
								for(i=0; i<5; i++)
								{
									runEvent[i].en = 0;
								}
								
								runEvent[0].en = 1;
								runEvent[0].weekEn.BYTE = 0xff;
								runEvent[0].startTimeHour = 0;     
								runEvent[0].startTimeMinutes= 0;
							
								runEvent[0].stopTimeHour = 23;     
								runEvent[0].stopTimeMinutes= 59; 
								
								switch(aroma.concentration)
								{
									case 1:
									{
										runEvent[0].workTime = 15;
										runEvent[0].pauseTime = 120;
										aroma.startTime = 0;
									}
									break;
									case 2:
									{
										runEvent[0].workTime = 15;
										runEvent[0].pauseTime = 60;
										aroma.startTime = 0;
									}
									break;
									case 3:
									{
										runEvent[0].workTime = 40;
										runEvent[0].pauseTime = 100;
										aroma.startTime = 0;
									}
									break;
									case 4:
									{
										runEvent[0].workTime = 60;
										runEvent[0].pauseTime = 60;
										aroma.startTime = 0;
									}
									break;
									case 5:
									{
										runEvent[0].workTime = 180;
										runEvent[0].pauseTime = 60;
										aroma.startTime = 0;
									}
									break;
								}
						}
						else if(globalWorkState == SET_CLOCK)
						{
							if(++set.clockSeg > 2)
							{
								set.clockSeg = 0;
//								globalWorkState = FULL_WORKING;
							}	
							RTC_SetDate(RTC_FORMAT_BIN, &RTC_DateStructure);
							RTC_ConfigTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
						}
						if(aroma.en == ON) alarmMode = 1;
					}
					key.type.B0 = 0;			
				}
			}break;
			
			case KEY_UP:   //UP
			{
				if(key.pressFlag==0)
				{
					if((power.status) && (key.lockStatus==0))
					{
						if(globalWorkState == FULL_WORKING)
						{
//							globalWorkState = SET_TIMEING;
//							alarmMode = 1;
						}
						else if(globalWorkState == SET_TIMEING)
						{
							aroma.timeing += 30;
							aroma.timeing -= (aroma.timeing%30);
							if(aroma.timeing > 1440) aroma.timeing = 0;
						}
						else if(globalWorkState == SET_CLOCK)
						{
							switch(set.clockSeg)
							{
								case 0: { if(++RTC_TimeStructure.Hours > 23) RTC_TimeStructure.Hours = 0; }break;
								case 1: { if(++RTC_TimeStructure.Minutes > 59) RTC_TimeStructure.Minutes = 0; }break;
								case 2: { if(++RTC_DateStructure.WeekDay > CLOCK_WEEK_MAX) RTC_DateStructure.WeekDay = CLOCK_WEEK_MIN; }break;
								case 3: { oil.totalVolume += 5; if(oil.totalVolume > 5000) oil.totalVolume = 0; }break;
								case 4: { oil.curretVolume += 5; if(oil.curretVolume > oil.totalVolume) oil.curretVolume = 0;  }break;
								case 5: { if(++oil.actualConsumeSpeed > oil.actualConsumeSpeedMax) oil.actualConsumeSpeed = oil.actualConsumeSpeedMax; }break;
							}
						}
						if(aroma.en == ON) alarmMode = 1;
					}
					key.type.B0 = 0;			
				}
			}break;	
			
			case KEY_DOWN:   //DOWN
			{
				if(key.pressFlag==0)
				{
					if((power.status) && (key.lockStatus==0))
					{
						if(globalWorkState == FULL_WORKING)
						{
						}
						else if(globalWorkState == SET_CLOCK)
						{
							switch(set.clockSeg)
							{
								case 0: { if(--RTC_TimeStructure.Hours > 23) RTC_TimeStructure.Hours = 23; }break;
								case 1: { if(--RTC_TimeStructure.Minutes > 59) RTC_TimeStructure.Minutes = 59; }break;
								case 2: 
								{ 
									--RTC_DateStructure.WeekDay;
									if((RTC_DateStructure.WeekDay > CLOCK_WEEK_MAX) || (RTC_DateStructure.WeekDay < CLOCK_WEEK_MIN)) RTC_DateStructure.WeekDay = CLOCK_WEEK_MAX; 
								}break;
								case 3: { oil.totalVolume -= 5; if(oil.totalVolume > 5000) oil.totalVolume = 5000; }break;
								case 4: { oil.curretVolume -= 5; if(oil.curretVolume > oil.totalVolume) oil.curretVolume = oil.totalVolume;  }break;
								case 5: 
								{ 
									oil.actualConsumeSpeed--;
									if((oil.actualConsumeSpeed <= oil.actualConsumeSpeedMin) || (oil.actualConsumeSpeed > oil.actualConsumeSpeedMax))
									oil.actualConsumeSpeed = oil.actualConsumeSpeedMin; 
								}break;
							}
						}
						if(aroma.en == ON) alarmMode = 1;
					}
					key.type.B0 = 0;			
				}
			}break;
						
			default: /*key.reset = 1;*/ break;			
		}
	}
	if(key.type.BYTE == 0x07)    //长按键操作
	{	
//		BCLR(KeyFlag,0);
        		
		switch(key.keep.BYTE)
		{
			case KEY_MMODE:   //MODE
			{
//				if(power.status == ALL_OFF)
//				{
//					power.status = FULL_POWER_OPERATION;
//					globalWorkState = FULL_WORKING;
//					alarmMode = 1;
//				}
//				else if((power.status) && (key.lockStatus==0)) 
//				{
//					power.status = ALL_OFF;
//					alarmMode = 1;
//				}
				if(aroma.en == OFF)
				{
					power.status = POWER_ON;
					globalWorkState = FULL_WORKING;
					aroma.en = ON;
					key.lockStatus = 0;
					oil.armalWaitTime = 0;
					alarmMode = 1;
				}
				else if((power.status) && (key.lockStatus==0)) 
				{
					if(aroma.en == ON)
					{
						aroma.en = OFF;
						alarmMode = 1;
					}
				}
				key.type.B0 = 0;
			}break;
			case KEY_SET:   //SET
			{
				if((power.status) && (key.lockStatus==0)) 
				{
					if(globalWorkState == FULL_WORKING)
					{
						globalWorkState = SET_CLOCK;
					}
					alarmMode = 1;
				}
				key.type.B0 = 0;
			}break;
			case KEY_UP:   //UP
			{
				if(power.status) 
				{
//					if(globalWorkState == SET_TIMEING)
//					{
//						if(++CntA > 40)
//						{
//							CntA = 0;
//							aroma.timeing += 30;
//							aroma.timeing -= (aroma.timeing%30);
//							if(aroma.timeing > 1440) aroma.timeing = 0;
//							
//							alarmMode = 1;
//						}
//						if(key.pressFlag == 0) { /*key.reset = 1;*/ key.type.B0 = 0; }
//					}
					if(key.lockStatus)
					{
						key.lockStatus = UNLOCK;
						alarmMode = 1;
						key.type.B0 = 0;
					}
					else if(globalWorkState == SET_CLOCK)
					{
						if(++CntA > 40)
						{
							CntA = 0;
							switch(set.clockSeg)
							{
								case 0: { if(++RTC_TimeStructure.Hours > 23) RTC_TimeStructure.Hours = 0; }break;
								case 1: { if(++RTC_TimeStructure.Minutes > 59) RTC_TimeStructure.Minutes = 0; }break;
								case 2: { if(++RTC_DateStructure.WeekDay > CLOCK_WEEK_MAX) RTC_DateStructure.WeekDay = CLOCK_WEEK_MIN; }break;
								case 3: { oil.totalVolume += 5; if(oil.totalVolume > 5000) oil.totalVolume = 0; }break;
								case 4: { oil.curretVolume += 5; if(oil.curretVolume > oil.totalVolume) oil.curretVolume = 0;  }break;
								case 5: { if(++oil.actualConsumeSpeed > oil.actualConsumeSpeedMax) oil.actualConsumeSpeed = oil.actualConsumeSpeedMax; }break;
							}
							alarmMode = 1;
						}
						if(key.pressFlag == 0) { /*key.reset = 1;*/ key.type.B0 = 0; }
					}
				}
			}break;	
			case KEY_DOWN:   //DOWN
			{
				if((power.status) && (key.lockStatus==0))
				{
					if(globalWorkState == SET_CLOCK)
					{
						if(++CntA > 40)
						{
							CntA = 0;
							switch(set.clockSeg)
							{
								case 0: { if(--RTC_TimeStructure.Hours > 23) RTC_TimeStructure.Hours = 23; }break;
								case 1: { if(--RTC_TimeStructure.Minutes > 59) RTC_TimeStructure.Minutes = 59; }break;
								case 2: 
								{ 
									--RTC_DateStructure.WeekDay;
									if((RTC_DateStructure.WeekDay > CLOCK_WEEK_MAX) || (RTC_DateStructure.WeekDay < CLOCK_WEEK_MIN)) RTC_DateStructure.WeekDay = CLOCK_WEEK_MAX; 
								}break;
								case 3: { oil.totalVolume -= 5; if(oil.totalVolume > 5000) oil.totalVolume = 5000; }break;
								case 4: { oil.curretVolume -= 5; if(oil.curretVolume > oil.totalVolume) oil.curretVolume = oil.totalVolume;  }break;
								case 5: 
								{ 
									oil.actualConsumeSpeed--;
									if((oil.actualConsumeSpeed <= oil.actualConsumeSpeedMin) || (oil.actualConsumeSpeed > oil.actualConsumeSpeedMax))
									oil.actualConsumeSpeed = oil.actualConsumeSpeedMin; 
								}break;
							}
							alarmMode = 1;
						}
						if(key.pressFlag == 0) { /*key.reset = 1;*/ key.type.B0 = 0; }
					}
					else
					{
						if(++CntA > 500)
						{
							CntA = 0;
							event_date_init();
							keyLock_status_set(UNLOCK);
							globalWorkState = MODE_RESET;
							set.eventSeg = 0;
							aroma.concentration = 0;
	//					LedDataResetFlag = 1;
							alarmMode = 4;
							key.type.B0 = 0;
						}
            if(key.pressFlag == 0) { /*key.reset = 1;*/ key.type.B0 = 0; }
					}
				}
			}break;
			
			default: /*key.reset = 1;*/ break;
		}
	}	
	if(key.type.BYTE == 0x0b)    //有双击按键标志
	{	
		switch(key.keep.BYTE)
		{
			case KEY_MMODE:   //MODE
			{
				if((power.status) && (key.lockStatus == 0)) 
				{
				}
				key.type.B0 = 0;
			}break;
			default:   break;
		}
	}
}


void KeySleepSet(void)
{
}
