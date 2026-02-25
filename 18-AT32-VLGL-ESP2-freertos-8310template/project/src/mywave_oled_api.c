#include "mywave_oled_api.h"
#include "codetab.h"
#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"
#include "at32f435_437_gpio.h"
#define OLED_CMD  0	//
#define OLED_DATA 1	//
#define OLED_MODE 0

//OLED模式设置
//0:4线串行模式
//1:并行8080模式

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    
#define WriteCmd(x) oledWriteByte(x,OLED_CMD)
#define WriteDat(x) oledWriteByte(x,OLED_DATA)
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

uint8_t Bef[3];//保存前一个数据的几个参数1.要写在第几页2.0x01要移动几位3.写什么数据
uint8_t Cur[3];//当前前一个数据1.要写在第几页2.0x01要移动几位3.写什么数据
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
 void spitransferByte(char data)
{
	spi_i2s_data_transmit(SPI3, data);
	while(spi_i2s_flag_get(SPI3, SPI_I2S_BF_FLAG) == SET);
	
}

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



void OLED_Init(void)
{
	SPI_OLED_Init();
	OLED_RST_Set();
	delay_ms(10);
	OLED_RST_Clr();
	delay_ms(10);
	OLED_RST_Set();
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //亮度调节 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd(0xb0+x);
	WriteCmd((y&0x0f)|0x00);//LOW
	WriteCmd(((y&0xf0)>>4)|0x10);//HIGHT
}

void OLED_Fill(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}

void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				//OLED_SetPos(x,y);
				OLED_SetPos(y,x);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				//OLED_SetPos(x,y);
				OLED_SetPos(y,x);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				//OLED_SetPos(x,y+1);
				OLED_SetPos(y+1,x);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
	// Description    : 显示codetab.h中的汉字,16*16点阵
	unsigned char wm=0;
	unsigned int  adder=32*N;
	//OLED_SetPos(x , y);
	OLED_SetPos(y,x);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	//OLED_SetPos(x,y + 1);
	OLED_SetPos(y+1,x);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
	// Description    : 显示BMP位图
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		//OLED_SetPos(x0,y);
		OLED_SetPos(y,x0);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}

////////由B站Up主“尔等小众”开源////////////////
///////////////////////////////////////打波函数//////////////////////
void Before_State_Update(uint8_t y)//根据y的值，求出前一个数据的有关参数
{
	Bef[0]=7-y/8;
	Bef[1]=7-y%8;
	Bef[2]=1<<Bef[1];
}
void Current_State_Update(uint8_t y)//根据Y值，求出当前数据的有关参数
{
	Cur[0]=7-y/8;//数据写在第几页
	Cur[1]=7-y%8;//0x01要移动的位数
	Cur[2]=1<<Cur[1];//要写什么数据
}

	
void OLED_DrawWave(uint8_t x,uint8_t y)
{

	int8_t page_sub;
	uint8_t page_buff,i,j;
	Current_State_Update(y);//根据Y值，求出当前数据的有关参数
	page_sub=Bef[0]-Cur[0];//当前值与前一个值的页数相比较
	//确定当前列，每一页应该写什么数据
	if(page_sub>0)
	{
		page_buff=Bef[0];
		OLED_SetPos(page_buff,x);
		WriteDat(Bef[2]-0x01);
		page_buff--;
		for(i=0;i<page_sub-1;i++)
		{
			OLED_SetPos(page_buff,x);
			WriteDat(0xff);
			page_buff--;
		}
		OLED_SetPos(page_buff,x);
		WriteDat(0xff<<Cur[1]);
	}
	else if(page_sub==0)
	{
		if(Cur[1]==Bef[1])
		{
			OLED_SetPos(Cur[0],x);
			WriteDat(Cur[2]);
		}
		else if(Cur[1]>Bef[1])
		{
			OLED_SetPos(Cur[0],x);
			WriteDat((Cur[2]-Bef[2])|Cur[2]);
		}
		else if(Cur[1]<Bef[1])
		{
			OLED_SetPos(Cur[0],x);
			WriteDat(Bef[2]-Cur[2]);
		}
	}
	else if(page_sub<0)
	{
		page_buff=Cur[0];
		OLED_SetPos(page_buff,x);
		WriteDat((Cur[2]<<1)-0x01);
		page_buff--;
		for(i=0;i<0-page_sub-1;i++)
		{
			OLED_SetPos(page_buff,x);
			WriteDat(0xff);
			page_buff--;
		}
		OLED_SetPos(page_buff,x);
		WriteDat(0xff<<(Bef[1]+1));
	}
	Before_State_Update(y);
	//把下一列，每一页的数据清除掉
	for(i=0;i<8;i++)
	{
		OLED_SetPos(i, x+1) ;
		for(j=0;j<1;j++)
			WriteDat(0x00);
	}
}

void show_title(void)
{
	static int i;
	for(i=0;i<5;i++)
	{
		OLED_ShowCN(22+i*16,5,i);//测试显示中文
	}
	OLED_ShowStr(12,3,"MINI Oscilloscope",1);				//测试8*16字符
	OLED_ShowStr(11,0,"ACE Robot Lab",2);
	delay_ms(1500);
	delay_ms(1500);
	OLED_CLS();//清屏
}

//void show_gif(void)
//{
//    OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//熊猫人合掌
//		delay_ms(150);
//	  OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP2);//熊猫人开掌
//	  delay_ms(150);
//	  OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//熊猫人合掌
//		delay_ms(150);
//	  OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP2);//熊猫人开掌
//	  delay_ms(150);
//	  OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//熊猫人合掌
//		delay_ms(150);
//	  OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP2);//熊猫人开掌
//}
