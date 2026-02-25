/**
 **************************************************************************
 * @file     main.c
 * @version  v2.0.0
 * @date     2020-11-02
 * @brief    main program
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

#include "at32f435_437_clock.h"
#include "at_surf_f437_board_lcd.h"
#include "at_surf_f437_board_usart.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "at_surf_f437_board_delay.h"
#include "at_surf_f437_board_touch.h"
#include "at32f435_437_board_qspi_flash.h"
#include "at_surf_f437_board_sdram.h"
#include "at32f435_437_board.h"

#include "lv_tick_custom.h"
#include <stdint.h>
#include <string.h>
#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "bsp.h"
#include "timers.h"
#include "queue.h"
#include "algorithms.h"

#include "bsp_uart.h"

#include "mywave_oled_api.h"
#include "page.h"
#include "displayChess.h"

#define USING_EXT_FLASH 0

/* 开始任务优先级 */
#define START_TASK_PRIO 1
/* 开始任务堆栈大小 */
#define START_STK_SIZE 128
/* 开始任务任务句柄 */
TaskHandle_t StartTask_Handler;
/* 开始任务入口函数 */
void start_task(void *pvParameters);

/* LCD1任务优先级 */
#define LVGL_TASK_PRIO 5
/* LCD1任务堆栈大小 */
#define LVGL_STK_SIZE 512
/* LCD1任务任务句柄 */
TaskHandle_t LVGLTask_Handler;
/* LCD1任务入口函数 */
void lvgl_task(void *pvParameters);

/* WAVE任务优先级 */
#define WAVE_TASK_PRIO 8
/* WAVE任务堆栈大小 */
#define WAVE_STK_SIZE 512
/* WAVE任务任务句柄 */
TaskHandle_t WAVETask_Handler;
/* WAVE任务入口函数 */
void wave_task(void *pvParameters);

/* ESP12任务优先级 */
#define ESP12_TASK_PRIO 1
/* ESP12任务堆栈大小 */
#define ESP12_STK_SIZE 512
/* ESP12任务任务句柄 */
TaskHandle_t ESP12Task_Handler;
/* ESP12任务入口函数 */
void esp12_task(void *pvParameters);

/* 浮点运算任务优先级 */
#define TICK10MS_TASK_PRIO 4
/* 浮点运算任务堆栈大小 */
#define TICK10MS_STK_SIZE 256
/* 浮点运算任务任务句柄 */
TaskHandle_t TICK10MSTask_Handler;
/* 浮点运算任务入口函数 */
void tick10ms_task(void *pvParameters);

TaskHandle_t ChessGameTask_Handler;
void ChessGame_task(void *pvParameters);

TaskHandle_t SortTask_Handler;

extern QueueHandle_t MutexSemaphore;

#define QUEUE_LENGTH 2000
#define QUEUE_ITEM_SIZE sizeof(int)
QueueHandle_t Queue_RequestData;

// 定时器用于每分钟获取一次天气数据
TimerHandle_t Timer1Timer_Handler;

lv_ui guider_ui;
extern PAGE pageflag;
#define accur 0.015295 // accur=18*3.3/4096（3.3/4096就是ADC采样精度，18是为了让波形转化一下能够显示在适当位子）

// 板子上的旋钮 ADC LVGL使用
static void adcUpdateCbk(void)
{
	u16 adcValue=0;
	
	switch(pageflag){
	case WEATHER:
		if(showflag){
			showflag=0;
			//printf("Humidity: %s\n", myweather.weather_phenomena);
//			lv_label_set_text_fmt(guider_ui.screen_humiddat_label, "%s", myweather.humidity);
//			lv_label_set_text_fmt(guider_ui.screen_temperdat_label, "%s", myweather.temperature);

//				lv_label_set_text_fmt(guider_ui.screen_weathdat_label, %s", myweather.weather_phenomena);

		}
		break;
	case WAVE:
		adcValue = analogRead();
		lv_label_set_text_fmt(guider_ui.screen_adc_label, "%04d", adcValue);
		lv_slider_set_value(guider_ui.screen_adc_slider,adcValue,LV_ANIM_OFF);
		break;
	case AICHAT:
		if(chatcount<10)
		{
			chatcount++;
		}
//		if(recvflag)
//		{
//			recvflag=0;
//			//lv_textarea_set_text(guider_ui.screen_ai_ta,"");
//			//printf("myaichat: %s", myaichat.txt);
//			printf("%s\n",myaichat.txt);
//			lv_textarea_set_text(guider_ui.screen_ai_ta, myaichat.txt);
//			
//		}
		if(SENDFLAG)
		{
			SENDFLAG=0;
			if(chatcount>=10){
				chatcount=0;
				const char *text = lv_textarea_get_text(guider_ui.screen_user_ta);
				char result[1024];
				snprintf(result, sizeof(result), "get_gpt%s", text);
				printf("拼接后的字符串： %s", result);
				ESP12_SendAT(result);
			}
		}
		break;
	case CHESS:
		printf("chess");
		break;
	default:break;
	}
}

/* wave开始任务 */
void wave_task(void *pvParameters)
{
  u16 x = 0;
  uint16_t ConvData = 0; // ADC采样数据

  while (1)
  {
    if (x < 128)
    {

      ConvData = analogRead();
      OLED_DrawWave(x, accur * ConvData); // 这是个画波形的函数
      // ConvData=0; 不注释就是柱形
      x = (x + 1) % 128;
    }
    vTaskDelay(10);
  }
}
unsigned char backflag;
void lvgl_task(void *pvParameters)
{
  /* for littlevgl gui tick increase */
  tmr7_int_init(287, 999); // 1 micro second interrupt

  lcd_init(LCD_DISPLAY_HORIZONTAL);

  touch_init(TOUCH_SCAN_HORIZONTAL);
  lcd_clear(BLACK);
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  // sdram_init();
  setup_ui(&guider_ui);
  events_init(&guider_ui);
  custom_init(&guider_ui);

  lv_timer_t *task_timer = lv_timer_create((lv_timer_cb_t)adcUpdateCbk, 1500, 0); // create a timer task with interval 1000ms
  if (task_timer == NULL)
  {
    printf("[%s:%d] lv_timer_create failed\n", __FUNCTION__, __LINE__);
  }

  while (1)
  {

    if (pageflag == CHESS)
    {
		lv_timer_pause(task_timer);
      xTaskCreate((TaskFunction_t)ChessGame_task,
                  (const char *)"ChessGame_task",
                  (uint16_t)512,
                  (void *)NULL,
                  (UBaseType_t)3,
                  (TaskHandle_t *)&ChessGameTask_Handler);
       vTaskSuspend(LVGLTask_Handler);

    }else if(pageflag==SORT)
	{
		lv_timer_pause(task_timer);
		  xTaskCreate((TaskFunction_t)bubbleSortVisualization_task,
                  (const char *)"bubbleSortVisualization_task",
                  (uint16_t)512,
                  (void *)NULL,
                  (UBaseType_t)3,
                  (TaskHandle_t *)&SortTask_Handler);
		vTaskSuspend(LVGLTask_Handler);
	}
	else{
		if(backflag==1)
		{
			backflag=0;
			
			lcd_init(LCD_DISPLAY_HORIZONTAL);
			touch_init(TOUCH_SCAN_HORIZONTAL);
			lcd_clear(BLACK);
			lv_init();
			lv_port_disp_init();
			lv_port_indev_init();
			ui_load_scr_animation(&guider_ui, &guider_ui.screen, guider_ui.screen_del, &guider_ui.screen_del, setup_scr_screen, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
			lv_timer_resume(task_timer);
			
		}
		xSemaphoreTake(MutexSemaphore, portMAX_DELAY); /* 获取互斥信号量 */
		lv_task_handler();
		xSemaphoreGive(MutexSemaphore); /* 释放互斥信号量 */

		
	}
    vTaskDelay(50);
	
  }
}
//uint8_t ucCount = 0;
//  u16 ucCount2=0;
// 按键扫描
static void tick10ms_task(void *pvParameters)
{
  
  int temp;
  while (1)
  {
    bsp_KeyScan10ms();
   // BEEP_Pro();
    temp = bsp_GetKey();
    switch (temp)
    {
    case KEY_7_DOWN:
      if (pageflag==CHESS)
    {
	 vTaskResume(LVGLTask_Handler);
      pageflag = HOME; 
	  backflag=1;
      vTaskDelete(ChessGameTask_Handler);
    }else if (pageflag==SORT){
		vTaskResume(LVGLTask_Handler);
      pageflag = HOME; 
	  backflag=1;
      vTaskDelete(SortTask_Handler);
	}
      break;
    default:
      break;
    }
//    if(++ucCount>100){
//		ucCount=0;
//		if(++ucCount2>60){
//			ucCount2=0;
//		
//		}printf("%d",ucCount2);
//	}
	
	parsing_OnePack();
    vTaskDelay(10);
  }
}
int queueflag;
void WeatherData_request()
{
  ESP12_SendAT("get_weather");
  printf("once\r\n");
}
void esp12_task(void *pvParameters)
{

  char oneData;
  esp12_connect();
  int count = 0;

  //  创建周期定时器
  Timer1Timer_Handler = xTimerCreate(
      (const char *)"Timer2",                        /* 定时器名 */
      (TickType_t)30000,                             /* 定时器超时时间 */
      (UBaseType_t)pdTRUE,                           /* 周期定时器 */
      (void *)2,                                     /* 定时器 ID */
      (TimerCallbackFunction_t)WeatherData_request); /* 定时器回调函数 */
  // 开启定时器
  xTimerStart((TimerHandle_t)Timer1Timer_Handler, (TickType_t)portMAX_DELAY);

  while (1)
  {
	parsing_OnePack();
//    if (comGetChar(COM3, &oneData))
//    {
//		queueflag = 1;
//		printf("%c", oneData);
//		xQueueSend(Queue_RequestData, &oneData, portMAX_DELAY);
//    }
    vTaskDelay(5);
  }
}

int main(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  bsp_Init();
  OLED_Init();

  uart_print_init(115200);

  /* 创建开始任务 */
  xTaskCreate((TaskFunction_t)start_task,
              (const char *)"start_task",
              (uint16_t)START_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)START_TASK_PRIO,
              (TaskHandle_t *)&StartTask_Handler);

  vTaskStartScheduler();
}

/* 开始任务函数 */
void start_task(void *pvParameters)
{
  /* 进入临界区 */
  taskENTER_CRITICAL();
	
  Queue_RequestData = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
  MutexSemaphore = xSemaphoreCreateMutex();

  /* 创建LVGL任务 */
  xTaskCreate((TaskFunction_t)lvgl_task,
              (const char *)"lvgl_task",
              (uint16_t)LVGL_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)LVGL_TASK_PRIO,
              (TaskHandle_t *)&LVGLTask_Handler);

  /*创建wave任务 */
  xTaskCreate((TaskFunction_t)wave_task,
              (const char *)"wave_task",
              (uint16_t)WAVE_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)WAVE_TASK_PRIO,
              (TaskHandle_t *)&WAVETask_Handler);
  


  xTaskCreate((TaskFunction_t)tick10ms_task,
              (const char *)"TICK10MS_Task",
              (uint16_t)TICK10MS_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TICK10MS_TASK_PRIO,
              (TaskHandle_t *)&TICK10MSTask_Handler);
// esp12任务
  xTaskCreate((TaskFunction_t)esp12_task,
              (const char *)"esp12_task",
              (uint16_t)ESP12_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)ESP12_TASK_PRIO,
              (TaskHandle_t *)&ESP12Task_Handler);
  /* 删除开始任务 */
  vTaskDelete(StartTask_Handler);
  /* 退出临界区 */
  taskEXIT_CRITICAL();
}
