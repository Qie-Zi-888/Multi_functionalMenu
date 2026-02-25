/*
* Copyright 2025 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_background_img;
	lv_obj_t *screen_cont_1;
	lv_obj_t *screen_led4_label;
	lv_obj_t *screen_led3_label;
	lv_obj_t *screen_head_label;
	lv_obj_t *screen_user_ta;
	lv_obj_t *screen_ai_ta;
	lv_obj_t *screen_ai_label;
	lv_obj_t *screen_led4_sw;
	lv_obj_t *screen_led3_sw;
	lv_obj_t *screen_chess_img;
	lv_obj_t *screen_weather_img;
	lv_obj_t *screen_wave_img;
	lv_obj_t *screen_sort_img;
	lv_obj_t *screen_back_img;
	lv_obj_t *screen_chat_img;
	lv_obj_t *screen_bsp_img;
	lv_obj_t *screen_adc_label;
	lv_obj_t *screen_adc_slider;
	lv_obj_t *screen_user_btn;
	lv_obj_t *screen_user_btn_label;
	lv_obj_t *screen_temper_img;
	lv_obj_t *screen_humid_img;
	lv_obj_t *screen_weath_img;
	lv_obj_t *screen_temperdat_label;
	lv_obj_t *screen_humiddat_label;
	lv_obj_t *screen_weathdat_label;
	lv_obj_t *screen_cont_2;
	lv_obj_t *screen_btnm_1;
	lv_obj_t *screen_ta_1;
	lv_obj_t *screen_btn_1;
	lv_obj_t *screen_btn_1_label;
	lv_obj_t *screen_calendar_1;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_move_animation(void * var, int32_t duration, int32_t delay, int32_t x_end, int32_t y_end, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_scale_animation(void * var, int32_t duration, int32_t delay, int32_t width, int32_t height, lv_anim_path_cb_t path_cb,
                        uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                        lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_img_zoom_animation(void * var, int32_t duration, int32_t delay, int32_t zoom, lv_anim_path_cb_t path_cb,
                           uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                           lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void ui_img_rotate_animation(void * var, int32_t duration, int32_t delay, lv_coord_t x, lv_coord_t y, int32_t rotate,
                   lv_anim_path_cb_t path_cb, uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time,
                   uint32_t playback_delay, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);

void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);


extern lv_ui guider_ui;


void setup_scr_screen(lv_ui *ui);
LV_IMG_DECLARE(_goodsun_alpha_480x320);
LV_IMG_DECLARE(_chessimg_alpha_48x45);
LV_IMG_DECLARE(_weathimg_alpha_48x45);
LV_IMG_DECLARE(_wave_alpha_48x45);
LV_IMG_DECLARE(_static_alpha_48x45);
LV_IMG_DECLARE(_back_alpha_50x36);
LV_IMG_DECLARE(_aichatimg_alpha_48x45);
LV_IMG_DECLARE(_bspimg_alpha_48x45);
LV_IMG_DECLARE(_temp_alpha_49x46);
LV_IMG_DECLARE(_humidness_alpha_48x45);
LV_IMG_DECLARE(_weathimg_alpha_49x46);

LV_FONT_DECLARE(lv_font_montserratMedium_18)
LV_FONT_DECLARE(lv_font_montserratMedium_18)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_18)
LV_FONT_DECLARE(lv_font_montserratMedium_18)


#ifdef __cplusplus
}
#endif
#endif
