/**
  **************************************************************************
  * @file     xmc_lcd.h
  * @brief    xmc_lcd program header
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
#ifndef __XMC_LCD_H
#define __XMC_LCD_H

#ifdef __cplusplus
extern "C" {
#endif
#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"

/** @addtogroup AT32F403A_periph_examples
  * @{
  */

/** @addtogroup 403A_XMC_lcd_touch_16bit
  * @{
  */

/** @defgroup XMC_lcd_touch_16bit_device_type
  * @{
  */


//LCD para
typedef struct  
{										    
	u16 width;			
	u16 height;			//LCD height
	u16 id;				  //LCD ID
	u8  dir;			  //0:    1:
	u16	 wramcmd;		//write gram
	u16  rramcmd;   //read gram
	u16  setxcmd;		//set x cmd
	u16  setycmd;		//set y cmd
}_lcd_para_type; 

extern _lcd_para_type lcdpara;	

#define LCD_W 320
#define LCD_H 480
/**
  * @}
  */

/** @defgroup XMC_lcd_touch_16bit_address_of_data_and_command
  * @{
  */

/* the address of write data & command (xmc_a16) */
//#define  XMC_LCD_COMMAND                 0x6C0007FE
//#define  XMC_LCD_DATA                    0x6C000800
#define  XMC_LCD_COMMAND                 0x6007FFFE
#define  XMC_LCD_DATA                    0x60080000

/**
  * @}
  */

/** @defgroup XMC_lcd_touch_16bit_reset_line
  * @{
  */

static uint16_t LCD_ID;

#define WHITE                            0xFFFF
#define BLACK                            0x0000
#define BLUE                             0x001F
#define BRED                             0xF81F
#define GRED                             0xFFE0
#define GBLUE                            0x07FF
#define RED                              0xF800
#define MAGENTA                          0xF81F
#define GREEN                            0x07E0
#define CYAN                             0x7FFF
#define YELLOW                           0xFFE0
#define BROWN                            0xBC40
#define BRRED                            0xFC07
#define GRAY                             0x8430

/**
  * @brief the lcd io definition
  */
#define LCD_D0_GPIO_PORT                 GPIOD
#define LCD_D0_GPIO_CLK                  CRM_GPIOD_PERIPH_CLOCK
#define LCD_D0_GPIO_PIN                  GPIO_PINS_14
#define LCD_D0_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE14 
#define LCD_D0_GPIO_MUX                  GPIO_MUX_12   

#define LCD_D1_GPIO_PORT                 GPIOD
#define LCD_D1_GPIO_CLK                  CRM_GPIOD_PERIPH_CLOCK
#define LCD_D1_GPIO_PIN                  GPIO_PINS_15
#define LCD_D1_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE15 
#define LCD_D1_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D2_GPIO_PORT                 GPIOD
#define LCD_D2_GPIO_CLK                  CRM_GPIOD_PERIPH_CLOCK
#define LCD_D2_GPIO_PIN                  GPIO_PINS_0
#define LCD_D2_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE0 
#define LCD_D2_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D3_GPIO_PORT                 GPIOD
#define LCD_D3_GPIO_CLK                  CRM_GPIOD_PERIPH_CLOCK
#define LCD_D3_GPIO_PIN                  GPIO_PINS_1
#define LCD_D3_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE1 
#define LCD_D3_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D4_GPIO_PORT                 GPIOE
#define LCD_D4_GPIO_CLK                  CRM_GPIOE_PERIPH_CLOCK
#define LCD_D4_GPIO_PIN                  GPIO_PINS_7
#define LCD_D4_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE7 
#define LCD_D4_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D5_GPIO_PORT                 GPIOE
#define LCD_D5_GPIO_CLK                  CRM_GPIOE_PERIPH_CLOCK
#define LCD_D5_GPIO_PIN                  GPIO_PINS_8
#define LCD_D5_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE8 
#define LCD_D5_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D6_GPIO_PORT                 GPIOE
#define LCD_D6_GPIO_CLK                  CRM_GPIOE_PERIPH_CLOCK
#define LCD_D6_GPIO_PIN                  GPIO_PINS_9
#define LCD_D6_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE9 
#define LCD_D6_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D7_GPIO_PORT                 GPIOE
#define LCD_D7_GPIO_CLK                  CRM_GPIOE_PERIPH_CLOCK
#define LCD_D7_GPIO_PIN                  GPIO_PINS_10
#define LCD_D7_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE10 
#define LCD_D7_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D8_GPIO_PORT                 GPIOE
#define LCD_D8_GPIO_CLK                  CRM_GPIOE_PERIPH_CLOCK
#define LCD_D8_GPIO_PIN                  GPIO_PINS_11
#define LCD_D8_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE11 
#define LCD_D8_GPIO_MUX                  GPIO_MUX_12     
                                         
#define LCD_D9_GPIO_PORT                 GPIOE
#define LCD_D9_GPIO_CLK                  CRM_GPIOE_PERIPH_CLOCK
#define LCD_D9_GPIO_PIN                  GPIO_PINS_12
#define LCD_D9_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE12 
#define LCD_D9_GPIO_MUX                  GPIO_MUX_12     

#define LCD_D10_GPIO_PORT                GPIOE
#define LCD_D10_GPIO_CLK                 CRM_GPIOE_PERIPH_CLOCK
#define LCD_D10_GPIO_PIN                 GPIO_PINS_13
#define LCD_D10_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE13 
#define LCD_D10_GPIO_MUX                 GPIO_MUX_12     
                                         
#define LCD_D11_GPIO_PORT                GPIOE
#define LCD_D11_GPIO_CLK                 CRM_GPIOE_PERIPH_CLOCK
#define LCD_D11_GPIO_PIN                 GPIO_PINS_14
#define LCD_D11_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE14 
#define LCD_D11_GPIO_MUX                 GPIO_MUX_12     
                                         
#define LCD_D12_GPIO_PORT                GPIOE
#define LCD_D12_GPIO_CLK                 CRM_GPIOE_PERIPH_CLOCK
#define LCD_D12_GPIO_PIN                 GPIO_PINS_15
#define LCD_D12_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE15 
#define LCD_D12_GPIO_MUX                 GPIO_MUX_12     

#define LCD_D13_GPIO_PORT                GPIOD
#define LCD_D13_GPIO_CLK                 CRM_GPIOD_PERIPH_CLOCK
#define LCD_D13_GPIO_PIN                 GPIO_PINS_8
#define LCD_D13_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE8 
#define LCD_D13_GPIO_MUX                 GPIO_MUX_12     
                                         
#define LCD_D14_GPIO_PORT                GPIOD
#define LCD_D14_GPIO_CLK                 CRM_GPIOD_PERIPH_CLOCK
#define LCD_D14_GPIO_PIN                 GPIO_PINS_9
#define LCD_D14_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE9
#define LCD_D14_GPIO_MUX                 GPIO_MUX_12     
                                         
#define LCD_D15_GPIO_PORT                GPIOD
#define LCD_D15_GPIO_CLK                 CRM_GPIOD_PERIPH_CLOCK
#define LCD_D15_GPIO_PIN                 GPIO_PINS_10
#define LCD_D15_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE10
#define LCD_D15_GPIO_MUX                 GPIO_MUX_12 
                                         
#define LCD_NE1_GPIO_PORT                GPIOD
#define LCD_NE1_GPIO_CLK                 CRM_GPIOD_PERIPH_CLOCK
#define LCD_NE1_GPIO_PIN                 GPIO_PINS_7
#define LCD_NE1_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE7
#define LCD_NE1_GPIO_MUX                 GPIO_MUX_12     
                                         
#define LCD_NWE_GPIO_PORT                GPIOD
#define LCD_NWE_GPIO_CLK                 CRM_GPIOD_PERIPH_CLOCK
#define LCD_NWE_GPIO_PIN                 GPIO_PINS_5
#define LCD_NWE_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE5 
#define LCD_NWE_GPIO_MUX                 GPIO_MUX_12     
                                         
#define LCD_NOE_GPIO_PORT                GPIOD
#define LCD_NOE_GPIO_CLK                 CRM_GPIOD_PERIPH_CLOCK
#define LCD_NOE_GPIO_PIN                 GPIO_PINS_4
#define LCD_NOE_GPIO_PINS_SOURCE         GPIO_PINS_SOURCE4 
#define LCD_NOE_GPIO_MUX                 GPIO_MUX_12     
                                         
#define LCD_A16_GPIO_PORT                 GPIOD
#define LCD_A16_GPIO_CLK                  CRM_GPIOD_PERIPH_CLOCK
#define LCD_A16_GPIO_PIN                  GPIO_PINS_13
#define LCD_A16_GPIO_PINS_SOURCE          GPIO_PINS_SOURCE13  
#define LCD_A16_GPIO_MUX                  GPIO_MUX_12     

#define LCD_BL_GPIO_CLK                  CRM_GPIOA_PERIPH_CLOCK
#define LCD_BL_GPIO_PIN                  GPIO_PINS_8
#define LCD_BL_GPIO_PORT                 GPIOA

#define LCD_RESET_GPIO_CLK               CRM_GPIOE_PERIPH_CLOCK
#define LCD_RESET_GPIO_PIN               GPIO_PINS_5
#define LCD_RESET_GPIO_PORT              GPIOE

/* the lcd back light line */
#define LCD_BL_HIGH()              gpio_bits_set(LCD_BL_GPIO_PORT,LCD_BL_GPIO_PIN)      
#define LCD_BL_LOW()               gpio_bits_reset(LCD_BL_GPIO_PORT,LCD_BL_GPIO_PIN)           

/* the lcd reset line */                 
#define LCD_RESET_HIGH()          gpio_bits_set(LCD_RESET_GPIO_PORT,LCD_RESET_GPIO_PIN)             
#define LCD_RESET_LOW()           gpio_bits_reset(LCD_RESET_GPIO_PORT,LCD_RESET_GPIO_PIN)          



void xmc_init(void);
void lcd_init(void);
void lcd_wr_command(uint16_t command);
void lcd_wr_data(uint16_t data);
uint16_t lcd_rd_data(void);
void lcd_set_block(uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend);
void lcd_write_one_point(uint16_t color);
void lcd_draw_point(uint16_t x, uint16_t y);
void lcd_clear(uint16_t color);
void LCD_direction(char direction);
void lcd_wr_reg(u16 LCD_Reg, u16 LCD_RegValue);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void Touch_Test(void);
int32_t lcd_DrawBitmap(uint32_t x, 
                         uint32_t y, 
                         uint32_t width, 
                         uint32_t height, 
                         const uint8_t *bitmap);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
