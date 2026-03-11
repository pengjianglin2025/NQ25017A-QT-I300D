#ifndef _VK1621B_H
#define _VK1621B_H

#include "main.h"


//#define  LCD_CS_HIGH()        PORT_SetBit(PORT1,PIN5)
//#define  LCD_CS_LOW()        PORT_ClrBit(PORT1,PIN5) 
//#define  LCD_WR_HIGH()        PORT_SetBit(PORT1,PIN6)
//#define  LCD_WR_LOW()        PORT_ClrBit(PORT1,PIN6) 
//#define  LCD_DATD_HIGH()       PORT_SetBit(PORT1,PIN7)
//#define  LCD_DATD_LOW()       PORT_ClrBit(PORT1,PIN7) 


extern const uint8_t seg_afed[];
extern const uint8_t seg_bgc[];

void Lcd_Config(void);
void SleepLcd_Config(void);
void Write_1621_RAM(uint8_t addr, uint8_t *buf, uint8_t size);
void Init_1621(void);
void vk1621_init(void);

#define DISPLY_DATA_MAX    16

typedef union
{
	uint8_t Buffer[DISPLY_DATA_MAX];
	struct
	{
		union
		{
			uint8_t BYTE0; 
			struct
			{
				uint8_t S4:	        1; 
				uint8_t S5:	        1; 
				uint8_t S6:	        1; 
				uint8_t S7:	          1; 
				uint8_t S3:	        1;
				uint8_t S2:	        1; 
				uint8_t S1:	        1; 
				uint8_t S8:	        1; 
				 //最高位
			};
		};
		union
		{
			uint8_t BYTE1; 
			struct
			{
				uint8_t S9:	        1; 
				uint8_t S10:	        1; 
				uint8_t S11:	        1; 
				uint8_t S12:	          1; 
				uint8_t S19:	        1;
				uint8_t S18:	        1; 
				uint8_t S17:	        1; 
				uint8_t S16:	        1;  
				//最高位
			};
		};
		union
		{
			uint8_t BYTE2; 
			struct
			{ 
				uint8_t S20:	        1; 
				uint8_t S23:	        1; 
				uint8_t S22:	        1; 
				uint8_t S21:	          1;
				uint8_t AFED_5:	        4; 
				//最高位
			};
		};
		union
		{
			uint8_t BYTE3; 
			struct
			{
				uint8_t S24:	        1; 
				uint8_t BGC_5:	        3; 
				uint8_t AFED_1:	        4; 
				//最高位
			};
		};
		union
		{
			uint8_t BYTE4; 
			struct
			{
				uint8_t BGC_1:	        3; 
				uint8_t S13:	        1; 
				uint8_t AFED_2:	        4; 
				//最高位
			};
		};
		union
		{
			uint8_t BYTE5; 
			struct
			{
				uint8_t BGC_2:	        3; 
				uint8_t COL:	        1; 
				uint8_t AFED_3:	        4; 
				//最高位
			};
		};
		union
		{
			uint8_t BYTE6; 
			struct
			{
				uint8_t BGC_3:	        3; 
				uint8_t S14:	        1;
				uint8_t AFED_4:	        4; 
				//最高位
			};
		};
		union
		{
			uint8_t BYTE7; 
			struct
			{
				uint8_t BGC_4:	        3; 
				uint8_t S15:	        1;
				uint8_t S25:	        1;
				uint8_t S26:	        1;
				uint8_t S27:	        1;
				uint8_t S28:	        1;
				  //最高位
			};
		};
		union
		{
			uint8_t BYTE8; 
			struct
			{
				uint8_t S35:	        1;
				uint8_t S34:	        1; 
				uint8_t S33:	        1; 
				uint8_t Reserverd0:	          1; 
				uint8_t S30:	        1; 
				uint8_t S31:	        1; 
				uint8_t S32:	        1; 
				uint8_t S29:	        1; 
				//最高位
			};
		};
		uint8_t BYTE9;
		uint8_t BYTE10;
		uint8_t BYTE11;
		uint8_t BYTE12;
		uint8_t BYTE13;
		uint8_t BYTE14;
		uint8_t BYTE15;
	};
}Dply_t;
extern Dply_t DplyData;

#define  TimeHS_ON(Num)  DplyData.AFED_1 = seg_afed[Num]; DplyData.BGC_1 = seg_bgc[Num]  //显示中时间的小时的十位
#define  TimeHG_ON(Num)  DplyData.AFED_2 = seg_afed[Num]; DplyData.BGC_2 = seg_bgc[Num]  //显示中时间的小时的个位
#define  TimeMS_ON(Num)  DplyData.AFED_3 = seg_afed[Num]; DplyData.BGC_3 = seg_bgc[Num]
#define  TimeMG_ON(Num)  DplyData.AFED_4 = seg_afed[Num]; DplyData.BGC_4 = seg_bgc[Num]

#define  TimeHS_OFF  DplyData.AFED_1 = 0; DplyData.BGC_1 = 0
#define  TimeHG_OFF  DplyData.AFED_2 = 0; DplyData.BGC_2 = 0
#define  TimeMS_OFF  DplyData.AFED_3 = 0; DplyData.BGC_3 = 0
#define  TimeMG_OFF  DplyData.AFED_4 = 0; DplyData.BGC_4 = 0

#define  EVENT_NUM_ON(Num) DplyData.AFED_5 = seg_afed[Num]; DplyData.BGC_5 = seg_bgc[Num] 
#define  EVENT_NUM_OFF     DplyData.AFED_5 = 0; DplyData.BGC_5 = 0

#define  SPOT_SEG_ON   DplyData.COL = 1   //中间分隔点
#define  SPOT_SEG_OFF  DplyData.COL = 0

//#define  KEY_LOCK_OPEN_SEG_ON    Pixel_Data[14] |= 0x50 //按键锁
#define  KEY_LOCK_CLOSE_SEG_ON   DplyData.S28 = 1   
//#define  KEY_LOCK_OPEN_SEG_OFF   Pixel_Data[14] &= ~0x50
#define  KEY_LOCK_COLSE_SEG_OFF  DplyData.S28 = 0

#define  BLUETOOTH_SEG_ON   DplyData.S27 = 1   //蓝牙图标
#define  BLUETOOTH_SEG_OFF  DplyData.S27 = 0

#define  WIFI_SEG_ON   DplyData.S26 = 1   //WIFI图标
#define  WIFI_SEG_OFF  DplyData.S26 = 0

#define  SIGNAL_SEG_ON   DplyData.S25 = 1   //蜂窝信号图标
#define  SIGNAL_SEG_OFF  DplyData.S25 = 0

//#define  OIL_QUANTITY1_SEG_ON   Pixel_Data[14] |= 0x80   //油量显示
//#define  OIL_QUANTITY2_SEG_ON   Pixel_Data[14] |= 0x88 
//#define  OIL_QUANTITY3_SEG_ON   Pixel_Data[14] |= 0x88; Pixel_Data[13] |= 0x80
//#define  OIL_QUANTITY_SEG_OFF  Pixel_Data[13] &= ~0x80;Pixel_Data[14] &= ~0x88

#define  LINE_SEG1_ON   DplyData.S8 = 1

#define  LINE_SEG1_OFF   DplyData.S8 = 0

#define  MON_SEG_ON   DplyData.S1 = 1
#define  TUE_SEG_ON   DplyData.S2 = 1 
#define  WED_SEG_ON   DplyData.S3 = 1 
#define  THU_SEG_ON   DplyData.S4 = 1
#define  FRI_SEG_ON   DplyData.S5 = 1 
#define  SAT_SEG_ON   DplyData.S6 = 1
#define  SUN_SEG_ON   DplyData.S7 = 1

#define  MON_SEG_OFF   DplyData.S1 = 0
#define  TUE_SEG_OFF   DplyData.S2 = 0 
#define  WED_SEG_OFF   DplyData.S3 = 0 
#define  THU_SEG_OFF   DplyData.S4 = 0
#define  FRI_SEG_OFF   DplyData.S5 = 0 
#define  SAT_SEG_OFF   DplyData.S6 = 0
#define  SUN_SEG_OFF   DplyData.S7 = 0

//星期图标下箭头
#define  MON_SEG_ARROW_ON   DplyData.S9 = 1
#define  TUE_SEG_ARROW_ON   DplyData.S10 = 1 
#define  WED_SEG_ARROW_ON   DplyData.S11 = 1 
#define  THU_SEG_ARROW_ON   DplyData.S12 = 1
#define  FRI_SEG_ARROW_ON   DplyData.S13 = 1 
#define  SAT_SEG_ARROW_ON   DplyData.S14 = 1
#define  SUN_SEG_ARROW_ON   DplyData.S15 = 1

#define  MON_SEG_ARROW_OFF   DplyData.S9 = 0
#define  TUE_SEG_ARROW_OFF   DplyData.S10 = 0 
#define  WED_SEG_ARROW_OFF   DplyData.S11 = 0 
#define  THU_SEG_ARROW_OFF   DplyData.S12 = 0
#define  FRI_SEG_ARROW_OFF   DplyData.S13 = 0 
#define  SAT_SEG_ARROW_OFF   DplyData.S14 = 0
#define  SUN_SEG_ARROW_OFF   DplyData.S15 = 0


#define  WORK_SEG_ON      DplyData.S16 = 1
#define  PAUSE_SEG_ON     DplyData.S17 = 1
#define  EVENT_SEG_ON     DplyData.S18 = 1
#define  ON_SEG_ON        DplyData.S19 = 1
#define  OFF_SEG_ON       DplyData.S20 = 1
//#define  PER_SEG_ON       DplyData.X1 = 1
#define  FAN_SEG_ON       DplyData.S29 = 1
#define  AIRPUMP_SEG1_ON  DplyData.S24 = 1
#define  AIRPUMP_SEG2_ON  DplyData.S23 = 1
#define  AIRPUMP_SEG3_ON  DplyData.S22 = 1
#define  AIRPUMP_SEG4_ON  DplyData.S21 = 1


//#define  PASSWORD_SEG_OFF  DplyData.X1 = 0
#define  WORK_SEG_OFF      DplyData.S16 = 0
#define  PAUSE_SEG_OFF     DplyData.S17 = 0
#define  EVENT_SEG_OFF     DplyData.S18 = 0
#define  ON_SEG_OFF        DplyData.S19 = 0
#define  OFF_SEG_OFF       DplyData.S20 = 0
//#define  PER_SEG_OFF       DplyData.X1 = 0
#define  FAN_SEG_OFF       DplyData.S29 = 0
#define  AIRPUMP_SEG1_OFF  DplyData.S24 = 0
#define  AIRPUMP_SEG2_OFF  DplyData.S23 = 0
#define  AIRPUMP_SEG3_OFF  DplyData.S22 = 0
#define  AIRPUMP_SEG4_OFF  DplyData.S21 = 0

#define B0000_0000 0x00
#define B0000_0001 0x01
#define B0000_0010 0x02
#define B0000_0011 0x03
#define B0000_0100 0x04
#define B0000_0101 0x05
#define B0000_0110 0x06
#define B0000_0111 0x07
#define B0000_1000 0x08
#define B0000_1001 0x09
#define B0000_1010 0x0A
#define B0000_1011 0x0B
#define B0000_1100 0x0C
#define B0000_1101 0x0D
#define B0000_1110 0x0E
#define B0000_1111 0x0F

#define B0001_0000 0x10
#define B0001_0001 0x11
#define B0001_0010 0x12
#define B0001_0011 0x13
#define B0001_0100 0x14
#define B0001_0101 0x15
#define B0001_0110 0x16
#define B0001_0111 0x17
#define B0001_1000 0x18
#define B0001_1001 0x19
#define B0001_1010 0x1A
#define B0001_1011 0x1B
#define B0001_1100 0x1C
#define B0001_1101 0x1D
#define B0001_1110 0x1E
#define B0001_1111 0x1F

#define B0010_0000 0x20
#define B0010_0001 0x21
#define B0010_0010 0x22
#define B0010_0011 0x23
#define B0010_0100 0x24
#define B0010_0101 0x25
#define B0010_0110 0x26
#define B0010_0111 0x27
#define B0010_1000 0x28
#define B0010_1001 0x29
#define B0010_1010 0x2A
#define B0010_1011 0x2B
#define B0010_1100 0x2C
#define B0010_1101 0x2D
#define B0010_1110 0x2E
#define B0010_1111 0x2F

#define B0011_0000 0x30
#define B0011_0001 0x31
#define B0011_0010 0x32
#define B0011_0011 0x33
#define B0011_0100 0x34
#define B0011_0101 0x35
#define B0011_0110 0x36
#define B0011_0111 0x37
#define B0011_1000 0x38
#define B0011_1001 0x39
#define B0011_1010 0x3A
#define B0011_1011 0x3B
#define B0011_1100 0x3C
#define B0011_1101 0x3D
#define B0011_1110 0x3E
#define B0011_1111 0x3F

#define B0100_0000 0x40
#define B0100_0001 0x41
#define B0100_0010 0x42
#define B0100_0011 0x43
#define B0100_0100 0x44
#define B0100_0101 0x45
#define B0100_0110 0x46
#define B0100_0111 0x47
#define B0100_1000 0x48
#define B0100_1001 0x49
#define B0100_1010 0x4A
#define B0100_1011 0x4B
#define B0100_1100 0x4C
#define B0100_1101 0x4D
#define B0100_1110 0x4E
#define B0100_1111 0x4F

#define B0101_0000 0x50
#define B0101_0001 0x51
#define B0101_0010 0x52
#define B0101_0011 0x53
#define B0101_0100 0x54
#define B0101_0101 0x55
#define B0101_0110 0x56
#define B0101_0111 0x57
#define B0101_1000 0x58
#define B0101_1001 0x59
#define B0101_1010 0x5A
#define B0101_1011 0x5B
#define B0101_1100 0x5C
#define B0101_1101 0x5D
#define B0101_1110 0x5E
#define B0101_1111 0x5F

#define B0110_0000 0x60
#define B0110_0001 0x61
#define B0110_0010 0x62
#define B0110_0011 0x63
#define B0110_0100 0x64
#define B0110_0101 0x65
#define B0110_0110 0x66
#define B0110_0111 0x67
#define B0110_1000 0x68
#define B0110_1001 0x69
#define B0110_1010 0x6A
#define B0110_1011 0x6B
#define B0110_1100 0x6C
#define B0110_1101 0x6D
#define B0110_1110 0x6E
#define B0110_1111 0x6F

#define B0111_0000 0x70
#define B0111_0001 0x71
#define B0111_0010 0x72
#define B0111_0011 0x73
#define B0111_0100 0x74
#define B0111_0101 0x75
#define B0111_0110 0x76
#define B0111_0111 0x77
#define B0111_1000 0x78
#define B0111_1001 0x79
#define B0111_1010 0x7A
#define B0111_1011 0x7B
#define B0111_1100 0x7C
#define B0111_1101 0x7D
#define B0111_1110 0x7E
#define B0111_1111 0x7F

#define B1000_0000 0x80
#define B1000_0001 0x81
#define B1000_0010 0x82
#define B1000_0011 0x83
#define B1000_0100 0x84
#define B1000_0101 0x85
#define B1000_0110 0x86
#define B1000_0111 0x87
#define B1000_1000 0x88
#define B1000_1001 0x89
#define B1000_1010 0x8A
#define B1000_1011 0x8B
#define B1000_1100 0x8C
#define B1000_1101 0x8D
#define B1000_1110 0x8E
#define B1000_1111 0x8F

#define B1001_0000 0x90
#define B1001_0001 0x91
#define B1001_0010 0x92
#define B1001_0011 0x93
#define B1001_0100 0x94
#define B1001_0101 0x95
#define B1001_0110 0x96
#define B1001_0111 0x97
#define B1001_1000 0x98
#define B1001_1001 0x99
#define B1001_1010 0x9A
#define B1001_1011 0x9B
#define B1001_1100 0x9C
#define B1001_1101 0x9D
#define B1001_1110 0x9E
#define B1001_1111 0x9F

#define B1010_0000 0xA0
#define B1010_0001 0xA1
#define B1010_0010 0xA2
#define B1010_0011 0xA3
#define B1010_0100 0xA4
#define B1010_0101 0xA5
#define B1010_0110 0xA6
#define B1010_0111 0xA7
#define B1010_1000 0xA8
#define B1010_1001 0xA9
#define B1010_1010 0xAA
#define B1010_1011 0xAB
#define B1010_1100 0xAC
#define B1010_1101 0xAD
#define B1010_1110 0xAE
#define B1010_1111 0xAF

#define B1011_0000 0xB0
#define B1011_0001 0xB1
#define B1011_0010 0xB2
#define B1011_0011 0xB3
#define B1011_0100 0xB4
#define B1011_0101 0xB5
#define B1011_0110 0xB6
#define B1011_0111 0xB7
#define B1011_1000 0xB8
#define B1011_1001 0xB9
#define B1011_1010 0xBA
#define B1011_1011 0xBB
#define B1011_1100 0xBC
#define B1011_1101 0xBD
#define B1011_1110 0xBE
#define B1011_1111 0xBF

#define B1100_0000 0xC0
#define B1100_0001 0xC1
#define B1100_0010 0xC2
#define B1100_0011 0xC3
#define B1100_0100 0xC4
#define B1100_0101 0xC5
#define B1100_0110 0xC6
#define B1100_0111 0xC7
#define B1100_1000 0xC8
#define B1100_1001 0xC9
#define B1100_1010 0xCA
#define B1100_1011 0xCB
#define B1100_1100 0xCC
#define B1100_1101 0xCD
#define B1100_1110 0xCE
#define B1100_1111 0xCF

#define B1101_0000 0xD0
#define B1101_0001 0xD1
#define B1101_0010 0xD2
#define B1101_0011 0xD3
#define B1101_0100 0xD4
#define B1101_0101 0xD5
#define B1101_0110 0xD6
#define B1101_0111 0xD7
#define B1101_1000 0xD8
#define B1101_1001 0xD9
#define B1101_1010 0xDA
#define B1101_1011 0xDB
#define B1101_1100 0xDC
#define B1101_1101 0xDD
#define B1101_1110 0xDE
#define B1101_1111 0xDF

#define B1110_0000 0xE0
#define B1110_0001 0xE1
#define B1110_0010 0xE2
#define B1110_0011 0xE3
#define B1110_0100 0xE4
#define B1110_0101 0xE5
#define B1110_0110 0xE6
#define B1110_0111 0xE7
#define B1110_1000 0xE8
#define B1110_1001 0xE9
#define B1110_1010 0xEA
#define B1110_1011 0xEB
#define B1110_1100 0xEC
#define B1110_1101 0xED
#define B1110_1110 0xEE
#define B1110_1111 0xEF

#define B1111_0000 0xF0
#define B1111_0001 0xF1
#define B1111_0010 0xF2
#define B1111_0011 0xF3
#define B1111_0100 0xF4
#define B1111_0101 0xF5
#define B1111_0110 0xF6
#define B1111_0111 0xF7
#define B1111_1000 0xF8
#define B1111_1001 0xF9
#define B1111_1010 0xFA
#define B1111_1011 0xFB
#define B1111_1100 0xFC
#define B1111_1101 0xFD
#define B1111_1110 0xFE
#define B1111_1111 0xFF 

#endif
