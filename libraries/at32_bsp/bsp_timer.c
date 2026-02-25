/*
    bsp——timer.c
*
*********************************************************************************************************
*/
#include "bsp.h"

typedef signed long      S32;


volatile unsigned int systick_ms = 0; 
volatile uint32_t debug_timerTick;//任务运行时间统计使用  

/* 保存 TIM定时中断到后执行的回调函数指针 */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);


/* 这2个全局变量转用于 bsp_DelayMS() 函数 */
static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t s_ucTimeOutFlag = 0;

/* 定于软件定时器结构体变量 */
static SOFT_TMR s_tTmr[TMR_COUNT] = {0};

static void bsp_SoftTimerDec(SOFT_TMR *_tmr);

extern void bsp_RunPer1ms(void);
extern void bsp_RunPer10ms(void);
void TMR5_GLOBAL_IRQHandler(void)
{
  /* TMR3_CH1 toggling with frequency = 2000Hz */
  if(tmr_flag_get(TMR5, TMR_C1_FLAG) != RESET)
  {
    tmr_flag_clear(TMR5, TMR_C1_FLAG );
    tmr_counter_value_set(TMR5,0);
  }
}
tmr_output_config_type tmr_oc_init_structure;
crm_clocks_freq_type crm_clocks_freq_struct = {0};
void timer5_init(void)
{
   /* compute the prescaler value */
  int prescaler_value;
	gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);
  
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;

  gpio_init_struct.gpio_pins = GPIO_PINS_12;//buzz
  gpio_init(GPIOB, &gpio_init_struct);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE12, GPIO_MUX_2);  
  crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);

  /* gpioa gpiob clock enable */
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	
	
	nvic_irq_enable(TMR5_GLOBAL_IRQn, 0, 0);
    /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);
	
	prescaler_value = (uint16_t) ((crm_clocks_freq_struct.apb1_freq * 2) / 1000000) - 1;  //1MHz
  /* tmre base configuration */
  tmr_base_init(TMR5, 65535, prescaler_value);
  tmr_cnt_dir_set(TMR5, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR5, TMR_CLOCK_DIV1);
  
	tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_SWITCH;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_oc_init_structure.oc_output_state = TRUE;
  
  tmr_output_channel_config(TMR5, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR5, TMR_SELECT_CHANNEL_1, 500);
  
  //tmr_output_channel_buffer_enable(TMR5, TMR_SELECT_CHANNEL_1, TRUE);
    /* tmr enable counter */
  tmr_counter_enable(TMR5, FALSE);

	  /* tmr it enable */
  tmr_interrupt_enable(TMR5, TMR_C1_INT, FALSE);
}
void tone(int freq)
{
   tmr_channel_value_set(TMR5, TMR_SELECT_CHANNEL_1, 1000000/(2*freq));
	 tmr_counter_enable(TMR5, TRUE);
	  /* tmr it enable */
  tmr_interrupt_enable(TMR5, TMR_C1_INT, TRUE);

}
void toneOff(void)
{
  tmr_counter_enable(TMR5, FALSE);

	  /* tmr it enable */
  tmr_interrupt_enable(TMR5, TMR_C1_INT, FALSE);
}
/*
*********************************************************************************************************
*	函 数 名: TMR7_GLOBAL_IRQHandler
*	功能说明: 定时中断服务程序，每隔1ms进入1次
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void TMR6_DAC_GLOBAL_IRQHandler(void)
{
	static uint8_t s_count = 0;
	uint8_t i;
	if(tmr_flag_get(TMR6, TMR_OVF_FLAG) != RESET)
  { 
    debug_timerTick++;
    /* 每隔1ms进来1次 （仅用于 bsp_DelayMS） */
    if (s_uiDelayCount > 0)
    {
      if (--s_uiDelayCount == 0)
      {
        s_ucTimeOutFlag = 1;
      }
    }

	/* 每隔1ms，对软件定时器的计数器进行减一操作 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&s_tTmr[i]);
	}

	systick_ms++;

	bsp_RunPer1ms();		/* 每隔1ms调用一次此函数，此函数在 bsp.c */

	if (++s_count >= 10)
	{
		s_count = 0;

		bsp_RunPer10ms();	/* 每隔10ms调用一次此函数，此函数在 bsp.c */
	}
	 tmr_flag_clear(TMR6, TMR_OVF_FLAG);
  }
	
}


// Timer6 初始化 产生1ms中断
void timer6_init(void)
{
	crm_clocks_freq_type crm_clocks_freq_struct = {0};
    /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);
	
	    /* enable tmr1 clock */
  crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
  /* tmr9 configuration */
  /* time base configuration */
  /* systemclock/28800/10000 = 1hz */
  // the timer and divider is 16-bits width, hence the divider parameter should be 16 bits 
  tmr_base_init(TMR6, 9, (crm_clocks_freq_struct.ahb_freq / 10000) - 1);  //1ms interrupr
  tmr_cnt_dir_set(TMR6, TMR_COUNT_UP);
  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR6, TMR_OVF_INT, TRUE);
  /* tmr1 overflow interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR6_DAC_GLOBAL_IRQn, 1, 1);
  /* enable tmr1 */
  tmr_counter_enable(TMR6, TRUE);
}



/*
*********************************************************************************************************
*	函 数 名: bsp_InitTimer
*	功能说明: 配置systick中断，并初始化软件定时器变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitTimer(void)
{
	uint8_t i;

	/* clear all the softtimer */
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;	/* 缺省是1次性工作模式 */
	}

	timer6_init(); //1ms interrput
}
/**
  * @brief  inserts a delay time.
  * @param  nms: specifies the delay time length, in milliseconds.
  * @retval none
  */
void delay_ms(int nms)
{
  volatile int delayTimer = Timer_GetTickCount();
  while(!Timer_PassedDelay(delayTimer, nms));

}
/*
*********************************************************************************************************
*	函 数 名: bsp_SoftTimerDec
*	功能说明: 每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*	形    参:  _tmr : 定时器变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1;

			/* 如果是自动模式，则自动重装计数器 */
			if(_tmr->Mode == TMR_AUTO_MODE)
			{
				_tmr->Count = _tmr->PreLoad;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_StartTimer
*	功能说明: 启动一个定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  			/* 关中断 */

	s_tTmr[_id].Count = _period;		/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 1次性工作模式 */

	ENABLE_INT();  				/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_StartAutoTimer
*	功能说明: 启动一个自动定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位10ms
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
#if uCOS_EN == 1
	CPU_SR_ALLOC();
#endif
	
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  		/* 关中断 */

	s_tTmr[_id].Count = _period;			/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_AUTO_MODE;	/* 自动工作模式 */

	ENABLE_INT();  			/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_StopTimer
*	功能说明: 停止一个定时器
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_StopTimer(uint8_t _id)
{
#if uCOS_EN == 1
	CPU_SR_ALLOC();
#endif
	
	if (_id >= TMR_COUNT)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  	/* 关中断 */

	s_tTmr[_id].Count = 0;				/* 实时计数器初值 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 自动工作模式 */

	ENABLE_INT();  		/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_CheckTimer
*	功能说明: 检测定时器是否超时
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 返回 0 表示定时未到， 1表示定时到
*********************************************************************************************************
*/
uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (s_tTmr[_id].Flag == 1)
	{
		s_tTmr[_id].Flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

