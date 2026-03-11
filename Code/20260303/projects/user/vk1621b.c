#include "vk1621b.h"

Dply_t DplyData;


const uint8_t seg_bgc[] = {0x05/*0*/,0x05/*1*/,0x06/*2*/,0x07/*3*/,0x07/*4*/,0x03/*5*/,0x03/*6*/,0x05/*7*/,0x07/*8*/,0x07/*9*/,0x02/*-*/,0x00/*不显*/};
const uint8_t seg_afed[] = {0x0f/*0*/,0x00/*1*/,0x0b/*2*/,0x09/*3*/,0x04/*4*/,0x0d/*5*/,0x0f/*6*/,0x08/*7*/,0x0f/*8*/,0x0d/*9*/,0x00/*-*/,0x00/*不显*/};

void vk1621_init(void)
{
	uint8_t i;

	Init_1621();
	for(i=0;i<16;i++)
	{
		DplyData.Buffer[i]=0x00;
	}
	delay_n_ms(1);
}

void Write_1621_RAM(uint8_t addr, uint8_t *buf, uint8_t size)
{
	uint8_t i,j;
	uint8_t tmp;

	LCD_CS_LOW();
  // CMD - 101
  tmp = B0000_0101;

  for (i=3; i>0; --i)
	{
		delay_n_10us(1);
    LCD_WR_LOW();
		delay_n_10us(1);
		if(tmp & 0x04)
		{
			LCD_DATD_HIGH();
		}
		else
		{
			LCD_DATD_LOW();
		}
		delay_n_10us(1);
    LCD_WR_HIGH();
		tmp <<= 1;
  }

  // Addr - 0x00
  tmp = addr & B0011_1111;

  for (i=6; i>0; --i)
	{
		delay_n_10us(1);
    LCD_WR_LOW();
		delay_n_10us(1);
		if(tmp & 0x20)
		{
			LCD_DATD_HIGH();
		}
		else
		{
			LCD_DATD_LOW();
		}
		delay_n_10us(1);
    LCD_WR_HIGH();
		tmp <<= 1;
  }

  // data - successive 32 bytes
  for (j=0; j<size; j++)
	{
    tmp = buf[j];
    for (i=8; i>0; --i)
		{
		delay_n_10us(1);
      LCD_WR_LOW();
		delay_n_10us(1);
			if(tmp & 0x01)
			{
				LCD_DATD_HIGH();
			}
			else
			{
				LCD_DATD_LOW();
			}
		delay_n_10us(1);
			LCD_WR_HIGH();
			tmp >>= 1;
    }
  }
	LCD_CS_HIGH();
}

static void Write_1621_COMMAND(uint8_t config)
{
	uint8_t i;
	uint8_t cmd;

  LCD_CS_LOW();

  cmd = B0000_0100;

  for (i=3; i>0; --i)
	{
		delay_n_10us(1);
		LCD_WR_LOW();
		delay_n_10us(1);
		if(cmd & 0x04)
		{
			LCD_DATD_HIGH();
		}
		else
		{
			LCD_DATD_LOW();
		}
		delay_n_10us(1);
    LCD_WR_HIGH();
		cmd <<= 1;
  }

  // config
  for (i=9; i>0; --i)
	{
		delay_n_10us(1);
		LCD_WR_LOW();
		delay_n_10us(1);
		if(config & 0x80)
		{
			LCD_DATD_HIGH();
		}
		else
		{
			LCD_DATD_LOW();
		}
		delay_n_10us(1);
    LCD_WR_HIGH();
		config <<= 1;
  }
	LCD_CS_HIGH();
}

void Init_1621(void)
{
  LCD_CS_HIGH();
	LCD_WR_HIGH();
	LCD_DATD_HIGH();
	delay_n_ms(1);
	Write_1621_COMMAND(B0000_0000);    // Disable System
	Write_1621_COMMAND(B0000_0101);    // 关闭看门狗
  Write_1621_COMMAND(B0010_1001);    // 4 comm
  Write_1621_COMMAND(B0000_0011);    // Turn On LCD
  Write_1621_COMMAND(B0000_0001);    // Enable System
}

void TurnOn_1621(uint8_t bOnoff)
{
  if (bOnoff)
	{
    Write_1621_COMMAND(B0000_0011);  // bias generator
		Write_1621_COMMAND(B0000_0001);    // Enable System
		
  }
  else 
	{
    Write_1621_COMMAND(B0000_0010);  // bias generator
		Write_1621_COMMAND(B0000_0000);    // Disable System
  }
} 


/*******************************************
* Function: void Lcd_Config(void)*
* Description: LCD配置*
* Parameter:  *
* Return: *
*******************************************/
void SleepLcd_Config(void)
{
	
	TurnOn_1621(OFF);
	lcd.backOnTime = 0;
	LCD_BACKLIGHT_OFF();
	
//	LCD_CS_HIGH();
//	LCD_WR_HIGH();
//	LCD_DATD_HIGH();
	LCD_CS_LOW();
	LCD_WR_LOW();
	LCD_DATD_LOW();
//	PORT_Init(PORT1,PIN5,INPUT);
//	PORT_Init(PORT1,PIN6,INPUT);
//	PORT_Init(PORT1,PIN7,INPUT);
}
