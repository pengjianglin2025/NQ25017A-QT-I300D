#include "lcd.h"

lcd_t lcd;

void Lcd_Display_Check(void)
{
	uint8_t i;
	
	LCD_BACKLIGHT_ON();
	for(i=0;i<16;i++) { DplyData.Buffer[i]=0xff; }
	Write_1621_RAM(0,DplyData.Buffer,16);
	
	delay_n_ms(1000);
	
	for(i=0;i<16;i++) { DplyData.Buffer[i]=0x00; }
	Write_1621_RAM(0,DplyData.Buffer,16);
}

void Lcd_Task(void)
{
	static uint8_t i,CntA;
//	if(power.status)
//	{
//		if(++lcd.backOnTime > 3000) { lcd.backOnTime = 3001;}
//		else { lcd.status = 1; }
//	}
//	else { lcd.status = 0; }
	
//	if(lcd.status)
//	{
//		if(CntA == 0) { vk1621_init(); }
//		if(++CntA > 200) CntA = 200;
//		LCD_BACKLIGHT_ON();
//		Write_1621_RAM(0,DplyData.Buffer,16);
//	}
//	else 
//	{
//		CntA = 0;
//		SleepLcd_Config();
//	}
		
	if((power.status) && (aroma.en))
	{
		lcd.status = 1;
		LCD_BACKLIGHT_ON();
		if(++lcd.backOnTime > 3000) 
		{ 
			lcd.backOnTime = 3001;
			//LCD_BACKLIGHT_OFF();
		}
	}
	else 
	{ 
		lcd.status = 0; 
		LCD_BACKLIGHT_OFF();
	}
	
	if(lcd.status)
	{
		if(CntA == 0) 
		{ 
			vk1621_init(); 
		}
		else
		{
			
		}
		if(++CntA > 200) CntA = 200;
	}
	else 
	{
		CntA = 0;
//		SleepLcd_Config();
		for(i=0;i<16;i++) { DplyData.Buffer[i]=0x00; }
	}
	Write_1621_RAM(0,DplyData.Buffer,16);
}

void Display_Task(void)
{
	uint8_t i;
	static uint16_t CntA,CntB,CntC,CntD;
	static bool WeekdaySWFlg = 0;
	static uint8_t WeekdaySWTime = 0;
	static uint16_t DplySWTime;
	
	for(i=0;i<16;i++) { DplyData.Buffer[i] = 0x00; }
	
	if(++WeekdaySWTime >= lcd.weekdaySWCycle) { WeekdaySWTime=0; WeekdaySWFlg = !WeekdaySWFlg; }
	
	if(++DplySWTime>=5) { DplySWTime=0; lcd.displaySWFlg = !lcd.displaySWFlg; }

  if(key.lockStatus) {KEY_LOCK_CLOSE_SEG_ON;}
	
	//Line
	LINE_SEG1_ON; 
	
	//bluetooth
//	if(lcd.bleStatus == BLE_STATUS_ADVERTISING)
//	{
////		if(CntD < 5) { BLUETOOTH_SEG_ON; }
////		if(++CntD > 15) CntD = 0;
//		CntD = 0;
//	}
//	else 
		if(lcd.bleStatus)
	{
		CntD = 0;
		BLUETOOTH_SEG_ON;
	}
	
	if(globalWorkState == FULL_WORKING)
	{
		if((aroma.en==ON) && (event.status))
		{
			//"ON"
			ON_SEG_ON;
			if(airpump.SW)
			{
				//"AIRPUMP"
				if(CntA < 1){AIRPUMP_SEG1_ON;}
				else if(CntA < 2){AIRPUMP_SEG1_ON;AIRPUMP_SEG2_ON;}
				else if(CntA < 3){AIRPUMP_SEG1_ON;AIRPUMP_SEG2_ON;AIRPUMP_SEG3_ON;}
				else {AIRPUMP_SEG1_ON;AIRPUMP_SEG2_ON;AIRPUMP_SEG3_ON;AIRPUMP_SEG4_ON;}
				if(++CntA > 4) CntA = 0;				
				
				//"WORK"
				WORK_SEG_ON;
			}
			else
			{
				//"PAUSE"
				PAUSE_SEG_ON;
				//"AIRPUMP"
				AIRPUMP_SEG1_ON;
				CntA = 0;
			}
		}
		else
		{
			//"OFF"
			OFF_SEG_ON;
			//"AIRPUMP"
			AIRPUMP_SEG1_ON;
			CntA = 0;
		}
		
		if((function.fan) && (fan.en))
		{
			//"FAN"
			if(fan.SW == ON)
			{
				if(CntB < 3) {FAN_SEG_ON;}
				else {FAN_SEG_OFF;}
				if(++CntB > 6) CntB = 0;
			}
			else
			{
				FAN_SEG_ON;
				CntB = 0;
			}
		}
		else
		{
			CntB = 0;
			FAN_SEG_OFF;
		}
		
		
		TimeHS_ON(RTC_TimeStructure.Hours/10);
		TimeHG_ON(RTC_TimeStructure.Hours%10);
		if(lcd.displaySWFlg) {SPOT_SEG_ON;}
		TimeMS_ON(RTC_TimeStructure.Minutes/10);
		TimeMG_ON(RTC_TimeStructure.Minutes%10);
		
		if(event.status)  
		{
			if(runEvent[event.num].weekEn.MON_EN) {MON_SEG_ON;MON_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.TUE_EN) {TUE_SEG_ON;TUE_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.WED_EN) {WED_SEG_ON;WED_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.THU_EN) {THU_SEG_ON;THU_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.FRI_EN) {FRI_SEG_ON;FRI_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.SAT_EN) {SAT_SEG_ON;SAT_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.SUN_EN) {SUN_SEG_ON;SUN_SEG_ARROW_ON;}
		}
		lcd.weekdaySWCycle = 5;
		
		switch(RTC_DateStructure.WeekDay)
		{
			case CLOCK_MON:{MON_SEG_ON;MON_SEG_ARROW_ON;}break;
			case CLOCK_TUE:{TUE_SEG_ON;TUE_SEG_ARROW_ON;}break;
			case CLOCK_WED:{WED_SEG_ON;WED_SEG_ARROW_ON;}break;
			case CLOCK_THU:{THU_SEG_ON;THU_SEG_ARROW_ON;}break;
			case CLOCK_FRI:{FRI_SEG_ON;FRI_SEG_ARROW_ON;}break;
			case CLOCK_SAT:{SAT_SEG_ON;SAT_SEG_ARROW_ON;}break;
			case CLOCK_SUN:{SUN_SEG_ON;SUN_SEG_ARROW_ON;}break;
		}
		
		if(event.status)
		{
			//EVENT_NUM
			if(aroma.concentration) { EVENT_NUM_ON(aroma.concentration);}//(event.num+1);
		}		
		
	}
	else if(globalWorkState == SET_CLOCK)
	{
		if(++set.clockTime>50)
		{
			set.clockTime = 0;
			set.clockSeg = 0;
			globalWorkState = FULL_WORKING;
		}
		
		if(key.type.BYTE == 0x07)
		{
			switch(set.clockSeg)
			{
				case 0:
				case 1:
				case 2:
				{
					TimeHS_ON(RTC_TimeStructure.Hours/10);//if((Calendar.Current.Hour/10)!=0){ TimeHS_ON(Calendar.Current.Hour/10); }
					TimeHG_ON(RTC_TimeStructure.Hours%10);
					if(lcd.displaySWFlg) {SPOT_SEG_ON;}
					TimeMS_ON(RTC_TimeStructure.Minutes/10);
					TimeMG_ON(RTC_TimeStructure.Minutes%10);
					switch(RTC_DateStructure.WeekDay)
					{
						case CLOCK_MON:{MON_SEG_ON;}break;
						case CLOCK_TUE:{TUE_SEG_ON;}break;
						case CLOCK_WED:{WED_SEG_ON;}break;
						case CLOCK_THU:{THU_SEG_ON;}break;
						case CLOCK_FRI:{FRI_SEG_ON;}break;
						case CLOCK_SAT:{SAT_SEG_ON;}break;
						case CLOCK_SUN:{SUN_SEG_ON;}break;
					}
				}break;
//				case 3:
//				{
//					if(oil.totalVolume/1000) {TimeHS_ON(oil.totalVolume/1000);}
//					if((oil.totalVolume/1000) || (oil.totalVolume%1000/100)) {TimeHG_ON(oil.totalVolume%1000/100);}
//					if((oil.totalVolume/1000) || (oil.totalVolume%1000/100) || (oil.totalVolume%100/10)) {TimeMS_ON(oil.totalVolume%100/10);}
//					TimeMG_ON(oil.totalVolume%10);
//					OIL_VOLUME_SEG_ON;
//				}break;
//				case 4:
//				{
//					if(oil.curretVolume/1000) {TimeHS_ON(oil.curretVolume/1000);}
//					if((oil.curretVolume/1000) || (oil.curretVolume%1000/100)) {TimeHG_ON(oil.curretVolume%1000/100);}
//					if((oil.curretVolume/1000) || (oil.curretVolume%1000/100) || (oil.curretVolume%100/10)) {TimeMS_ON(oil.curretVolume%100/10);}
//					TimeMG_ON(oil.curretVolume%10);
//					OIL_LEVEL_SEG_ON;
//				}break;
//				case 5:
//				{
//					if(oil.consumeSpeed/100) {TimeHS_ON(oil.consumeSpeed/100);}
//					TimeHG_ON(oil.consumeSpeed%100/10);
//					TimeMS_ON(oil.consumeSpeed%10);
//					DROP_SEG_ON;
//					OIL_ML_H_SEG_ON;
//				}break;
			}
			
		}
		else
		{
			switch(set.clockSeg)
			{
				case 0:
				{
					if(lcd.displaySWFlg)
					{
					  TimeHS_ON(RTC_TimeStructure.Hours/10);//if((Calendar.Current.Hour/10)!=0){ TimeHS_ON(Calendar.Current.Hour/10); }
						TimeHG_ON(RTC_TimeStructure.Hours%10);
					}
					SPOT_SEG_ON;
					TimeMS_ON(RTC_TimeStructure.Minutes/10);
					TimeMG_ON(RTC_TimeStructure.Minutes%10);
					
					switch(RTC_DateStructure.WeekDay)
					{
						case CLOCK_MON:{MON_SEG_ON;}break;
						case CLOCK_TUE:{TUE_SEG_ON;}break;
						case CLOCK_WED:{WED_SEG_ON;}break;
						case CLOCK_THU:{THU_SEG_ON;}break;
						case CLOCK_FRI:{FRI_SEG_ON;}break;
						case CLOCK_SAT:{SAT_SEG_ON;}break;
						case CLOCK_SUN:{SUN_SEG_ON;}break;
					}
					
				}break;
				case 1:
				{
					TimeHS_ON(RTC_TimeStructure.Hours/10);//if((Calendar.Current.Hour/10)!=0){ TimeHS_ON(Calendar.Current.Hour/10); }
					TimeHG_ON(RTC_TimeStructure.Hours%10);
					SPOT_SEG_ON;
					if(lcd.displaySWFlg)
					{
						TimeMS_ON(RTC_TimeStructure.Minutes/10);
						TimeMG_ON(RTC_TimeStructure.Minutes%10);
					}
					switch(RTC_DateStructure.WeekDay)
					{
						case CLOCK_MON:{MON_SEG_ON;}break;
						case CLOCK_TUE:{TUE_SEG_ON;}break;
						case CLOCK_WED:{WED_SEG_ON;}break;
						case CLOCK_THU:{THU_SEG_ON;}break;
						case CLOCK_FRI:{FRI_SEG_ON;}break;
						case CLOCK_SAT:{SAT_SEG_ON;}break;
						case CLOCK_SUN:{SUN_SEG_ON;}break;
					}
				}break;
				case 2:
				{
					TimeHS_ON(RTC_TimeStructure.Hours/10);//if((Calendar.Current.Hour/10)!=0){ TimeHS_ON(Calendar.Current.Hour/10); }
					TimeHG_ON(RTC_TimeStructure.Hours%10);
					SPOT_SEG_ON;
					TimeMS_ON(RTC_TimeStructure.Minutes/10);
					TimeMG_ON(RTC_TimeStructure.Minutes%10);
					
					if(lcd.displaySWFlg)
					{
						switch(RTC_DateStructure.WeekDay)
						{
							case CLOCK_MON:{MON_SEG_ON;}break;
							case CLOCK_TUE:{TUE_SEG_ON;}break;
							case CLOCK_WED:{WED_SEG_ON;}break;
							case CLOCK_THU:{THU_SEG_ON;}break;
							case CLOCK_FRI:{FRI_SEG_ON;}break;
							case CLOCK_SAT:{SAT_SEG_ON;}break;
							case CLOCK_SUN:{SUN_SEG_ON;}break;
						}
					}
				}break;
//				case 3:
//				{
//					if(lcd.displaySWFlg)
//					{
//						if(oil.totalVolume/1000) {TimeHS_ON(oil.totalVolume/1000);}
//						if((oil.totalVolume/1000) || (oil.totalVolume%1000/100)) {TimeHG_ON(oil.totalVolume%1000/100);}
//						if((oil.totalVolume/1000) || (oil.totalVolume%1000/100) || (oil.totalVolume%100/10)) {TimeMS_ON(oil.totalVolume%100/10);}
//						TimeMG_ON(oil.totalVolume%10);
//					}
//					OIL_VOLUME_SEG_ON;
//				}break;
//				case 4:
//				{
//					if(lcd.displaySWFlg)
//					{
//						if(oil.curretVolume/1000) {TimeHS_ON(oil.curretVolume/1000);}
//						if((oil.curretVolume/1000) || (oil.curretVolume%1000/100)) {TimeHG_ON(oil.curretVolume%1000/100);}
//						if((oil.curretVolume/1000) || (oil.curretVolume%1000/100) || (oil.curretVolume%100/10)) {TimeMS_ON(oil.curretVolume%100/10);}
//						TimeMG_ON(oil.curretVolume%10);
//					}
//					OIL_LEVEL_SEG_ON;
//				}break;
//				case 5:
//				{
//					if(lcd.displaySWFlg)
//					{
//						if(oil.consumeSpeed/100) {TimeHS_ON(oil.consumeSpeed/100);}
//						TimeHG_ON(oil.consumeSpeed%100/10);
//						TimeMS_ON(oil.consumeSpeed%10);
//					}
//					DROP_SEG_ON;
//					OIL_ML_H_SEG_ON;
//				}break;
			}
		}
	}
	else if(globalWorkState == SET_GEAR)
	{
		if(++set.parameterTime>50)
		{
			set.parameterTime = 0;
			set.eventSeg = 0;
			set.weekdayNum = EVENT_MON;
			globalWorkState = FULL_WORKING;
		}
		
		//"WORK"
		WORK_SEG_ON;
		
			if(runEvent[event.num].weekEn.MON_EN) {MON_SEG_ON;MON_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.TUE_EN) {TUE_SEG_ON;TUE_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.WED_EN) {WED_SEG_ON;WED_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.THU_EN) {THU_SEG_ON;THU_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.FRI_EN) {FRI_SEG_ON;FRI_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.SAT_EN) {SAT_SEG_ON;SAT_SEG_ARROW_ON;}
			if(runEvent[event.num].weekEn.SUN_EN) {SUN_SEG_ON;SUN_SEG_ARROW_ON;}
			
		TimeHG_ON(aroma.concentration);
	}	
	else if(globalWorkState == SET_TIMEING)
	{
		if(++set.parameterTime>50)
		{
			set.parameterTime = 0;
			set.eventSeg = 0;
			set.weekdayNum = EVENT_MON;
			globalWorkState = FULL_WORKING;
		}
		//"EVENT"
		EVENT_SEG_ON;
		
		TimeHS_ON(aroma.timeing/60/10);
		TimeHG_ON(aroma.timeing/60%10);
		SPOT_SEG_ON;
		TimeMS_ON(aroma.timeing%60/10);
		TimeMG_ON(aroma.timeing%60%10);
	}
	else if(globalWorkState == MODE_RESET)
	{
		for(i=0;i<16;i++) { DplyData.Buffer[i]=0xff; }
		if(++CntC > 30)
		{
			CntC = 0;
			for(i=0;i<16;i++) { DplyData.Buffer[i]=0x00; }
			globalWorkState = FULL_WORKING;
		}
	}
}

