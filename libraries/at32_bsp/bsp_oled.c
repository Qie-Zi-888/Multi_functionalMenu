
#include "bsp_oled.h"
#include "bsp_oledfont.h"
#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"
#include "at32f435_437_gpio.h"

void spitransferByte(char data)
{
	spi_i2s_data_transmit(SPI3, data);
	while(spi_i2s_flag_get(SPI3, SPI_I2S_BF_FLAG) == SET);
	
}

#define OLED_DC_Set() gpio_bits_write(GPIOD, GPIO_PINS_6, TRUE)
#define OLED_DC_Clr() gpio_bits_write(GPIOD, GPIO_PINS_6, FALSE)
#define OLED_CS_Set() gpio_bits_write(GPIOD, GPIO_PINS_3, TRUE)
#define OLED_CS_Clr() gpio_bits_write(GPIOD, GPIO_PINS_3, FALSE)
/*
#define OLED_CS_Set() gpio_bits_write(GPIOC, GPIO_PINS_3, TRUE)
#define OLED_CS_Clr() gpio_bits_write(GPIOC, GPIO_PINS_3, FALSE)
#define OLED_DC_Set() gpio_bits_write(GPIOB, GPIO_PINS_1, TRUE)
#define OLED_DC_Clr() gpio_bits_write(GPIOB, GPIO_PINS_1, FALSE)*/
#define OLED_RST_Set() gpio_bits_write(GPIOE, GPIO_PINS_6, TRUE)
#define OLED_RST_Clr() gpio_bits_write(GPIOE, GPIO_PINS_6, FALSE)

void oledWriteByte(u8 dat, u8 cmd)
{
	if (cmd)
	{
		OLED_DC_Set();
	}
	else if (cmd != 1)
	{
		OLED_DC_Clr();
	}
	OLED_CS_Clr();
	spitransferByte(dat);
	OLED_CS_Set();
	OLED_DC_Set();
}

void oledSetPos(unsigned char x, unsigned char y)
{
	oledWriteByte(0xb0 + y, OLED_CMD);
	oledWriteByte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	oledWriteByte((x & 0x0f), OLED_CMD);
}

void oledDisplayOn(void)
{
	oledWriteByte(0X8D, OLED_CMD); // SET DCDC
	oledWriteByte(0X14, OLED_CMD); // DCDC ON
	oledWriteByte(0XAF, OLED_CMD); // DISPLAY ON
}

void oledDisplayOff(void)
{
	oledWriteByte(0X8D, OLED_CMD); // SET DCDC
	oledWriteByte(0X10, OLED_CMD); // DCDC OFF
	oledWriteByte(0XAE, OLED_CMD); // DISPLAY OFF
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	
void oledClear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		oledWriteByte(0xb0 + i, OLED_CMD); //设置页地址（0~7）
		oledWriteByte(0x00, OLED_CMD);	  //设置显示位置—列低地址
		oledWriteByte(0x10, OLED_CMD);	  //设置显示位置—列高地址   
		for (n = 0; n < 128; n++)
			oledWriteByte(0x00, OLED_DATA);
	}//更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~8
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/8

void oledShowChar(u8 x, u8 y, u8 chr)
{
	unsigned char c = 0, i = 0;
	c = chr - ' '; //得到偏移后的值	
	if (x > Max_Column - 1)
	{
		x = 0;
		y = y + 2;
	}
	if (SIZE == 16)
	{
		oledSetPos(x, y);
		for (i = 0; i < 8; i++)
			oledWriteByte(F8X16[c * 16 + i], OLED_DATA);
		oledSetPos(x, y + 1);
		for (i = 0; i < 8; i++)
			oledWriteByte(F8X16[c * 16 + i + 8], OLED_DATA);
	}
	else
	{
		oledSetPos(x, y + 1);
		for (i = 0; i < 6; i++){
		   // oledSetPos(x+i, y + 1);
			oledWriteByte(F6x8[c][i], OLED_DATA);
		}
	}

}
//m^n函数
u32 oled_pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	

void oledShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				oledShowChar(x + (size2 / 2) * t, y, ' ');
				oledShowChar(x + (size2 / 2) * t+2, y, ' ');
				continue;
			}
			else
				enshow = 1;
		}
		oledShowChar(x + (size2 / 2) * t, y, temp + '0');
	}
}

void oledShowString(u8 x, u8 y, u8 *chr)//(0,0."hello world")
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		oledShowChar(x, y, chr[j]);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}


void oledShowCHinese(u8 x, u8 y, u8 no)
{
	u8 t, adder = 0;
	oledSetPos(x, y);
	for (t = 0; t < 16; t++)
	{
		oledWriteByte(Hzk[2 * no][t], OLED_DATA);
		adder += 1;
	}
	oledSetPos(x, y + 1);
	for (t = 0; t < 16; t++)
	{
		oledWriteByte(Hzk[2 * no + 1][t], OLED_DATA);
		adder += 1;
	}
}

//oledShowCHStr(0,0,"我们嵌入式");
void oledShowCHStr(unsigned char X , unsigned char Y, unsigned char *pstr)
{
	unsigned char i;
	unsigned char Addr = 0;
	unsigned char count = 0;

	while(*pstr)
	{
		for(i=0; i<chinese_12_len; i++)    //在字库中根据汉字GBK内码寻找汉字的字模
		{
			if((*pstr == chinese_12[i].value[0])&&(*(pstr+1) == chinese_12[i].value[1]))
			{
				Addr = i;                      //Addr 汉字在字库中的下标
				break;
			}				
		}		
		oledSetPos(X+count*12,Y);                             //设置初始位置
		for (i=0; i<12; i++)              //写上半部分
		{ 	
			oledWriteByte(chinese_12[Addr].data[i],1);
		}
		oledSetPos(X+count*12,Y+1);    
		for (i=0; i<12; i++)              //写一个汉字
		{ 	
			oledWriteByte(chinese_12[Addr].data[12+i],1);
		}
		count++;                          //已显示的字数	
		pstr++;
		pstr++;  	
	}	  
}


/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void oledDrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		oledSetPos(x0, y);
		for (x = x0; x < x1; x++)
		{
			oledWriteByte(BMP[j++], OLED_DATA);
		}
	}
}


 void SPI_OLED_Init(void);

void oledInit(void)
{
  SPI_OLED_Init();
  OLED_RST_Set();
	delay_ms(10);
	OLED_RST_Clr();
	delay_ms(10);
	OLED_RST_Set();
	
	oledWriteByte(0xAE, OLED_CMD); //--turn off oled panel
	oledWriteByte(0x00, OLED_CMD); //---set low column address
	oledWriteByte(0x10, OLED_CMD); //---set high column address
	oledWriteByte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	oledWriteByte(0x81, OLED_CMD); //--set contrast control register
	oledWriteByte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
	oledWriteByte(0xA1, OLED_CMD); //--Set SEG/Column Mapping      0xa0左右反置 0xa1正常
	oledWriteByte(0xC8, OLED_CMD); // Set COM/Row Scan Direction    0xc0上下反置 0xc8正常
	oledWriteByte(0xA6, OLED_CMD); //--set normal display
	oledWriteByte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
	oledWriteByte(0x3f, OLED_CMD); //--1/64 duty
	oledWriteByte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	oledWriteByte(0x00, OLED_CMD); //-not offset
	oledWriteByte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
	oledWriteByte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
	oledWriteByte(0xD9, OLED_CMD); //--set pre-charge period
	oledWriteByte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	oledWriteByte(0xDA, OLED_CMD); //--set com pins hardware configuration
	oledWriteByte(0x12, OLED_CMD);
	oledWriteByte(0xDB, OLED_CMD); //--set vcomh
	oledWriteByte(0x40, OLED_CMD); // Set VCOM Deselect Level
	oledWriteByte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
	oledWriteByte(0x02, OLED_CMD); //
	oledWriteByte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
	oledWriteByte(0x14, OLED_CMD); //--set(0x10) disable
	oledWriteByte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
	oledWriteByte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
	oledWriteByte(0xAF, OLED_CMD); //--turn on oled panel

	oledWriteByte(0xAF, OLED_CMD); /*display ON*/
	oledClear();
	oledSetPos(0, 0);
}
/**
  * @brief  spi configuration.
  * @param  none
  * @retval none
  */
static void spi_config(void)
{
  spi_init_type spi_init_struct;
  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
	
  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_32;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI3, &spi_init_struct);

  spi_enable(SPI3, TRUE);
}

/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void oled_gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
	
  gpio_default_para_init(&gpio_initstructure);
  /* spi3 sck pin  PB 3*/ 
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_3;
  gpio_init(GPIOB, &gpio_initstructure);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE3, GPIO_MUX_6);


  /* spi3 mosi pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_pins           = GPIO_PINS_5;
  gpio_init(GPIOB, &gpio_initstructure);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE5, GPIO_MUX_6);


  /* set default parameter */
  gpio_default_para_init(&gpio_initstructure);

  /* configure the led gpio */
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_3;
  gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOD, &gpio_initstructure);
	
	gpio_initstructure.gpio_pins = GPIO_PINS_6;
	gpio_init(GPIOD, &gpio_initstructure);

}


 void SPI_OLED_Init(void)
 {
 
    oled_gpio_config();
    spi_config();
    OLED_CS_Clr();
    OLED_RST_Set();
    delay_ms(10);
    OLED_RST_Clr();
    delay_ms(10);
    OLED_RST_Set();
 }
