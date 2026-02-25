
#ifndef __GT911_H__
#define __GT911_H__

#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"

#define TOUCH_MAX 5



#define RST_OUT()	{	TRST_PORT->cfgr&=~(3<<(TRST_PIN_NUM*2));TRST_PORT->cfgr|=1<<((TRST_PIN_NUM)*2);}		//set RSSET pin to output
//#define INT_OUT()	{	TINT_PORT->cfgr&=~(3<<(TINT_PIN_NUM*2));TINT_PORT->cfgr|=1<<((TINT_PIN_NUM)*2);}		//set RSSET pin to output

//#define INT_IN()	{	TINT_PORT->cfgr&=~(3<<(TINT_PIN_NUM*2));}		//set INT pin to input
#define INT_IN()

#define TRST_PORT  GPIOB
#define TRST_PIN   GPIO_PINS_9
#define TRST_PIN_NUM   9




#define RST_CTRL(n)   (n?gpio_bits_set(TRST_PORT,TRST_PIN):gpio_bits_reset(TRST_PORT,TRST_PIN))	//GT911 RESET pin out high or low
//#define INT_CTRL(n)   (n?gpio_bits_set(TINT_PORT,TINT_PIN):gpio_bits_reset(TINT_PORT,TINT_PIN))	//GT911 RESET pin out high or low
#define INT_CTRL(n) 

#define GT9XX_IIC_RADDR 0xBB	//IIC read address
#define GT9XX_IIC_WADDR 0xBA	//IIC write address

#define GT9XX_READ_ADDR 0x814E	//touch point information
#define GT9XX_ID_ADDR 0x8140		//ID of touch IC

#define TP_PRES_DOWN 0x80  //?????	  
#define TP_CATH_PRES 0x40  //?????? 	
#define CT_MAX_TOUCH  5 

typedef struct
{
	u16 x[CT_MAX_TOUCH]; 		//
	u16 y[CT_MAX_TOUCH];		//
								//x[4],y[4]???????????. 
	u8  sta;			//???? 
								//b7:??1/??0; 
	              //b6:0,??????;1,?????. 
								//b5:??
								//b4~b0:??????????(0,?????,1????)
}_m_tp_dev;


void GT911_int_sync(u16 ms);
void GT911_reset_guitar(u8 addr);
void GT911_gpio_init(void);
u8 GT9XX_WriteHandle (u16 addr);
u8 GT9XX_WriteData (u16 addr,u8 value);
u8 GT9XX_ReadData (u16 addr, u8 cnt, u8 *value);
u8 GT911_Init(void);
u8 Touch_Get_Count(void);
u8 GT911_Scan(void);
void GT9xx_send_config(void);
void GT9xx_Eint_Init(void);


#endif
