/*
 * Copyright 2024 NXP
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */
/*******************************************************************************
 * Size: 17 px
 * Bpp: 4
 * Opts: --user-data-dir=C:\Users\lenovo\AppData\Roaming\gui-guider --app-path=D:\NXP\GUI-Guider-1.7.2-GA\resources\app.asar --no-sandbox --no-zygote --lang=zh-CN --device-scale-factor=1.5 --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=5 --time-ticks-at-unix-epoch=-1719402779964106 --launch-time-ticks=76075548632 --mojo-platform-channel-handle=3372 --field-trial-handle=2124,i,4100558599758847157,7832436733577728670,131072 --disable-features=SpareRendererForSitePerProcess,WinRetrieveSuggestionsOnlyOnDemand /prefetch:1
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef LV_CUSTOMER_FONT_SOURCEHANSERIFSC_REGULAR_17
#define LV_CUSTOMER_FONT_SOURCEHANSERIFSC_REGULAR_17 1
#endif

#if LV_CUSTOMER_FONT_SOURCEHANSERIFSC_REGULAR_17

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+9634 "阴" */
    0x52, 0x11, 0x91, 0x5, 0x0, 0x0, 0x7, 0xb,
    0x96, 0x8f, 0x70, 0xe8, 0x77, 0x78, 0xf5, 0xb4,
    0x8, 0xa0, 0xe, 0x20, 0x0, 0xf, 0xb, 0x40,
    0xd1, 0x0, 0xe2, 0x0, 0x0, 0xf0, 0xb4, 0x55,
    0x0, 0xe, 0x87, 0x77, 0x8f, 0xb, 0x47, 0x10,
    0x0, 0xe2, 0x0, 0x0, 0xf0, 0xb4, 0xa, 0x10,
    0xe, 0x20, 0x0, 0xf, 0xb, 0x40, 0x5a, 0x0,
    0xf1, 0x0, 0x0, 0xf0, 0xb4, 0x1, 0xf1, 0xf,
    0x87, 0x77, 0x8f, 0xb, 0x40, 0xf, 0x32, 0xd0,
    0x0, 0x0, 0xf0, 0xb6, 0x58, 0xf1, 0x6a, 0x0,
    0x0, 0xf, 0xb, 0x45, 0xf8, 0xb, 0x30, 0x0,
    0x0, 0xf0, 0xb4, 0x0, 0x4, 0xa0, 0x0, 0x0,
    0xf, 0xb, 0x40, 0x2, 0xa0, 0x0, 0x4, 0x45,
    0xf0, 0xb3, 0x1, 0x70, 0x0, 0x0, 0x3, 0xe8,
    0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 272, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 38452, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t lv_customer_font_SourceHanSerifSC_Regular_17 = {
#else
lv_font_t lv_customer_font_SourceHanSerifSC_Regular_17 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font  default: (f.src.ascent - f.src.descent)*/
    .base_line = 2,                          /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if LV_CUSTOMER_FONT_SOURCEHANSERIFSC_REGULAR_17*/

