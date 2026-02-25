/*
* Copyright 2025 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


#include "page.h"
unsigned char chatcount;
unsigned char led4_status=0;
#include "at32f435_437_board.h"
unsigned char led3_status=0;
#include "bsp_lcd.h"
    static double prev_value = 0;
    static char current_operator = '\0';
    static char current_input[32] = {0};
    static double string_to_double(const char *str) {
    double value = 0.0;
    double fraction = 1.0;
    int is_decimal = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str) {
        if (*str == '.') {
            is_decimal = 1;
            str++;
            continue;
        }

        if (*str >= '0' && *str <= '9') {
            if (is_decimal) {
                fraction *= 0.1;
                value += (*str - '0') * fraction;
            } else {
                value = value * 10.0 + (*str - '0');
            }
        }
        str++;
    }
    
    return sign * value;
}


static void screen_cont_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		// if(pageflag==AICHAT)
	// {
	//    chatcount=20;
	//    //加载AI_Chat页面有的东西
	//     lv_obj_clear_flag(guider_ui.screen_ai_ta,LV_OBJ_FLAG_HIDDEN);
	//     lv_obj_clear_flag(guider_ui.screen_user_ta,LV_OBJ_FLAG_HIDDEN);
	//     lv_obj_clear_flag(guider_ui.screen_ai_label,LV_OBJ_FLAG_HIDDEN);
	//   lv_obj_set_pos(guider_ui.screen_user_ta, 13, 242);
	// }
		break;
	}
	default:
		break;
	}
}
static void screen_user_ta_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		lv_obj_add_flag(guider_ui.screen_ai_label, LV_OBJ_FLAG_HIDDEN);//隐藏对象
		lv_obj_add_flag(guider_ui.screen_ai_ta, LV_OBJ_FLAG_HIDDEN);//隐藏对象	
		//上移输入框
		lv_obj_set_pos(guider_ui.screen_user_ta, 52, 63);
		break;
	}
	default:
		break;
	}
}
static void screen_led4_sw_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		        led4_status=gpio_output_data_bit_read(LED4_GPIO,LED4_PIN)==0?1:0;
			if(led4_status)
			{
			    at32_led_off(LED4);
			}
			else
			{
			    at32_led_on(LED4);
			}
		lv_obj_t * status_obj = lv_event_get_target(e);
		int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? 1 : 0;
		switch(status) {
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}
static void screen_led3_sw_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		        led3_status=gpio_output_data_bit_read(LED3_GPIO,LED3_PIN)==0?1:0;
			if(led3_status)
			{
			    at32_led_off(LED3);
			}
			else
			{
			    at32_led_on(LED3);
			}
		lv_obj_t * status_obj = lv_event_get_target(e);
		int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? 1 : 0;
		switch(status) {
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}
static void screen_chess_img_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		    pageflag=CHESS;
	    
		break;
	}
	default:
		break;
	}
}
static void screen_weather_img_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		pageflag=WEATHER;
	    // //隐藏对象
	    // lv_obj_add_flag(guider_ui.screen_chat_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_chess_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_sort_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_wave_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_weather_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_bsp_img, LV_OBJ_FLAG_HIDDEN);
	    
	    // //设置页面标题
	    // lv_label_set_text(guider_ui.screen_head_label, "Weather");
	    
	    // //打开返回键
	    // lv_obj_clear_flag(guider_ui.screen_back_img,LV_OBJ_FLAG_HIDDEN);
	
	    // lv_obj_clear_flag(guider_ui.screen_weathdat_label,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_weath_img,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_humiddat_label,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_humid_img,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_temperdat_label,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_temper_img,LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(guider_ui.screen_calendar_1, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(guider_ui.screen_cont_2, LV_OBJ_FLAG_HIDDEN);
		break;
	}
	default:
		break;
	}
}
static void screen_wave_img_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		pageflag=WAVE;
	    //隐藏对象
	    lv_obj_add_flag(guider_ui.screen_chat_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_chess_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_sort_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_wave_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_weather_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_bsp_img, LV_OBJ_FLAG_HIDDEN);
	    
	    //设置页面标题
	    lv_label_set_text(guider_ui.screen_head_label, "Wave");
	    
	    //打开返回键
	    lv_obj_clear_flag(guider_ui.screen_back_img,LV_OBJ_FLAG_HIDDEN);
	
	    lv_obj_clear_flag(guider_ui.screen_adc_label,LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_adc_slider,LV_OBJ_FLAG_HIDDEN);
	
		break;
	}
	default:
		break;
	}
}
static void screen_sort_img_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		pageflag=SORT;
	    lcd_clear(BLACK);
		break;
	}
	default:
		break;
	}
}
static void screen_back_img_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		pageflag=HOME;
	    lv_label_set_text(guider_ui.screen_head_label, "HOME");
	    //隐藏
	    lv_obj_add_flag(guider_ui.screen_ai_ta, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_user_ta, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_ai_label, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_user_btn, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_back_img, LV_OBJ_FLAG_HIDDEN);
	
	    lv_obj_add_flag(guider_ui.screen_led3_sw, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_led4_sw, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_led3_label, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_led4_label, LV_OBJ_FLAG_HIDDEN);
	
	    lv_obj_add_flag(guider_ui.screen_adc_label,LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_adc_slider,LV_OBJ_FLAG_HIDDEN);
	
	    lv_obj_add_flag(guider_ui.screen_weathdat_label,LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_weath_img,LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_humiddat_label,LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_humid_img,LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_temperdat_label,LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_temper_img,LV_OBJ_FLAG_HIDDEN);
	
	    //取消隐藏
	     lv_obj_clear_flag(guider_ui.screen_chat_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_chess_img, LV_OBJ_FLAG_HIDDEN);
	     lv_obj_clear_flag(guider_ui.screen_sort_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_wave_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_weather_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_bsp_img, LV_OBJ_FLAG_HIDDEN);
		break;
	}
	default:
		break;
	}
}
static void screen_chat_img_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		pageflag=AICHAT;
	    // //隐藏对象
	    // lv_obj_add_flag(guider_ui.screen_chat_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_chess_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_sort_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_wave_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_weather_img, LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_add_flag(guider_ui.screen_bsp_img, LV_OBJ_FLAG_HIDDEN);
	    
	    // //设置页面标题
	    // lv_label_set_text(guider_ui.screen_head_label, "AI Chat");
	    
	    // //打开返回键
	    // lv_obj_clear_flag(guider_ui.screen_back_img,LV_OBJ_FLAG_HIDDEN);
	    
	    // //加载AI_Chat页面有的东西
	    // lv_obj_clear_flag(guider_ui.screen_ai_ta,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_user_ta,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_ai_label,LV_OBJ_FLAG_HIDDEN);
	    // lv_obj_clear_flag(guider_ui.screen_user_btn,LV_OBJ_FLAG_HIDDEN);
	    
	    // //重置用户对话框的位置
	    // lv_obj_set_pos(guider_ui.screen_user_ta, 13, 242);
		lv_obj_clear_flag(guider_ui.screen_cont_2, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(guider_ui.screen_cont_1, LV_OBJ_FLAG_HIDDEN);
		break;
	}
	default:
		break;
	}
}
static void screen_bsp_img_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		pageflag=BSP_CRTL;
	    
	    //隐藏对象
	    lv_obj_add_flag(guider_ui.screen_chat_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_chess_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_sort_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_wave_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_weather_img, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_add_flag(guider_ui.screen_bsp_img, LV_OBJ_FLAG_HIDDEN);
	
	    lv_obj_clear_flag(guider_ui.screen_led3_sw, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_led4_sw, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_led3_label, LV_OBJ_FLAG_HIDDEN);
	    lv_obj_clear_flag(guider_ui.screen_led4_label, LV_OBJ_FLAG_HIDDEN);
	
	    lv_obj_clear_flag(guider_ui.screen_back_img, LV_OBJ_FLAG_HIDDEN);
	  //设置页面标题
	    lv_label_set_text(guider_ui.screen_head_label, "Bsp");
		break;
	}
	default:
		break;
	}
}
static void screen_user_btn_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		SENDFLAG=1;
	    printf("send!");
		break;
	}
	default:
		break;
	}
}
static void screen_btnm_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		
	        uint16_t btn_id = lv_btnmatrix_get_selected_btn(guider_ui.screen_btnm_1);
	        const char * txt = lv_btnmatrix_get_btn_text(guider_ui.screen_btnm_1, btn_id);
	                if (strcmp(txt, "C") == 0) { // 清除
	            prev_value = 0;
	            current_operator = '\0';
	            memset(current_input, 0, sizeof(current_input));
	            lv_textarea_set_text(guider_ui.screen_ta_1, "");
	        }
	
	     else if (strchr("+-*/", txt[0]) != NULL) { // 运算符
	    if(current_input[0] != '\0') {
	        prev_value = string_to_double(current_input);
	        lv_textarea_add_text(guider_ui.screen_ta_1, txt);  // 新增：显示运算符
	        memset(current_input, 0, sizeof(current_input));
	    }
	    current_operator = txt[0];
	          }
	        else { // 数字或小数点
	            if(strlen(current_input) < sizeof(current_input)-1) {
	                strcat(current_input, txt);
	                lv_textarea_add_text(guider_ui.screen_ta_1, txt);
	            }
	        }
	
	          if (strcmp(txt, "=") == 0) { // 计算结果
	    double current_value = string_to_double(current_input);
	    double result = prev_value;
	    
	    switch(current_operator) {
	        case '+': result += current_value; break;
	        case '-': result -= current_value; break;
	        case '*': result *= current_value; break;
	        case '/': 
	            if(current_value == 0) {
	                lv_textarea_set_text(guider_ui.screen_ta_1, "Error");
	                return;
	            }
	            result /= current_value; 
	            break;
	        default: result = current_value; break;
	    }
	    
	    // 替换为标准库实现
	    static char result_str[32];
	    snprintf(result_str, sizeof(result_str), "%.6f", result);
	    
	    lv_textarea_set_text(guider_ui.screen_ta_1, "");
	    lv_textarea_add_text(guider_ui.screen_ta_1, result_str);
	    prev_value = result;
	    current_operator = '\0';
	    memset(current_input, 0, sizeof(current_input));
	}
	
	
	
		lv_obj_t * obj = lv_event_get_target(e);
		uint32_t id = lv_btnmatrix_get_selected_btn(obj);
		switch(id) {
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}
static void screen_ta_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_FOCUSED:
	{
		lv_obj_set_style_text_font(guider_ui.screen_ta_1, &lv_font_montserratMedium_18, 0);
		lv_textarea_set_text(guider_ui.screen_ta_1, "");
		break;
	}
	default:
		break;
	}
}
static void screen_btn_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_CLICKED:
	{
		lv_obj_add_flag(guider_ui.screen_cont_2, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(guider_ui.screen_cont_1, LV_OBJ_FLAG_HIDDEN);
		break;
	}
	default:
		break;
	}
}
static void screen_calendar_1_event_handler (lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	switch (code) {
	case LV_EVENT_LONG_PRESSED:
	{
		lv_obj_add_flag(guider_ui.screen_calendar_1, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(guider_ui.screen_cont_1, LV_OBJ_FLAG_HIDDEN);
		break;
	}
	default:
		break;
	}
}
void events_init_screen(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_cont_1, screen_cont_1_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_user_ta, screen_user_ta_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_led4_sw, screen_led4_sw_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_led3_sw, screen_led3_sw_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_chess_img, screen_chess_img_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_weather_img, screen_weather_img_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_wave_img, screen_wave_img_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_sort_img, screen_sort_img_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_back_img, screen_back_img_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_chat_img, screen_chat_img_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_bsp_img, screen_bsp_img_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_user_btn, screen_user_btn_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_btnm_1, screen_btnm_1_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_ta_1, screen_ta_1_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_btn_1, screen_btn_1_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->screen_calendar_1, screen_calendar_1_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{

}
