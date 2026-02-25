/**
  **************************************************************************
  * @file     at32f435_437_board.c
  * @brief    set of firmware functions to manage leds and push-button.
  *           initialize delay function.
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

#include "at32f435_437_board.h"


/** @addtogroup AT32F435_437_board
  * @{
  */

/** @defgroup BOARD
  * @brief onboard periph driver
  * @{
  */
volatile uint32_t debug_timerTick;//任务运行时间统计使用  
static void delay_init(void);

volatile unsigned int systick_ms = 0; 
/* delay macros */
#define STEP_DELAY_MS                    50

/* at-start led resouce array */
gpio_type *led_gpio_port[LED_NUM]        = {LED3_GPIO, LED4_GPIO};
uint16_t led_gpio_pin[LED_NUM]           = { LED3_PIN, LED4_PIN};
crm_periph_clock_type led_gpio_crm_clk[LED_NUM] = {LED3_GPIO_CRM_CLK, LED4_GPIO_CRM_CLK};

/* delay variable */
static __IO uint32_t fac_us;
static __IO uint32_t fac_ms;

/* support printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
  __asm (".global __use_no_semihosting\n\t");
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
  FILE __stdout;
#else
 #ifdef __CC_ARM
  #pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  FILE __stdout;
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
 #endif
#endif

#if defined (__GNUC__) && !defined (__clang__)
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
  * @brief  retargets the c library printf function to the usart.
  * @param  none
  * @retval none
  */
PUTCHAR_PROTOTYPE
{
  while(usart_flag_get(PRINT_UART, USART_TDBE_FLAG) == RESET);
  usart_data_transmit(PRINT_UART, ch);
  return ch;
}

#if defined (__GNUC__) && !defined (__clang__)
int _write(int fd, char *pbuffer, int size)
{
  for(int i = 0; i < size; i ++)
  {
    __io_putchar(*pbuffer++);
  }

  return size;
}
#endif

/**
  * @brief  initialize uart
  * @param  baudrate: uart baudrate
  * @retval none
  */
void uart_print_init(uint32_t baudrate)
{
  gpio_init_type gpio_init_struct;

#if defined (__GNUC__) && !defined (__clang__)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif

  /* enable the uart and gpio clock */
  crm_periph_clock_enable(PRINT_UART_CRM_CLK, TRUE);
  crm_periph_clock_enable(PRINT_UART_TX_GPIO_CRM_CLK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the uart tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = PRINT_UART_TX_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PRINT_UART_TX_GPIO, &gpio_init_struct);
	
	 gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = PRINT_UART_RX_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(PRINT_UART_RX_GPIO, &gpio_init_struct);
	
	gpio_pin_mux_config(PRINT_UART_TX_GPIO, PRINT_UART_TX_PIN_SOURCE, PRINT_UART_TX_PIN_MUX_NUM);
	gpio_pin_mux_config(PRINT_UART_RX_GPIO, PRINT_UART_RX_PIN_SOURCE, PRINT_UART_RX_PIN_MUX_NUM);
	  /* config usart nvic interrupt */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(USART1_IRQn, 0, 0);

  /* configure uart param */
  usart_init(PRINT_UART, baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(PRINT_UART, TRUE);
	usart_receiver_enable(PRINT_UART, TRUE);
	usart_interrupt_enable(PRINT_UART, USART_RDBF_INT, TRUE);
  usart_enable(PRINT_UART, TRUE);
}


/**
  * @brief  this function handles timer1 overflow handler.
  * @param  none
  * @retval none
  */
void TMR1_OVF_TMR10_IRQHandler(void)
{
  if(tmr_flag_get(TMR1, TMR_OVF_FLAG) != RESET)
  {
    systick_ms++;
    debug_timerTick++;
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
}
crm_clocks_freq_type crm_clocks_freq_struct = {0};
void timer1Init(void)
{
    /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);
    /* enable tmr1 clock */
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  /* tmr1 configuration */
  /* time base configuration */
  /* systemclock/28800/10000 = 1hz */
  // the timer and divider is 16-bits width, hence the divider parameter should be 16 bits 
  tmr_base_init(TMR1, 9, (crm_clocks_freq_struct.ahb_freq / 10000) - 1);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);

  /* tmr1 overflow interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 0, 0);

  /* enable tmr1 */
  tmr_counter_enable(TMR1, TRUE);
}
/**
  * @brief  board initialize interface init led and button
  * @param  none
  * @retval none
  */
void at32_board_init()
{
  /* initialize delay function */
  delay_init();
  timer1Init();
  uart_print_init(115200);
  adc_config();
  

  /* configure led in at_start_board */
  at32_led_init(LED3);
  at32_led_init(LED4);
  at32_led_off(LED3);
  at32_led_off(LED4);

  /* configure button in at_start board */
  at32_key_init();
}

/**
  * @brief  configure button gpio
  * @param  button: specifies the button to be configured.
  * @retval none
  */
void at32_key_init(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the button clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure KEY pin as input with pull-up/pull-down */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_2|GPIO_PINS_13|GPIO_PINS_14|GPIO_PINS_15;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init(GPIOB, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = GPIO_PINS_1|GPIO_PINS_4;
  gpio_init(GPIOE, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = GPIO_PINS_2|GPIO_PINS_12;
  gpio_init(GPIOD, &gpio_init_struct);

}

uint8_t key_state(key_type key)
{
  switch(key)
  {
    case KEY1: return gpio_input_data_bit_read(KEY1_PORT, KEY1_PIN);
    case KEY2: return gpio_input_data_bit_read(KEY2_PORT, KEY2_PIN);
    case KEY3: return gpio_input_data_bit_read(KEY3_PORT, KEY3_PIN);
    case KEY4: return gpio_input_data_bit_read(KEY4_PORT, KEY4_PIN);
    case KEY5: return gpio_input_data_bit_read(KEY5_PORT, KEY5_PIN);
    case KEY6: return gpio_input_data_bit_read(KEY6_PORT, KEY6_PIN);
    case KEY7: return gpio_input_data_bit_read(KEY7_PORT, KEY7_PIN);
    case KEY8: return gpio_input_data_bit_read(KEY8_PORT, KEY8_PIN);
  }
   
}

/**
  * @brief  adc configuration.
  * @param  none
  * @retval none
  */
void adc_config(void)
{
  gpio_init_type gpio_initstructure;
  adc_common_config_type adc_common_struct;
  adc_base_config_type adc_base_struct;
  
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);

  /* config adc pin as analog input mode */
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = GPIO_PINS_0;
  gpio_init(GPIOA, &gpio_initstructure);
  
  

  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(ADC1_2_3_IRQn, 0, 0);

  adc_common_default_para_init(&adc_common_struct);

  /* config combine mode */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* config division,adcclk is division by hclk */
  adc_common_struct.div = ADC_HCLK_DIV_4;

  /* config common dma mode,it's not useful in independent mode */
  adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;

  /* config common dma request repeat */
  adc_common_struct.common_dma_request_repeat_state = FALSE;

  /* config adjacent adc sampling interval,it's useful for ordinary shifting mode */
  adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;

  /* config inner temperature sensor and vintrv */
  adc_common_struct.tempervintrv_state = FALSE;

  /* config voltage battery */
  adc_common_struct.vbat_state = FALSE;
  adc_common_config(&adc_common_struct);

  adc_base_default_para_init(&adc_base_struct);

  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 1;
  adc_base_config(ADC1, &adc_base_struct);
  adc_resolution_set(ADC1, ADC_RESOLUTION_12B);

  /* config ordinary channel */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_640_5);

  /* config ordinary trigger source and trigger edge */
  adc_ordinary_conversion_trigger_set(ADC1, ADC_ORDINARY_TRIG_TMR1CH1, ADC_ORDINARY_TRIG_EDGE_NONE);

  /* config dma mode,it's not useful when common dma mode is use */
  adc_dma_mode_enable(ADC1, FALSE);

  /* config dma request repeat,it's not useful when common dma mode is use */
  adc_dma_request_repeat_enable(ADC1, FALSE);

  /* each ordinary channel conversion set occe flag */
  adc_occe_each_conversion_enable(ADC1, TRUE);

  /* enable adc overflow interrupt */
  adc_interrupt_enable(ADC1, ADC_OCCO_INT, TRUE);

  /* adc enable */
  adc_enable(ADC1, TRUE);
  while(adc_flag_get(ADC1, ADC_RDY_FLAG) == RESET);

  /* adc calibration */
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}


uint16_t  analogRead(void)
{
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
  while(adc_flag_get(ADC1, ADC_OCCE_FLAG) == RESET);
  return  adc_ordinary_conversion_data_get(ADC1);
  
}

/**
  * @brief  configure led gpio
  * @param  led: specifies the led to be configured.
  * @retval none
  */
void at32_led_init(led_type led)
{
  gpio_init_type gpio_init_struct;

  /* enable the led clock */
  crm_periph_clock_enable(led_gpio_crm_clk[led], TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure the led gpio */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = led_gpio_pin[led];
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(led_gpio_port[led], &gpio_init_struct);
}

/**
  * @brief  turns selected led on.
  * @param  led: specifies the led to be set on.
  *   this parameter can be one of following parameters:
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval none
  */
void at32_led_on(led_type led)
{
  if(led > (LED_NUM - 1))
    return;
  if(led_gpio_pin[led])
    led_gpio_port[led]->clr = led_gpio_pin[led];
}

/**
  * @brief  turns selected led off.
  * @param  led: specifies the led to be set off.
  *   this parameter can be one of following parameters:
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval none
  */
void at32_led_off(led_type led)
{
  if(led > (LED_NUM - 1))
    return;
  if(led_gpio_pin[led])
    led_gpio_port[led]->scr = led_gpio_pin[led];
}

/**
  * @brief  turns selected led toggle.
  * @param  led: specifies the led to be set off.
  *   this parameter can be one of following parameters:
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval none
  */
void at32_led_toggle(led_type led)
{
  if(led > (LED_NUM - 1))
    return;
  if(led_gpio_pin[led])
    led_gpio_port[led]->odt ^= led_gpio_pin[led];
}

/**
  * @brief  initialize delay function
  * @param  none
  * @retval none
  */

void delay_init(void)
{
  /* configure systick */
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);
  fac_us = system_core_clock / (1000000U);
  fac_ms = fac_us * (1000U);
}

/**
  * @brief  inserts a delay time.
  * @param  nus: specifies the delay time length, in microsecond.
  * @retval none
  */
void delay_us(uint32_t nus)
{
  uint32_t temp = 0;
  SysTick->LOAD = (uint32_t)(nus * fac_us);
  SysTick->VAL = 0x00;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;
  do
  {
    temp = SysTick->CTRL;
  }while((temp & 0x01) && !(temp & (1 << 16)));

  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  SysTick->VAL = 0x00;
}

/**
  * @brief  inserts a delay time.
  * @param  nms: specifies the delay time length, in milliseconds.
  * @retval none
  */
void delay_ms(uint16_t nms)
{
  uint32_t temp = 0;
  while(nms)
  {
    if(nms > STEP_DELAY_MS)
    {
      SysTick->LOAD = (uint32_t)(STEP_DELAY_MS * fac_ms);
      nms -= STEP_DELAY_MS;
    }
    else
    {
      SysTick->LOAD = (uint32_t)(nms * fac_ms);
      nms = 0;
    }
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
      temp = SysTick->CTRL;
    }while((temp & 0x01) && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
  }
}

/**
  * @brief  inserts a delay time.
  * @param  sec: specifies the delay time, in seconds.
  * @retval none
  */
void delay_sec(uint16_t sec)
{
  uint16_t index;
  for(index = 0; index < sec; index++)
  {
    delay_ms(500);
    delay_ms(500);
  }
}

/**
  * @}
  */

/**
  * @}
  */

