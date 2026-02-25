/*
*********************************************************************************************************
*
*	模块名称 : 蜂鸣器驱动模块
*	文件名称 : bsp_beep.c
*	版    本 : V1.1
*	说    明 : 驱动蜂鸣器.
*
*********************************************************************************************************
*/

#include "bsp.h"


/* 无源蜂鸣器 */  
  //PB12   TMR5_CH1
	/* 1500表示频率1.5KHz，5000表示50.00%的占空比 */
	#define BEEP_ENABLE()	tmr_counter_enable(TMR5, TRUE)

	/* 禁止蜂鸣器鸣叫 */
	#define BEEP_DISABLE() tmr_counter_enable(TMR5, FALSE)

BEEP_T g_tBeep;		/* 定义蜂鸣器全局结构体变量 */

/*
*********************************************************************************************************
*	函 数 名: BEEP_InitHard
*	功能说明: 初始化蜂鸣器硬件
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#define PWWTIMER  TMR5
static uint16_t prescaler_value = 0;

void BEEP_SetFreq(int freq)
{
   tmr_base_init(PWWTIMER, 1000000UL/freq, prescaler_value);
}
void BEEP_InitHard(void)
{
  
  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_oc_init_structure;
  
	g_tBeep.ucMute = 0;	/* 关闭静音 */
  
  crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);   

  gpio_default_para_init(&gpio_init_struct);  
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_12;  
  gpio_init(GPIOB, &gpio_init_struct);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE12, GPIO_MUX_2);
  
   /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);
    /* compute the prescaler value  */  //the freq is 15MHz
  prescaler_value = (uint16_t)(crm_clocks_freq_struct.apb1_freq * 2 / 1000000) - 1;

  /* tmr5 time base configuration */ //the PWM freq is 15KHz
  tmr_base_init(PWWTIMER, 999, prescaler_value);
  tmr_cnt_dir_set(PWWTIMER, TMR_COUNT_UP);
  tmr_clock_source_div_set(PWWTIMER, TMR_CLOCK_DIV1);
  
  tmr_output_default_para_init(&tmr_oc_init_structure);	  
  
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;  
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;
   
  tmr_output_channel_config(PWWTIMER, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);  
  tmr_channel_value_set(PWWTIMER, TMR_SELECT_CHANNEL_1, 500);
  
  tmr_output_channel_buffer_enable(PWWTIMER, TMR_SELECT_CHANNEL_1, TRUE);
  tmr_period_buffer_enable(TMR5, TRUE);
  /* tmr enable counter */
  tmr_counter_enable(PWWTIMER, FALSE);  
}

/*
*********************************************************************************************************
*	函 数 名: BEEP_Start
*	功能说明: 启动蜂鸣音。
*	形    参: _usBeepTime : 蜂鸣时间，单位10ms; 0 表示不鸣叫
*			  _usStopTime : 停止时间，单位10ms; 0 表示持续鸣叫
*			  _usCycle : 鸣叫次数， 0 表示持续鸣叫
*	返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Start(uint16_t _usBeepTime, uint16_t _usStopTime, uint16_t _usCycle)
{
	if (_usBeepTime == 0 || g_tBeep.ucMute == 1)
	{
		return;
	}

	g_tBeep.usBeepTime = _usBeepTime;
	g_tBeep.usStopTime = _usStopTime;
	g_tBeep.usCycle = _usCycle;
	g_tBeep.usCount = 0;
	g_tBeep.usCycleCount = 0;
	g_tBeep.ucState = 0;
	g_tBeep.ucEnalbe = 1;	/* 设置完全局参数后再使能发声标志 */

	BEEP_ENABLE();			/* 开始发声 */
}

/*
*********************************************************************************************************
*	函 数 名: BEEP_Stop
*	功能说明: 停止蜂鸣音。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Stop(void)
{
	g_tBeep.ucEnalbe = 0;

	if ((g_tBeep.usStopTime == 0) || (g_tBeep.usCycle == 0))
	{
		BEEP_DISABLE();	/* 必须在清控制标志后再停止发声，避免停止后在中断中又开启 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: BEEP_Pause
*	功能说明: 由于TIM冲突等原因，临时屏蔽蜂鸣音。通过 BEEP_Resume 恢复
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Pause(void)
{
	BEEP_Stop();
	
	g_tBeep.ucMute = 1;		/* 静音 */
}

/*
*********************************************************************************************************
*	函 数 名: BEEP_Resume
*	功能说明: 恢复蜂鸣器正常功能
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Resume(void)
{
	BEEP_Stop();
	
	g_tBeep.ucMute = 0;		/* 静音 */
}

/*
*********************************************************************************************************
*	函 数 名: BEEP_KeyTone
*	功能说明: 发送按键音
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BEEP_KeyTone(void)
{
	BEEP_Start(5, 1, 1);	/* 鸣叫50ms，停10ms， 1次 */
}

/*
*********************************************************************************************************
*	函 数 名: BEEP_Pro
*	功能说明: 每隔10ms调用1次该函数，用于控制蜂鸣器发声。该函数在 bsp_timer.c 中被调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Pro(void)
{
	if ((g_tBeep.ucEnalbe == 0) || (g_tBeep.usStopTime == 0) || (g_tBeep.ucMute == 1))
	{
		return;
	}

	if (g_tBeep.ucState == 0)
	{
		if (g_tBeep.usStopTime > 0)	/* 间断发声 */
		{
			if (++g_tBeep.usCount >= g_tBeep.usBeepTime)
			{
				BEEP_DISABLE();		/* 停止发声 */
				g_tBeep.usCount = 0;
				g_tBeep.ucState = 1;
			}
		}
		else
		{
			;	/* 不做任何处理，连续发声 */
		}
	}
	else if (g_tBeep.ucState == 1)
	{
		if (++g_tBeep.usCount >= g_tBeep.usStopTime)
		{
			/* 连续发声时，直到调用stop停止为止 */
			if (g_tBeep.usCycle > 0)
			{
				if (++g_tBeep.usCycleCount >= g_tBeep.usCycle)
				{
					/* 循环次数到，停止发声 */
					g_tBeep.ucEnalbe = 0;
				}

				if (g_tBeep.ucEnalbe == 0)
				{
					g_tBeep.usStopTime = 0;
					return;
				}
			}

			g_tBeep.usCount = 0;
			g_tBeep.ucState = 0;

			BEEP_ENABLE();			/* 开始发声 */
		}
	}
}


