#include "GT911.h"
#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"
#include "i2c_application.h"
#include "xmc_lcd.h"


#define I2C_TIMEOUT                      0xFFFFFFF

//#define I2Cx_CLKCTRL                   0xB170FFFF   //10K
//#define I2Cx_CLKCTRL                   0xC0E06969   //50K
#define I2Cx_CLKCTRL                     0x80504C4E   //100K
//#define I2Cx_CLKCTRL                   0x30F03C6B   //200K

_m_tp_dev tp_dev;

#define I2Cx_SPEED                       100000
#define I2Cx_ADDRESS                     0xA0

#define I2Cx_PORT                        I2C2
#define I2Cx_CLK                         CRM_I2C2_PERIPH_CLOCK

#define I2Cx_SCL_PIN                     GPIO_PINS_10
#define I2Cx_SCL_GPIO_PORT               GPIOB
#define I2Cx_SCL_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SCL_GPIO_PinsSource         GPIO_PINS_SOURCE10
#define I2Cx_SCL_GPIO_MUX                GPIO_MUX_4

#define I2Cx_SDA_PIN                     GPIO_PINS_11
#define I2Cx_SDA_GPIO_PORT               GPIOB
#define I2Cx_SDA_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SDA_GPIO_PinsSource         GPIO_PINS_SOURCE11
#define I2Cx_SDA_GPIO_MUX                GPIO_MUX_4

#define BUF_SIZE                         8
#define MASTER_BOARD

uint8_t tx_buf[BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t rx_buf[BUF_SIZE] = {0};

u8 TP_Init(void);

const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED}; //the five color for the five touch point

void error_handler(uint32_t error_code);
void i2c_lowlevel_init(i2c_handle_type* hi2c);

/*****************************************************************************
 * @name       :u8 TP_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization touch screen
 * @parameters :None
 * @retvalue   :0-no calibration
								1-Has been calibrated
******************************************************************************/  
u8 TP_Init(void)
{			    		   
	GT911_Init();	
	return 0;
}


/**
  * @brief  error handler program
  * @param  i2c_status
  * @retval none
  */
void error_handler(uint32_t error_code)
{
  while(1)
  {
    at32_led_toggle(LED3);
    delay_ms(500);
  }
}

/**
  * @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_initstructure;

  if(hi2c->i2cx == I2Cx_PORT)
  {
    /* i2c periph clock enable */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);
    
        /* gpio configuration */
    gpio_pin_mux_config(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_GPIO_PinsSource, I2Cx_SCL_GPIO_MUX);

    gpio_pin_mux_config(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_GPIO_PinsSource, I2Cx_SDA_GPIO_MUX);

    /* gpio configuration */
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;

    /* configure i2c pins: scl */
    gpio_initstructure.gpio_pins = I2Cx_SCL_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_initstructure);

    /* configure i2c pins: sda */
    gpio_initstructure.gpio_pins = I2Cx_SDA_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_initstructure);

    i2c_init(hi2c->i2cx, 0x0F, I2Cx_CLKCTRL);

    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
  }
}

//==================================================================
//Name:						GT911_int_sync
//Author:					QD
//Date:						2017-07-06
//Function:				Sync GT911 INT signal
//								Sync is the most important part in initial stage
//Input:					unsigned int
//Output:					void
//option:
//==================================================================
void GT911_int_sync(u16 ms)
{
	INT_CTRL(0);
	delay_ms(ms);
	INT_IN();
}	

//==================================================================
//Name:						GT911_reset_guitar
//Author:					QD
//Date:						2017-07-06
//Function:				Rest GT911, and work with function GT911_int_sync
//								sync sequence depends on slave address
//Input:					unsigned char addr
//Output:					void
//option:
//==================================================================
void GT911_reset_guitar(u8 addr)
{
	INT_CTRL(1);
	RST_CTRL(1);
	delay_ms(20);
	RST_CTRL(0);
	INT_CTRL(0);
	delay_ms(20);
	INT_CTRL(addr == 0x28);
	delay_ms(20);
	RST_CTRL(1);
	delay_ms(20);
}

//==================================================================
//Name:						GT911_reset
//Author:					QD
//Date:						2017-07-06
//Function:				Rest GT911, just reset without sync
//Input:					void
//Output:					void
//option:
//==================================================================
void GT911_reset(void)
{
	RST_CTRL(0);
	delay_ms(10);
	RST_CTRL(1);
	delay_ms(10);
}

i2c_handle_type hi2cx;
//==================================================================
//Name:						GT911_gpio_init
//Author:					QD
//Date:						2017-07-06
//Function:				Init GT911 related GPIO: IIC,RST,INT
//Input:					void
//Output:					void
//option:
//==================================================================
void GT911_gpio_init(void)
{
  gpio_init_type  gpio_init_struct = {0};
  
  /* enable the gpio clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);

 
  gpio_init_struct.gpio_pins = TRST_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(TRST_PORT, &gpio_init_struct);
  /*
  gpio_init_struct.gpio_pins = TINT_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(TINT_PORT, &gpio_init_struct);*/
  
	
  hi2cx.i2cx = I2Cx_PORT;
   /* reset i2c peripheral */
  i2c_reset((&hi2cx)->i2cx);

  /* i2c peripheral initialization */
  i2c_lowlevel_init(&hi2cx);

  /* i2c peripheral enable */
  i2c_enable((&hi2cx)->i2cx, TRUE);
  GT911_reset();
}



//==================================================================
//Name:						GT9XX_WriteData
//Author:					QD
//Date:						2017-07-06
//Function:				Write data to GT911
//Input:					unsigned int addr				//address of register
//								unsigned char value			//value should be writed
//Output:					void
//option:
//==================================================================
u8 GT9XX_WriteData (u16 addr,u8 value)
{
	i2c_status_type i2c_status;
	tx_buf[0] = (u8)(addr >> 8);
	tx_buf[1] = (u8)addr;
	tx_buf[2] = value;
	/* start the request reception process */
  if((i2c_status = i2c_master_transmit(&hi2cx, GT9XX_IIC_WADDR, tx_buf, 3, I2C_TIMEOUT)) != I2C_OK)
  {
      error_handler(i2c_status);
  }
	return 0;
}

//==================================================================
//Name:						GT9XX_ReadData
//Author:					QD
//Date:						2017-07-06
//Function:				Read data from GT911
//Input:					unsigned int addr				//address of register
//								unsigned char *value		//pointer of data output
//Output:					void
//option:
//==================================================================
u8 GT9XX_ReadData (u16 addr, u8 cnt, u8 *value)
{
	u8 status;
	status = i2c_memory_read(&hi2cx, I2C_MEM_ADDR_WIDIH_16, GT9XX_IIC_RADDR, addr, value, cnt, I2C_TIMEOUT);
	return status;
}


//==================================================================
//Name:						Touch_Init
//Author:					QD
//Date:						2017-07-06
//Function:				GT911 init, including GPIO init, sync, and version check
//Input:					unsigned int addr				//address of register
//								unsigned char *value		//pointer of data output
//Output:					void
//option:
//==================================================================
u8 GT911_Init(void)
{
	u8 touchIC_ID[4];	
	GT911_gpio_init();
	GT911_reset_guitar(GT9XX_IIC_WADDR);
	GT911_int_sync(50);
	GT9XX_ReadData (GT9XX_ID_ADDR,4,touchIC_ID);
	if( touchIC_ID[0] == '9' )
	{
		printf("Touch ID: %s \r\n",touchIC_ID);
		GT9xx_send_config();
		return 1;
	}
	else
	{
		printf("Touch Error\r\n");
		return 0;
	}
}

u8 Touch_Get_Count(void)
{
	u8 count[1] = {0};
	GT9XX_ReadData (GT9XX_READ_ADDR,1,count);	//read touch data
	return (count[0]&0x0f);
}

const u16 TPX[] = {0x8150,0x8158,0x8160,0x8168,0x8170}; //电容屏触摸点数据地址（1~5）

//==================================================================
//Name:						Touch_Get_Data
//Author:					QD
//Date:						2017-07-06
//Function:				Get GT911 data, such as point and coordinate
//Input:					void
//Output:					void
//option:
//==================================================================
u8 GT911_Scan(void)
{
	u8 buf[42];
	u8 i=0;
	u8 res=0;
	u8 temp;
	u8 tempsta;
	
//	u8 mode=0;
 	static u8 t=0;//control the scan interval  
	t++;
	if((t%10)==0||t<10)//???,???10?CTP_Scan?????1?,????CPU???
	{ 
		GT9XX_ReadData(GT9XX_READ_ADDR, 42, buf);
		if((buf[0]&0X80)&&((buf[0]&0XF)<6))
		{	
			temp=0;	
			GT9XX_WriteData(GT9XX_READ_ADDR,temp);
		}		
		if((buf[0]&0XF)&&((buf[0]&0XF)<6))
		{
			for(i=0;i<(buf[0]&0XF);i++)
			{
				switch(buf[1+i*8])
				{
					case 4:
					{
						temp |= 1<<4;
						break;
					}
					case 3:
					{
						temp |= 1<<3;
						break;
					}
					case 2:
					{
						temp |= 1<<2;
						break;
					}
					case 1:
					{
						temp |= 1<<1;
						break;
					}
					case 0:
					{	
						temp |= 1<<0;
						break;
					}
					default:
					break;				
				}
			}
//			temp=0XFF<<(buf[0]&0XF);		//????????1???,??tp_dev.sta?? 
			tempsta=tp_dev.sta;			//?????tp_dev.sta?
			tp_dev.sta=temp|TP_PRES_DOWN|TP_CATH_PRES; 
			tp_dev.x[4]=tp_dev.x[0];	//????0???
			tp_dev.y[4]=tp_dev.y[0];
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//?????
				{
					//GT9XX_ReadData(TPX[i],4,buf);	//??XY???设定
					if(lcdpara.dir==0)
					{
						tp_dev.x[i]=((u16)buf[3+i*8]<<8)+buf[2+i*8];
						tp_dev.y[i]=((u16)buf[5+i*8]<<8)+buf[4+i*8];
					}
					else if(lcdpara.dir==1)
					{
						tp_dev.y[i]=lcdpara.height-(((u16)buf[3+i*8]<<8)+buf[2+i*8]);
						tp_dev.x[i]=((u16)buf[5+i*8]<<8)+buf[4+i*8];
					}
					else if(lcdpara.dir==2)
					{
						tp_dev.x[i]=lcdpara.width-(((u16)buf[3+i*8]<<8)+buf[2+i*8]);
						tp_dev.y[i]=lcdpara.height-(((u16)buf[5+i*8]<<8)+buf[4+i*8]);
					}
					else if(lcdpara.dir==3)
					{
						tp_dev.y[i]=((u16)buf[3+i*8]<<8)+buf[2+i*8];
						tp_dev.x[i]=lcdpara.width-(((u16)buf[5+i*8]<<8)+buf[4+i*8]);						
					}  
					//printf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
				}
			} 
			res=1;
			if(tp_dev.x[0]>lcdpara.width||tp_dev.y[0]>lcdpara.height)//????(?????)
			{ 
				if((buf[0]&0XF)>1)		//???????,????????????????.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//????,????????10?,???????
				}else					//????,???????(?????)  
				{
					tp_dev.x[0]=tp_dev.x[4];
					tp_dev.y[0]=tp_dev.y[4];
					buf[0]=0X80;		
					tp_dev.sta=tempsta;	//??tp_dev.sta
				}
			}else t=0;					//????,????????10?,???????
		}
	}
	if((buf[0]&0X8F)==0X80)//无触摸点按下
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);	//标记按键松开
		}else						//之前就没有被按下
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//清除点有效标记
		}	
	}
	if(t>240)t=10;//重新从10开始计数
	return res;
}


/*****************************************************************************
 * @name       :void Touch_Test(void)
 * @date       :2018-08-09 
 * @function   :touch test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*
void Touch_Test(void)
{
	u8 t=0;
	u8 i=0,j=0;	 
 	u16 lastpos[5][2];		
	GT911_Init();
	while(1)
	{
		j++;
		GT911_Scan();
		for(t=0;t<CT_MAX_TOUCH;t++)//5
		{
			if((tp_dev.sta)&(1<<t))//
			{
				if(tp_dev.x[t]<lcdpara.width&&tp_dev.y[t]<lcdpara.height)//?LCD???
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					if(tp_dev.x[t]>(lcdpara.width-24)&&tp_dev.y[t]<16)
					{
							if(j>1) //
							{
								continue;
							}
							//DrawTestPage("??10:??Touch??(??5???)  ");
							//LCD_ShowString(lcdpara.width-24,0,16,"RST",1);//??????
					}
					else
					{
						LCD_DrawLine2(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//??
						//lcd_draw_point(tp_dev.x[t],tp_dev.y[t]);

					}
					lastpos[t][0]=tp_dev.x[t];					lastpos[t][1]=tp_dev.y[t];
				}
			}else lastpos[t][0]=0XFFFF;
		}
		if(i%30==0)at32_led_toggle(LED3);
		if(j>4)
		{
			j=0;
		}
	}  
}
*/


