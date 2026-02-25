/*
* Copyright 2025 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



lv_calendar_date_t screen_calendar_1_today;
lv_calendar_date_t screen_calendar_1_highlihted_days[1];
void setup_scr_screen(lv_ui *ui)
{
	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	lv_obj_set_size(ui->screen, 480, 320);
	lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0x292929), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_background_img
	ui->screen_background_img = lv_img_create(ui->screen);
	lv_obj_add_flag(ui->screen_background_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_background_img, &_goodsun_alpha_480x320);
	lv_img_set_pivot(ui->screen_background_img, 50,50);
	lv_img_set_angle(ui->screen_background_img, 0);
	lv_obj_set_pos(ui->screen_background_img, 0, 0);
	lv_obj_set_size(ui->screen_background_img, 480, 320);

	//Write style for screen_background_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_background_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_cont_1
	ui->screen_cont_1 = lv_obj_create(ui->screen);
	lv_obj_set_pos(ui->screen_cont_1, 0, 0);
	lv_obj_set_size(ui->screen_cont_1, 480, 320);
	lv_obj_set_scrollbar_mode(ui->screen_cont_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_led4_label
	ui->screen_led4_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_led4_label, "LED4");
	lv_label_set_long_mode(ui->screen_led4_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_led4_label, 129, 170);
	lv_obj_set_size(ui->screen_led4_label, 100, 32);
	lv_obj_add_flag(ui->screen_led4_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_led4_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_led4_label, lv_color_hex(0xffb100), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_led4_label, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_led4_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_led4_label, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_led4_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_led4_label, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_led4_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_led3_label
	ui->screen_led3_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_led3_label, "LED3");
	lv_label_set_long_mode(ui->screen_led3_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_led3_label, 128, 105);
	lv_obj_set_size(ui->screen_led3_label, 100, 32);
	lv_obj_add_flag(ui->screen_led3_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_led3_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_led3_label, lv_color_hex(0x00ff2b), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_led3_label, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_led3_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_led3_label, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_led3_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_led3_label, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_led3_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_head_label
	ui->screen_head_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_head_label, "菜单");
	lv_label_set_long_mode(ui->screen_head_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_head_label, 0, -1);
	lv_obj_set_size(ui->screen_head_label, 480, 43);
	lv_obj_add_flag(ui->screen_head_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_head_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_head_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_head_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_head_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_head_label, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_head_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_head_label, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_head_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_head_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_head_label, 122, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_head_label, lv_color_hex(0x83d1ff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_head_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_head_label, 13, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_head_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_head_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_head_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_head_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_user_ta
	ui->screen_user_ta = lv_textarea_create(ui->screen_cont_1);
	lv_textarea_set_text(ui->screen_user_ta, "Who are you");
	lv_textarea_set_placeholder_text(ui->screen_user_ta, "");
	lv_textarea_set_password_bullet(ui->screen_user_ta, "*");
	lv_textarea_set_password_mode(ui->screen_user_ta, false);
	lv_textarea_set_one_line(ui->screen_user_ta, false);
	lv_textarea_set_accepted_chars(ui->screen_user_ta, "");
	lv_textarea_set_max_length(ui->screen_user_ta, 32);
	lv_obj_set_pos(ui->screen_user_ta, 13, 242);
	lv_obj_set_size(ui->screen_user_ta, 406, 63);
	lv_obj_add_flag(ui->screen_user_ta, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_user_ta, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_user_ta, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_user_ta, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_user_ta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_user_ta, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_user_ta, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_user_ta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_user_ta, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_user_ta, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_user_ta, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_user_ta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_user_ta, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_user_ta, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_user_ta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_user_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_user_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_user_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_user_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_user_ta, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_user_ta, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_user_ta, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_user_ta, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_user_ta, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

	//Write codes screen_ai_ta
	ui->screen_ai_ta = lv_textarea_create(ui->screen_cont_1);
	lv_textarea_set_text(ui->screen_ai_ta, "Waiting...");
	lv_textarea_set_placeholder_text(ui->screen_ai_ta, "");
	lv_textarea_set_password_bullet(ui->screen_ai_ta, "*");
	lv_textarea_set_password_mode(ui->screen_ai_ta, false);
	lv_textarea_set_one_line(ui->screen_ai_ta, false);
	lv_textarea_set_accepted_chars(ui->screen_ai_ta, "");
	lv_textarea_set_max_length(ui->screen_ai_ta, 300);
	lv_obj_set_pos(ui->screen_ai_ta, 52, 63);
	lv_obj_set_size(ui->screen_ai_ta, 411, 160);
	lv_obj_add_flag(ui->screen_ai_ta, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_ai_ta, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_ai_ta, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_ai_ta, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_ai_ta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_ai_ta, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_ai_ta, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_ai_ta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_ai_ta, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_ai_ta, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_ai_ta, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_ai_ta, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_ai_ta, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_ai_ta, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_ai_ta, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_ai_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_ai_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_ai_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_ai_ta, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_ai_ta, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_ai_ta, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_ai_ta, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_ai_ta, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_ai_ta, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

	//Write codes screen_ai_label
	ui->screen_ai_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_ai_label, "A");
	lv_label_set_long_mode(ui->screen_ai_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_ai_label, 7, 72);
	lv_obj_set_size(ui->screen_ai_label, 34, 34);
	lv_obj_add_flag(ui->screen_ai_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_ai_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_ai_label, lv_color_hex(0x00eeff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_ai_label, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_ai_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_ai_label, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_ai_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_ai_label, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_ai_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_led4_sw
	ui->screen_led4_sw = lv_switch_create(ui->screen_cont_1);
	lv_obj_set_pos(ui->screen_led4_sw, 241, 175);
	lv_obj_set_size(ui->screen_led4_sw, 40, 20);
	lv_obj_add_flag(ui->screen_led4_sw, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_led4_sw, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_led4_sw, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_led4_sw, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_led4_sw, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_led4_sw, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_led4_sw, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_led4_sw, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_led4_sw, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->screen_led4_sw, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->screen_led4_sw, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->screen_led4_sw, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->screen_led4_sw, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for screen_led4_sw, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_led4_sw, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_led4_sw, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_led4_sw, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_led4_sw, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_led4_sw, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_led3_sw
	ui->screen_led3_sw = lv_switch_create(ui->screen_cont_1);
	lv_obj_set_pos(ui->screen_led3_sw, 241, 110);
	lv_obj_set_size(ui->screen_led3_sw, 40, 20);
	lv_obj_add_flag(ui->screen_led3_sw, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_led3_sw, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_led3_sw, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_led3_sw, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_led3_sw, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_led3_sw, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_led3_sw, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_led3_sw, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_led3_sw, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->screen_led3_sw, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->screen_led3_sw, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->screen_led3_sw, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->screen_led3_sw, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for screen_led3_sw, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_led3_sw, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_led3_sw, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_led3_sw, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_led3_sw, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_led3_sw, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_chess_img
	ui->screen_chess_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_chess_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_chess_img, &_chessimg_alpha_48x45);
	lv_img_set_pivot(ui->screen_chess_img, 50,50);
	lv_img_set_angle(ui->screen_chess_img, 0);
	lv_obj_set_pos(ui->screen_chess_img, 223, 85);
	lv_obj_set_size(ui->screen_chess_img, 48, 45);

	//Write style for screen_chess_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_chess_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_weather_img
	ui->screen_weather_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_weather_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_weather_img, &_weathimg_alpha_48x45);
	lv_img_set_pivot(ui->screen_weather_img, 50,50);
	lv_img_set_angle(ui->screen_weather_img, 0);
	lv_obj_set_pos(ui->screen_weather_img, 354, 86);
	lv_obj_set_size(ui->screen_weather_img, 48, 45);

	//Write style for screen_weather_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_weather_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_wave_img
	ui->screen_wave_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_wave_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_wave_img, &_wave_alpha_48x45);
	lv_img_set_pivot(ui->screen_wave_img, 50,50);
	lv_img_set_angle(ui->screen_wave_img, 0);
	lv_obj_set_pos(ui->screen_wave_img, 76, 175);
	lv_obj_set_size(ui->screen_wave_img, 48, 45);

	//Write style for screen_wave_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_wave_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_sort_img
	ui->screen_sort_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_sort_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_sort_img, &_static_alpha_48x45);
	lv_img_set_pivot(ui->screen_sort_img, 50,50);
	lv_img_set_angle(ui->screen_sort_img, 0);
	lv_obj_set_pos(ui->screen_sort_img, 224, 175);
	lv_obj_set_size(ui->screen_sort_img, 48, 45);

	//Write style for screen_sort_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_sort_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_back_img
	ui->screen_back_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_back_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_back_img, &_back_alpha_50x36);
	lv_img_set_pivot(ui->screen_back_img, 50,50);
	lv_img_set_angle(ui->screen_back_img, 0);
	lv_obj_set_pos(ui->screen_back_img, 23, 3);
	lv_obj_set_size(ui->screen_back_img, 50, 36);
	lv_obj_add_flag(ui->screen_back_img, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_back_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_back_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_chat_img
	ui->screen_chat_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_chat_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_chat_img, &_aichatimg_alpha_48x45);
	lv_img_set_pivot(ui->screen_chat_img, 50,50);
	lv_img_set_angle(ui->screen_chat_img, 0);
	lv_obj_set_pos(ui->screen_chat_img, 74, 85);
	lv_obj_set_size(ui->screen_chat_img, 48, 45);

	//Write style for screen_chat_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_chat_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_bsp_img
	ui->screen_bsp_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_bsp_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_bsp_img, &_bspimg_alpha_48x45);
	lv_img_set_pivot(ui->screen_bsp_img, 50,50);
	lv_img_set_angle(ui->screen_bsp_img, 0);
	lv_obj_set_pos(ui->screen_bsp_img, 354, 175);
	lv_obj_set_size(ui->screen_bsp_img, 48, 45);

	//Write style for screen_bsp_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_bsp_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_adc_label
	ui->screen_adc_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_adc_label, "0000");
	lv_label_set_long_mode(ui->screen_adc_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_adc_label, 190, 210);
	lv_obj_set_size(ui->screen_adc_label, 100, 32);
	lv_obj_add_flag(ui->screen_adc_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_adc_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_adc_label, lv_color_hex(0x00eeff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_adc_label, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_adc_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_adc_label, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_adc_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_adc_label, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_adc_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_adc_slider
	ui->screen_adc_slider = lv_slider_create(ui->screen_cont_1);
	lv_slider_set_range(ui->screen_adc_slider, 0, 4096);
	lv_slider_set_mode(ui->screen_adc_slider, LV_SLIDER_MODE_NORMAL);
	lv_slider_set_value(ui->screen_adc_slider, 0, LV_ANIM_OFF);
	lv_obj_set_pos(ui->screen_adc_slider, 79, 166);
	lv_obj_set_size(ui->screen_adc_slider, 332, 10);
	lv_obj_add_flag(ui->screen_adc_slider, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_adc_slider, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_adc_slider, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_adc_slider, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_adc_slider, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_adc_slider, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_width(ui->screen_adc_slider, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_adc_slider, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_adc_slider, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_adc_slider, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_adc_slider, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_adc_slider, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_adc_slider, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for screen_adc_slider, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_adc_slider, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_adc_slider, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_adc_slider, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_adc_slider, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_user_btn
	ui->screen_user_btn = lv_btn_create(ui->screen_cont_1);
	ui->screen_user_btn_label = lv_label_create(ui->screen_user_btn);
	lv_label_set_text(ui->screen_user_btn_label, "Q");
	lv_label_set_long_mode(ui->screen_user_btn_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_user_btn_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_user_btn, 0, LV_STATE_DEFAULT);
	lv_obj_set_width(ui->screen_user_btn_label, LV_PCT(100));
	lv_obj_set_pos(ui->screen_user_btn, 424, 244);
	lv_obj_set_size(ui->screen_user_btn, 41, 58);
	lv_obj_add_flag(ui->screen_user_btn, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_user_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_user_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_user_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_user_btn, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_user_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_user_btn, lv_color_hex(0x00ffee), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_user_btn, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_user_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_user_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_temper_img
	ui->screen_temper_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_temper_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_temper_img, &_temp_alpha_49x46);
	lv_img_set_pivot(ui->screen_temper_img, 50,50);
	lv_img_set_angle(ui->screen_temper_img, 0);
	lv_obj_set_pos(ui->screen_temper_img, 55, 79);
	lv_obj_set_size(ui->screen_temper_img, 49, 46);
	lv_obj_add_flag(ui->screen_temper_img, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_temper_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_temper_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_humid_img
	ui->screen_humid_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_humid_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_humid_img, &_humidness_alpha_48x45);
	lv_img_set_pivot(ui->screen_humid_img, 50,50);
	lv_img_set_angle(ui->screen_humid_img, 0);
	lv_obj_set_pos(ui->screen_humid_img, 58, 161);
	lv_obj_set_size(ui->screen_humid_img, 48, 45);
	lv_obj_add_flag(ui->screen_humid_img, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_humid_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_humid_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_weath_img
	ui->screen_weath_img = lv_img_create(ui->screen_cont_1);
	lv_obj_add_flag(ui->screen_weath_img, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_weath_img, &_weathimg_alpha_49x46);
	lv_img_set_pivot(ui->screen_weath_img, 50,50);
	lv_img_set_angle(ui->screen_weath_img, 0);
	lv_obj_set_pos(ui->screen_weath_img, 57, 236);
	lv_obj_set_size(ui->screen_weath_img, 49, 46);
	lv_obj_add_flag(ui->screen_weath_img, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_weath_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->screen_weath_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_temperdat_label
	ui->screen_temperdat_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_temperdat_label, "28");
	lv_label_set_long_mode(ui->screen_temperdat_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_temperdat_label, 144, 84);
	lv_obj_set_size(ui->screen_temperdat_label, 200, 41);
	lv_obj_add_flag(ui->screen_temperdat_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_temperdat_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_temperdat_label, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_temperdat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_temperdat_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_temperdat_label, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_temperdat_label, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_temperdat_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_temperdat_label, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_temperdat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_temperdat_label, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_temperdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_temperdat_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_temperdat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_temperdat_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_temperdat_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_temperdat_label, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_temperdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_temperdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_temperdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_temperdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_humiddat_label
	ui->screen_humiddat_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_humiddat_label, "85");
	lv_label_set_long_mode(ui->screen_humiddat_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_humiddat_label, 147, 161);
	lv_obj_set_size(ui->screen_humiddat_label, 200, 41);
	lv_obj_add_flag(ui->screen_humiddat_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_humiddat_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_humiddat_label, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_humiddat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_humiddat_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_humiddat_label, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_humiddat_label, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_humiddat_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_humiddat_label, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_humiddat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_humiddat_label, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_humiddat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_humiddat_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_humiddat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_humiddat_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_humiddat_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_humiddat_label, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_humiddat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_humiddat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_humiddat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_humiddat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_weathdat_label
	ui->screen_weathdat_label = lv_label_create(ui->screen_cont_1);
	lv_label_set_text(ui->screen_weathdat_label, "阴");
	lv_label_set_long_mode(ui->screen_weathdat_label, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_weathdat_label, 146, 240);
	lv_obj_set_size(ui->screen_weathdat_label, 200, 41);
	lv_obj_add_flag(ui->screen_weathdat_label, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_weathdat_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_weathdat_label, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_weathdat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_weathdat_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_weathdat_label, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_weathdat_label, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_weathdat_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_weathdat_label, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_weathdat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_weathdat_label, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_weathdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_weathdat_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_weathdat_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_weathdat_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_weathdat_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_weathdat_label, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_weathdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_weathdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_weathdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_weathdat_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_cont_2
	ui->screen_cont_2 = lv_obj_create(ui->screen);
	lv_obj_set_pos(ui->screen_cont_2, 30, 35);
	lv_obj_set_size(ui->screen_cont_2, 414, 258);
	lv_obj_set_scrollbar_mode(ui->screen_cont_2, LV_SCROLLBAR_MODE_OFF);
	lv_obj_add_flag(ui->screen_cont_2, LV_OBJ_FLAG_HIDDEN);

	//Write style for screen_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_cont_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_cont_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_btnm_1
	ui->screen_btnm_1 = lv_btnmatrix_create(ui->screen_cont_2);
	static const char *screen_btnm_1_text_map[] = {"1", "2", "3", "+", "\n", "4", "5", "6", "-", "\n", "7", "8", "9", "*", "\n", "C", "0", "=", "/", "",};
	lv_btnmatrix_set_map(ui->screen_btnm_1, screen_btnm_1_text_map);
	lv_obj_set_pos(ui->screen_btnm_1, -6, 109);
	lv_obj_set_size(ui->screen_btnm_1, 339, 150);

	//Write style for screen_btnm_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_btnm_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_btnm_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_btnm_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_btnm_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_btnm_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_btnm_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_row(ui->screen_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_column(ui->screen_btnm_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_btnm_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_btnm_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_btnm_1, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_btnm_1, 1, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_btnm_1, lv_color_hex(0xc9c9c9), LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_btnm_1, LV_BORDER_SIDE_FULL, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_btnm_1, lv_color_hex(0xffffff), LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_btnm_1, &lv_font_montserratMedium_18, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_btnm_1, 4, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_btnm_1, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_btnm_1, lv_color_hex(0xff6500), LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_btnm_1, LV_GRAD_DIR_NONE, LV_PART_ITEMS|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_btnm_1, 0, LV_PART_ITEMS|LV_STATE_DEFAULT);

	//Write codes screen_ta_1
	ui->screen_ta_1 = lv_textarea_create(ui->screen_cont_2);
	lv_textarea_set_text(ui->screen_ta_1, "");
	lv_textarea_set_placeholder_text(ui->screen_ta_1, "");
	lv_textarea_set_password_bullet(ui->screen_ta_1, "*");
	lv_textarea_set_password_mode(ui->screen_ta_1, false);
	lv_textarea_set_one_line(ui->screen_ta_1, false);
	lv_textarea_set_accepted_chars(ui->screen_ta_1, "");
	lv_textarea_set_max_length(ui->screen_ta_1, 32);
	lv_obj_set_pos(ui->screen_ta_1, 2, 1);
	lv_obj_set_size(ui->screen_ta_1, 408, 101);

	//Write style for screen_ta_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_ta_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_ta_1, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_ta_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_ta_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_ta_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_ta_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_ta_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_ta_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_ta_1, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_ta_1, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_ta_1, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

	//Write codes screen_btn_1
	ui->screen_btn_1 = lv_btn_create(ui->screen_cont_2);
	ui->screen_btn_1_label = lv_label_create(ui->screen_btn_1);
	lv_label_set_text(ui->screen_btn_1_label, "Back");
	lv_label_set_long_mode(ui->screen_btn_1_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_btn_1_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->screen_btn_1, 0, LV_STATE_DEFAULT);
	lv_obj_set_width(ui->screen_btn_1_label, LV_PCT(100));
	lv_obj_set_pos(ui->screen_btn_1, 338, 108);
	lv_obj_set_size(ui->screen_btn_1, 67, 145);

	//Write style for screen_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_btn_1, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_btn_1, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_calendar_1
	ui->screen_calendar_1 = lv_calendar_create(ui->screen);
	screen_calendar_1_today.year = 2025;
	screen_calendar_1_today.month = 5;
	screen_calendar_1_today.day = 11;
	lv_calendar_set_today_date(ui->screen_calendar_1, screen_calendar_1_today.year, screen_calendar_1_today.month, screen_calendar_1_today.day);
	lv_calendar_set_showed_date(ui->screen_calendar_1, screen_calendar_1_today.year, screen_calendar_1_today.month);
	screen_calendar_1_highlihted_days[0].year = 2025;
	screen_calendar_1_highlihted_days[0].month = 5;
	screen_calendar_1_highlihted_days[0].day = 12;
	lv_calendar_set_highlighted_dates(ui->screen_calendar_1, screen_calendar_1_highlihted_days, 1);
	lv_obj_t *screen_calendar_1_header = lv_calendar_header_arrow_create(ui->screen_calendar_1);
	lv_calendar_t *screen_calendar_1 = (lv_calendar_t *)ui->screen_calendar_1;
	lv_obj_add_event_cb(screen_calendar_1->btnm, screen_calendar_1_draw_part_begin_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
	lv_obj_add_event_cb(ui->screen_calendar_1, screen_calendar_1_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_pos(ui->screen_calendar_1, 43, 8);
	lv_obj_set_size(ui->screen_calendar_1, 400, 300);
	lv_obj_add_flag(ui->screen_calendar_1, LV_OBJ_FLAG_HIDDEN);

	//Write style state: LV_STATE_DEFAULT for &style_screen_calendar_1_main_main_default
	static lv_style_t style_screen_calendar_1_main_main_default;
	ui_init_style(&style_screen_calendar_1_main_main_default);
	
	lv_style_set_border_width(&style_screen_calendar_1_main_main_default, 1);
	lv_style_set_border_opa(&style_screen_calendar_1_main_main_default, 255);
	lv_style_set_border_color(&style_screen_calendar_1_main_main_default, lv_color_hex(0xc0c0c0));
	lv_style_set_border_side(&style_screen_calendar_1_main_main_default, LV_BORDER_SIDE_FULL);
	lv_style_set_bg_opa(&style_screen_calendar_1_main_main_default, 255);
	lv_style_set_bg_color(&style_screen_calendar_1_main_main_default, lv_color_hex(0xffffff));
	lv_style_set_bg_grad_dir(&style_screen_calendar_1_main_main_default, LV_GRAD_DIR_NONE);
	lv_style_set_shadow_width(&style_screen_calendar_1_main_main_default, 0);
	lv_style_set_radius(&style_screen_calendar_1_main_main_default, 0);
	lv_obj_add_style(ui->screen_calendar_1, &style_screen_calendar_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for &style_screen_calendar_1_extra_header_main_default
	static lv_style_t style_screen_calendar_1_extra_header_main_default;
	ui_init_style(&style_screen_calendar_1_extra_header_main_default);
	
	lv_style_set_text_color(&style_screen_calendar_1_extra_header_main_default, lv_color_hex(0xffffff));
	lv_style_set_text_font(&style_screen_calendar_1_extra_header_main_default, &lv_font_montserratMedium_18);
	lv_style_set_text_opa(&style_screen_calendar_1_extra_header_main_default, 255);
	lv_style_set_bg_opa(&style_screen_calendar_1_extra_header_main_default, 255);
	lv_style_set_bg_color(&style_screen_calendar_1_extra_header_main_default, lv_color_hex(0x2195f6));
	lv_style_set_bg_grad_dir(&style_screen_calendar_1_extra_header_main_default, LV_GRAD_DIR_NONE);
	lv_obj_add_style(screen_calendar_1_header, &style_screen_calendar_1_extra_header_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for &style_screen_calendar_1_main_items_default
	static lv_style_t style_screen_calendar_1_main_items_default;
	ui_init_style(&style_screen_calendar_1_main_items_default);
	
	lv_style_set_bg_opa(&style_screen_calendar_1_main_items_default, 0);
	lv_style_set_border_width(&style_screen_calendar_1_main_items_default, 1);
	lv_style_set_border_opa(&style_screen_calendar_1_main_items_default, 255);
	lv_style_set_border_color(&style_screen_calendar_1_main_items_default, lv_color_hex(0xc0c0c0));
	lv_style_set_border_side(&style_screen_calendar_1_main_items_default, LV_BORDER_SIDE_FULL);
	lv_style_set_text_color(&style_screen_calendar_1_main_items_default, lv_color_hex(0x0D3055));
	lv_style_set_text_font(&style_screen_calendar_1_main_items_default, &lv_font_montserratMedium_18);
	lv_style_set_text_opa(&style_screen_calendar_1_main_items_default, 255);
	lv_obj_add_style(lv_calendar_get_btnmatrix(ui->screen_calendar_1), &style_screen_calendar_1_main_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

	//The custom code of screen.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->screen);

	//Init events for screen.
	events_init_screen(ui);
}
