/**
  **************************************************************************
  * @file     xmc_lcd.c
  * @brief    xmc_lcd program file
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "xmc_lcd.h"
#include "gt911.h"
#include "stdio.h"
#include "string.h"

/** @addtogroup AT32F403A_periph_examples
  * @{
  */

/** @addtogroup 403A_XMC_lcd_touch_16bit
  * @{
  */
	
_lcd_para_type lcdpara;	

u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  

/**
  * @brief  configures the xmc and gpios to interface with the lcd.
  *         this function must be called before any write/read operation
  *         on the lcd.
  * @param  none
  * @retval none
  */
void xmc_init(void)
{
  gpio_init_type  gpio_init_struct = {0};
  xmc_norsram_init_type  xmc_norsram_init_struct;
  xmc_norsram_timing_init_type rw_timing_struct, w_timing_struct;

  /* enable the gpio clock */
  crm_periph_clock_enable(LCD_D0_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D1_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D2_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D3_GPIO_CLK,  TRUE); 
  crm_periph_clock_enable(LCD_D4_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D5_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D6_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D7_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D8_GPIO_CLK,  TRUE); 
  crm_periph_clock_enable(LCD_D9_GPIO_CLK,  TRUE);
  crm_periph_clock_enable(LCD_D10_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_D11_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_D12_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_D13_GPIO_CLK, TRUE); 
  crm_periph_clock_enable(LCD_D14_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_D15_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_NE1_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_NWE_GPIO_CLK, TRUE);
  crm_periph_clock_enable(LCD_NOE_GPIO_CLK, TRUE); 
  crm_periph_clock_enable(LCD_A16_GPIO_CLK,  TRUE);

  /* enable the xmc clock */
  crm_periph_clock_enable(CRM_XMC_PERIPH_CLOCK, TRUE);
  
  /*-- gpio configuration ------------------------------------------------------*/
  gpio_pin_mux_config(LCD_D0_GPIO_PORT,  LCD_D0_GPIO_PINS_SOURCE,  LCD_D0_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D1_GPIO_PORT,  LCD_D1_GPIO_PINS_SOURCE,  LCD_D1_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D2_GPIO_PORT,  LCD_D2_GPIO_PINS_SOURCE,  LCD_D2_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D3_GPIO_PORT,  LCD_D3_GPIO_PINS_SOURCE,  LCD_D3_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D4_GPIO_PORT,  LCD_D4_GPIO_PINS_SOURCE,  LCD_D4_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D5_GPIO_PORT,  LCD_D5_GPIO_PINS_SOURCE,  LCD_D5_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D6_GPIO_PORT,  LCD_D6_GPIO_PINS_SOURCE,  LCD_D6_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D7_GPIO_PORT,  LCD_D7_GPIO_PINS_SOURCE,  LCD_D7_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D8_GPIO_PORT,  LCD_D8_GPIO_PINS_SOURCE,  LCD_D8_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D9_GPIO_PORT,  LCD_D9_GPIO_PINS_SOURCE,  LCD_D9_GPIO_MUX); 
  gpio_pin_mux_config(LCD_D10_GPIO_PORT, LCD_D10_GPIO_PINS_SOURCE, LCD_D10_GPIO_MUX);
  gpio_pin_mux_config(LCD_D11_GPIO_PORT, LCD_D11_GPIO_PINS_SOURCE, LCD_D11_GPIO_MUX);
  gpio_pin_mux_config(LCD_D12_GPIO_PORT, LCD_D12_GPIO_PINS_SOURCE, LCD_D12_GPIO_MUX);
  gpio_pin_mux_config(LCD_D13_GPIO_PORT, LCD_D13_GPIO_PINS_SOURCE, LCD_D13_GPIO_MUX);
  gpio_pin_mux_config(LCD_D14_GPIO_PORT, LCD_D14_GPIO_PINS_SOURCE, LCD_D14_GPIO_MUX);
  gpio_pin_mux_config(LCD_D15_GPIO_PORT, LCD_D15_GPIO_PINS_SOURCE, LCD_D15_GPIO_MUX);
  gpio_pin_mux_config(LCD_NE1_GPIO_PORT, LCD_NE1_GPIO_PINS_SOURCE, LCD_NE1_GPIO_MUX);
  gpio_pin_mux_config(LCD_NWE_GPIO_PORT, LCD_NWE_GPIO_PINS_SOURCE, LCD_NWE_GPIO_MUX);
  gpio_pin_mux_config(LCD_NOE_GPIO_PORT, LCD_NOE_GPIO_PINS_SOURCE, LCD_NOE_GPIO_MUX);  
  gpio_pin_mux_config(LCD_A16_GPIO_PORT,  LCD_A16_GPIO_PINS_SOURCE, LCD_A16_GPIO_MUX); 
    
  /* lcd data lines configuration */  
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  
  gpio_init_struct.gpio_pins = LCD_D0_GPIO_PIN;
  gpio_init(LCD_D0_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D1_GPIO_PIN;
  gpio_init(LCD_D1_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D2_GPIO_PIN;
  gpio_init(LCD_D2_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D3_GPIO_PIN;
  gpio_init(LCD_D3_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D4_GPIO_PIN;
  gpio_init(LCD_D4_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D5_GPIO_PIN;
  gpio_init(LCD_D5_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D6_GPIO_PIN;
  gpio_init(LCD_D6_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D7_GPIO_PIN;
  gpio_init(LCD_D7_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D8_GPIO_PIN;
  gpio_init(LCD_D8_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D9_GPIO_PIN;
  gpio_init(LCD_D9_GPIO_PORT,  &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D10_GPIO_PIN;
  gpio_init(LCD_D10_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D11_GPIO_PIN;
  gpio_init(LCD_D11_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D12_GPIO_PIN;
  gpio_init(LCD_D12_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D13_GPIO_PIN;
  gpio_init(LCD_D13_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D14_GPIO_PIN;
  gpio_init(LCD_D14_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_D15_GPIO_PIN;
  gpio_init(LCD_D15_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_NE1_GPIO_PIN;
  gpio_init(LCD_NE1_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_NWE_GPIO_PIN;
  gpio_init(LCD_NWE_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_NOE_GPIO_PIN;
  gpio_init(LCD_NOE_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LCD_A16_GPIO_PIN;
  gpio_init(LCD_A16_GPIO_PORT,  &gpio_init_struct);
  
	
  /* lcd BL  configuration */
  gpio_init_struct.gpio_pins = LCD_BL_GPIO_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(LCD_BL_GPIO_PORT, &gpio_init_struct);

  /* lcd rst  configuration */
  gpio_init_struct.gpio_pins = LCD_RESET_GPIO_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(LCD_RESET_GPIO_PORT, &gpio_init_struct);


  /*-- xmc configuration ------------------------------------------------------*/
  xmc_norsram_default_para_init(&xmc_norsram_init_struct);
  xmc_norsram_init_struct.subbank = XMC_BANK1_NOR_SRAM1;
  xmc_norsram_init_struct.data_addr_multiplex = XMC_DATA_ADDR_MUX_DISABLE;
  xmc_norsram_init_struct.device = XMC_DEVICE_SRAM;
  xmc_norsram_init_struct.bus_type = XMC_BUSTYPE_16_BITS;
  xmc_norsram_init_struct.burst_mode_enable = XMC_BURST_MODE_DISABLE;
  xmc_norsram_init_struct.asynwait_enable = XMC_ASYN_WAIT_DISABLE;
  xmc_norsram_init_struct.wait_signal_lv = XMC_WAIT_SIGNAL_LEVEL_LOW;
  xmc_norsram_init_struct.wrapped_mode_enable = XMC_WRAPPED_MODE_DISABLE;
  xmc_norsram_init_struct.wait_signal_config = XMC_WAIT_SIGNAL_SYN_BEFORE;
  xmc_norsram_init_struct.write_enable = XMC_WRITE_OPERATION_ENABLE;
  xmc_norsram_init_struct.wait_signal_enable = XMC_WAIT_SIGNAL_DISABLE;
  xmc_norsram_init_struct.write_timing_enable = XMC_WRITE_TIMING_ENABLE;
  xmc_norsram_init_struct.write_burst_syn = XMC_WRITE_BURST_SYN_DISABLE;
  xmc_nor_sram_init(&xmc_norsram_init_struct);

  /* timing configuration */
  xmc_norsram_timing_default_para_init(&rw_timing_struct, &w_timing_struct);
  rw_timing_struct.subbank = XMC_BANK1_NOR_SRAM1;
  rw_timing_struct.write_timing_enable = XMC_WRITE_TIMING_ENABLE;
  rw_timing_struct.addr_setup_time = 0x2;
  rw_timing_struct.addr_hold_time = 0x0;
  rw_timing_struct.data_setup_time = 0x2;
  rw_timing_struct.bus_latency_time = 0x0;
  rw_timing_struct.clk_psc = 0x0;
  rw_timing_struct.data_latency_time = 0x0;
  rw_timing_struct.mode = XMC_ACCESS_MODE_A;
  w_timing_struct.subbank = XMC_BANK1_NOR_SRAM1;
  w_timing_struct.write_timing_enable = XMC_WRITE_TIMING_ENABLE;
  w_timing_struct.addr_setup_time = 0x2;
  w_timing_struct.addr_hold_time = 0x0;
  w_timing_struct.data_setup_time = 0x2;
  w_timing_struct.bus_latency_time = 0x0;
  w_timing_struct.clk_psc = 0x0;
  w_timing_struct.data_latency_time = 0x0;
  w_timing_struct.mode = XMC_ACCESS_MODE_A;
  xmc_nor_sram_timing_config(&rw_timing_struct, &w_timing_struct);

  /* bus turnaround phase for consecutive read duration and consecutive write duration */
  xmc_ext_timing_config(XMC_BANK1_NOR_SRAM1, 0x08, 0x08);

  /* enable xmc_bank1_nor_sram4 */
  xmc_nor_sram_enable(XMC_BANK1_NOR_SRAM1, TRUE);
}


/**
  * @brief  this function is write command to lcd.
  * @param  command : the command to write.
  * @retval none
  */
void lcd_wr_command(uint16_t command)
{
  *(__IO uint16_t*) XMC_LCD_COMMAND = command;
}

/**
  * @brief  this function is write data to lcd.
  * @param  data : the data to write.
  * @retval none
  */
void lcd_wr_data(uint16_t data)
{
  *(__IO uint16_t*) XMC_LCD_DATA = data;
}


uint16_t lcd_rd_data(void)
{
  uint16_t data;

  data = *(uint16_t*)XMC_LCD_DATA;

  return data;
}
/*****************************************************************************
 * @name       :void lcd_wr_reg(u16 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void lcd_wr_reg(u16 LCD_Reg, u16 LCD_RegValue)
{	
	*(__IO uint16_t*) XMC_LCD_COMMAND = LCD_Reg;		//
	*(__IO uint16_t*) XMC_LCD_DATA = LCD_RegValue;//
}	 

/*****************************************************************************
 * @name       :u16 LCD_ReadReg(u16 LCD_Reg)
 * @date       :2018-11-13 
 * @function   :read value from specially registers
 * @parameters :LCD_Reg:Register address
 * @retvalue   :read value
******************************************************************************/
void LCD_ReadReg(u16 LCD_Reg,u8 *Rval,int n)
{
	lcd_wr_command(LCD_Reg); 
	while(n--)
	{		
		*(Rval++) = lcd_rd_data();
		delay_us(100);
	}
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	lcd_wr_command(lcdpara.wramcmd);
}	 

/*****************************************************************************
 * @name       :void LCD_ReadRAM_Prepare(void)
 * @date       :2018-11-13 
 * @function   :Read GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_ReadRAM_Prepare(void)
{
	lcd_wr_command(lcdpara.rramcmd);
}


u16 Color_To_565(u8 r, u8 g, u8 b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

/*****************************************************************************
 * @name       :u16 Lcd_ReadData_16Bit(void)
 * @date       :2018-11-13 
 * @function   :Read an 16-bit value from the LCD screen
 * @parameters :None
 * @retvalue   :read value
******************************************************************************/	
u16 Lcd_ReadData_16Bit(void)
{
	u16 r,g,b;
	//dummy data
	r = lcd_rd_data();
	delay_us(1);//
	//8bit:red data	
	//16bit:red and green data
	r = lcd_rd_data();
	delay_us(1);//
	//8bit:green data
  //16bit:blue data
	g = lcd_rd_data();
#if LCD_USE8BIT_MODEL
	delay_us(1);//ÑÓÊ±1us	
	//8bit:blue data
	b = LCD_RD_DATA();
#else	
  b = g>>8;
	g = r&0xFF; 
	r = r>>8;
#endif
	return Color_To_565(r, g, b);
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//
	lcd_wr_data(POINT_COLOR); 
}


/*****************************************************************************
 * @name       :u16 LCD_ReadPoint(u16 x,u16 y)
 * @date       :2018-11-13 
 * @function   :Read a pixel color value at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :the read color value
******************************************************************************/	
u16 LCD_ReadPoint(u16 x,u16 y)
{
	u16 color;
	if(x>=lcdpara.width||y>=lcdpara.height)
	{
		return 0;	//
	}
	LCD_SetCursor(x,y);//
	LCD_ReadRAM_Prepare();
	color = Lcd_ReadData_16Bit(); 
	return color;
}


/**
  * @brief  configures the lcd.
  *         this function must be called before any write/read operation
  *         on the lcd.
  * @param  none
  * @retval none
  */
void lcd_init(void)
{
  /* init xmc */
  xmc_init();
  
  LCD_RESET_HIGH();
  delay_ms(120);
  LCD_RESET_LOW();
  delay_ms(120);
  LCD_RESET_HIGH();
  delay_ms(120);
  

  delay_ms(50);
	//************* ILI9488**********//	
	lcd_wr_command(0XF7);
	lcd_wr_data(0xA9);
	lcd_wr_data(0x51);
	lcd_wr_data(0x2C);
	lcd_wr_data(0x82);

	lcd_wr_command(0xC0);
	lcd_wr_data(0x11);
	lcd_wr_data(0x09);

	lcd_wr_command(0xC1);
	lcd_wr_data(0x41);

	lcd_wr_command(0XC5);
	lcd_wr_data(0x00);
	lcd_wr_data(0x0A);
	lcd_wr_data(0x80);

	lcd_wr_command(0xB1);
	lcd_wr_data(0xB0);
	lcd_wr_data(0x11);

	lcd_wr_command(0xB4);
	lcd_wr_data(0x02);

	lcd_wr_command(0xB6);
	lcd_wr_data(0x02);
	lcd_wr_data(0x22);

	lcd_wr_command(0xB7);
	lcd_wr_data(0xc6);

	lcd_wr_command(0xBE);
	lcd_wr_data(0x00);
	lcd_wr_data(0x04);

	lcd_wr_command(0xE9);
	lcd_wr_data(0x00);

	lcd_wr_command(0x36);
	lcd_wr_data(0x08);

	lcd_wr_command(0x3A);
	lcd_wr_data(0x55);

	lcd_wr_command(0xE0);
	lcd_wr_data(0x00);
	lcd_wr_data(0x07);
	lcd_wr_data(0x10);
	lcd_wr_data(0x09);
	lcd_wr_data(0x17);
	lcd_wr_data(0x0B);
	lcd_wr_data(0x41);
	lcd_wr_data(0x89);
	lcd_wr_data(0x4B);
	lcd_wr_data(0x0A);
	lcd_wr_data(0x0C);
	lcd_wr_data(0x0E);
	lcd_wr_data(0x18);
	lcd_wr_data(0x1B);
	lcd_wr_data(0x0F);

	lcd_wr_command(0XE1);
	lcd_wr_data(0x00);
	lcd_wr_data(0x17);
	lcd_wr_data(0x1A);
	lcd_wr_data(0x04);
	lcd_wr_data(0x0E);
	lcd_wr_data(0x06);
	lcd_wr_data(0x2F);
	lcd_wr_data(0x45);
	lcd_wr_data(0x43);
	lcd_wr_data(0x02);
	lcd_wr_data(0x0A);
	lcd_wr_data(0x09);
	lcd_wr_data(0x32);
	lcd_wr_data(0x36);
	lcd_wr_data(0x0F);

	lcd_wr_command(0x11);
	delay_ms(120);
	lcd_wr_command(0x29);
	
  LCD_direction(0);
	LCD_BL_HIGH();	 
	lcd_clear(RED);
	
}

  
/**
  * @brief  this function is set row&column coordinates for lcd.
  * @param  xstart : row coordinates starting vaule.
  * @param  ystart : column coordinates starting vaule.
  * @param  xend : row coordinates ending vaule.
  * @param  yend : column coordinates ending vaule.
  * @retval none
  */
void lcd_set_block(uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend)
{
  /* set row coordinates */
  lcd_wr_command(0x2a);
  lcd_wr_data(xstart >> 8);
  lcd_wr_data(xstart);
  lcd_wr_data(xend >> 8);
  lcd_wr_data(xend);

  /* set column coordinates */
  lcd_wr_command(0x2b);
  lcd_wr_data(ystart >> 8);
  lcd_wr_data(ystart);
  lcd_wr_data(yend >> 8);
  lcd_wr_data(yend);

  /* enable write menory */
  lcd_wr_command(0x2c);
}

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	lcd_set_block(Xpos,Ypos,Xpos,Ypos);	
}

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(char direction)
{ 
	lcdpara.setxcmd=0x2A;
	lcdpara.setycmd=0x2B;
	lcdpara.wramcmd=0x2C;
	lcdpara.rramcmd=0x2E;
	lcdpara.dir=direction%4;
	switch(direction){		  
		case 0:						 	 		
			lcdpara.width=LCD_W;
			lcdpara.height=LCD_H;		
			lcd_wr_reg(0x36,(1<<3));
		break;
		case 1:
			lcdpara.width=LCD_H;
			lcdpara.height=LCD_W;
			lcd_wr_reg(0x36,(1<<3)|(1<<6)|(1<<5));
		break;
		case 2:						 	 		
			lcdpara.width=LCD_W;
			lcdpara.height=LCD_H;	
			lcd_wr_reg(0x36,(1<<3)|(1<<6)|(1<<7));
		break;
		case 3:
			lcdpara.width=LCD_H;
			lcdpara.height=LCD_W;
			lcd_wr_reg(0x36,(1<<3)|(1<<5)|(1<<7));
		break;	
		default:break;
	}		
}
/**
  * @brief  this function is write one point to lcd.
  * @param  data : the data to write.
  * @retval none
  */
void lcd_write_one_point(uint16_t color)
{
  lcd_wr_data(color);
}

/**
  * @brief  this function is draw point to lcd.
  * @param  data : the data to write.
  * @retval none
  */
void lcd_draw_point(uint16_t x, uint16_t y)
{
  lcd_set_block(x, y ,x ,y);
  lcd_write_one_point(POINT_COLOR);	
}

/**
  * @brief  this function is clear the lcd.
  * @param  data : the data to write.
  * @retval none
  */
void lcd_clear(uint16_t color)
{
  uint32_t i;

  lcd_set_block(0, 0, 320, 480);
  for(i = 0; i < 320*480; i++)
  {
    lcd_write_one_point(color);
  }
}

int32_t lcd_DrawBitmap(uint32_t x, 
                         uint32_t y, 
                         uint32_t width, 
                         uint32_t height, 
                         const uint8_t *bitmap) 
{
    volatile uint16_t *phwDes = (volatile uint16_t*)(XMC_LCD_DATA + y * LCD_H + x);
    const uint16_t *phwSrc = (const uint16_t *)bitmap;
    for (int_fast16_t i = 0; i < height; i++) {
        memcpy ((uint16_t *)phwDes, phwSrc, width * 2);
        phwSrc += width;
        phwDes += LCD_H;
    }

    return 0;
}


/**
  * @}
  */

/**
  * @}
  */
