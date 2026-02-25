/**
  **************************************************************************
  * @file     at32f435_437_board.c
  * @brief    header file for at-start board. set of firmware functions to
  *           manage leds and push-KEY. initialize delay function.
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

#ifndef __AT32F435_437_BOARD_H
#define __AT32F435_437_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "at32f435_437.h"
typedef signed long      S32;

//#include "zscdefine.h"

/** @addtogroup AT32F435_437_board
  * @{
  */

/** @addtogroup BOARD
  * @{
  */

/** @defgroup BOARD_pins_definition
  * @{
  */

/**
  * this header include define support list:
  *   1. at-start-f435 v1.x board
  *   2. at-start-f437 v1.x board
  * if define AT_START_F435_V1, the header file support at-start-f437 v1.x board
  * if define AT_START_F437_V1, the header file support at-start-f437 v1.x board
  */

#if !defined (AT_START_F435_V1) && !defined (AT_START_F437_V1)
#error "please select first the board at-start device used in your application (in at32f435_437_board.h file)"
#endif

/******************** define led ********************/
typedef enum
{
  LED3                                   = 0,
  LED4                                   = 1
}led_type;

#define LED_NUM                         2

#if defined (AT_START_F435_V1) || defined (AT_START_F437_V1)
#define LED3_PIN                         GPIO_PINS_12
#define LED3_GPIO                        GPIOC
#define LED3_GPIO_CRM_CLK                CRM_GPIOC_PERIPH_CLOCK

#define LED4_PIN                         GPIO_PINS_9
#define LED4_GPIO                        GPIOC
#define LED4_GPIO_CRM_CLK                CRM_GPIOC_PERIPH_CLOCK
#endif

/**************** define print uart ******************/
#define PRINT_UART                       USART1
#define PRINT_UART_CRM_CLK               CRM_USART1_PERIPH_CLOCK
#define PRINT_UART_TX_PIN                GPIO_PINS_6
#define PRINT_UART_TX_GPIO               GPIOB
#define PRINT_UART_TX_GPIO_CRM_CLK       CRM_GPIOB_PERIPH_CLOCK
#define PRINT_UART_TX_PIN_SOURCE         GPIO_PINS_SOURCE6
#define PRINT_UART_TX_PIN_MUX_NUM        GPIO_MUX_7

#define PRINT_UART_RX_PIN                GPIO_PINS_7
#define PRINT_UART_RX_GPIO               GPIOB
#define PRINT_UART_RX_GPIO_CRM_CLK       CRM_GPIOB_PERIPH_CLOCK
#define PRINT_UART_RX_PIN_SOURCE         GPIO_PINS_SOURCE7
#define PRINT_UART_RX_PIN_MUX_NUM        GPIO_MUX_7


/**************** define print uart ******************/
#define ESP12_UART                       USART3
#define ESP12_UART_CRM_CLK               CRM_USART3_PERIPH_CLOCK
#define ESP12_UART_TX_PIN                GPIO_PINS_10
#define ESP12_UART_TX_GPIO               GPIOC
#define ESP12_UART_TX_GPIO_CRM_CLK       CRM_GPIOC_PERIPH_CLOCK
#define ESP12_UART_TX_PIN_SOURCE         GPIO_PINS_SOURCE10
#define ESP12_UART_TX_PIN_MUX_NUM        GPIO_MUX_7

#define ESP12_UART_RX_PIN                GPIO_PINS_11
#define ESP12_UART_RX_GPIO               GPIOC
#define ESP12_UART_RX_GPIO_CRM_CLK       CRM_GPIOC_PERIPH_CLOCK
#define ESP12_UART_RX_PIN_SOURCE         GPIO_PINS_SOURCE11
#define ESP12_UART_RX_PIN_MUX_NUM        GPIO_MUX_7



/******************* define KEY *******************/
typedef enum
{
  KEY_PRESSED                         = 0,
  NO_KEY                              = 1
}KEY_Value_t;

/******************** define KEY ********************/
typedef enum
{
  KEY1        = 0,
  KEY2        = 1,
  KEY3,
  KEY4,
  KEY5,
  KEY6,
  KEY7,
  KEY8
  
}key_type;

#define LED_NUM                         2

#define KEY1_PIN                  GPIO_PINS_13
#define KEY1_PORT                 GPIOB                                 
#define KEY1_CRM_CLK              CRM_GPIOB_PERIPH_CLOCK

#define KEY2_PIN                  GPIO_PINS_14
#define KEY2_PORT                 GPIOB                                 
#define KEY2_CRM_CLK              CRM_GPIOB_PERIPH_CLOCK

#define KEY3_PIN                  GPIO_PINS_15
#define KEY3_PORT                 GPIOB                                 
#define KEY3_CRM_CLK              CRM_GPIOB_PERIPH_CLOCK

#define KEY4_PIN                  GPIO_PINS_1
#define KEY4_PORT                 GPIOE                                 
#define KEY4_CRM_CLK              CRM_GPIOE_PERIPH_CLOCK

#define KEY5_PIN                  GPIO_PINS_2
#define KEY5_PORT                 GPIOD                                 
#define KEY5_CRM_CLK              CRM_GPIOD_PERIPH_CLOCK

#define KEY6_PIN                  GPIO_PINS_12
#define KEY6_PORT                 GPIOD                                 
#define KEY6_CRM_CLK              CRM_GPIOD_PERIPH_CLOCK

#define KEY7_PIN                  GPIO_PINS_2
#define KEY7_PORT                 GPIOB                                 
#define KEY7_CRM_CLK              CRM_GPIOB_PERIPH_CLOCK

#define KEY8_PIN                  GPIO_PINS_4
#define KEY8_PORT                 GPIOE                                 
#define KEY8_CRM_CLK              CRM_GPIOE_PERIPH_CLOCK
/**
  * @}
  */

/** @defgroup BOARD_exported_functions
  * @{
  */

#define time_after(a, b) ((S32)((S32)(b) - (S32)(a)) < 0)
#define Timer_PassedDelay(startTime, msDelay) (time_after(Timer_GetTickCount(), (startTime + msDelay)))
#define Timer_GetTickCount() (systick_ms)
extern volatile unsigned int systick_ms;
extern crm_clocks_freq_type crm_clocks_freq_struct;
/******************** functions ********************/
void at32_board_init(void);
KEY_Value_t key_pressed(key_type key);
/* led operation function */
void at32_led_init(led_type led);
void at32_led_on(led_type led);
void at32_led_off(led_type led);
void at32_led_toggle(led_type led);
void adc_config(void);
uint16_t  analogRead(void);

/* KEY operation function */
void at32_key_init(void);

uint8_t key_state(key_type key);



/* printf uart init function */
void uart_print_init(uint32_t baudrate);

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

