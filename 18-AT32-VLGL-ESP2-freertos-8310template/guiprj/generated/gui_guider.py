# Copyright 2025 NXP
# NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct
import fs_driver

lv.init()
SDL.init(w=480,h=320)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(480*10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 480
disp_drv.ver_res = 320
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init()
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

# Below: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV

    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = ustruct.unpack(">LL", png_header[start:start+8])
        except ustruct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

def convert_rgba8888_to_bgra8888(img_view):
    for i in range(0, len(img_view), lv.color_t.__SIZE__):
        ch = lv.color_t.__cast__(img_view[i:i]).ch
        ch.red, ch.blue = ch.blue, ch.red

# Read and parse PNG file

def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size)
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

# Above: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_zoom(v)

def anim_img_rotate_cb(obj, v):
    obj.set_angle(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.font_load(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.img_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = e.get_current_target()
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RES.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    target = e.get_target()
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = target.get_screen()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = target.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, target), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    target = e.get_current_target()
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if target.get_pressed_date(date) == lv.RES.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.clear_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            target.delete()

# Create screen
screen = lv.obj()
screen.set_size(480, 320)
screen.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen.set_style_bg_color(lv.color_hex(0x292929), lv.PART.MAIN|lv.STATE.DEFAULT)
screen.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_background_img
screen_background_img = lv.img(screen)
screen_background_img.set_src("B:MicroPython/_goodsun_alpha_480x320.bin")
screen_background_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_background_img.set_pivot(50,50)
screen_background_img.set_angle(0)
screen_background_img.set_pos(0, 0)
screen_background_img.set_size(480, 320)
# Set style for screen_background_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_background_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_cont_1
screen_cont_1 = lv.obj(screen)
screen_cont_1.set_pos(0, 0)
screen_cont_1.set_size(480, 320)
screen_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_led4_label
screen_led4_label = lv.label(screen_cont_1)
screen_led4_label.set_text("LED4")
screen_led4_label.set_long_mode(lv.label.LONG.WRAP)
screen_led4_label.set_width(lv.pct(100))
screen_led4_label.set_pos(129, 170)
screen_led4_label.set_size(100, 32)
screen_led4_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_led4_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_led4_label.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_text_color(lv.color_hex(0xffb100), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_text_letter_space(3, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_pad_top(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_led3_label
screen_led3_label = lv.label(screen_cont_1)
screen_led3_label.set_text("LED3")
screen_led3_label.set_long_mode(lv.label.LONG.WRAP)
screen_led3_label.set_width(lv.pct(100))
screen_led3_label.set_pos(128, 105)
screen_led3_label.set_size(100, 32)
screen_led3_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_led3_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_led3_label.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_text_color(lv.color_hex(0x00ff2b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_text_letter_space(3, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_pad_top(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_head_label
screen_head_label = lv.label(screen_cont_1)
screen_head_label.set_text("菜单")
screen_head_label.set_long_mode(lv.label.LONG.WRAP)
screen_head_label.set_width(lv.pct(100))
screen_head_label.set_pos(0, -1)
screen_head_label.set_size(480, 43)
screen_head_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_head_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_head_label.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_bg_opa(122, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_bg_color(lv.color_hex(0x83d1ff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_pad_top(13, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_head_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_user_ta
screen_user_ta = lv.textarea(screen_cont_1)
screen_user_ta.set_text("Who are you")
screen_user_ta.set_placeholder_text("")
screen_user_ta.set_password_bullet("*")
screen_user_ta.set_password_mode(False)
screen_user_ta.set_one_line(False)
screen_user_ta.set_accepted_chars("")
screen_user_ta.set_max_length(32)
screen_user_ta.set_pos(13, 242)
screen_user_ta.set_size(406, 63)
screen_user_ta.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_user_ta, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_user_ta.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_pad_top(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_pad_left(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_ta.set_style_radius(4, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_user_ta, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_user_ta.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_user_ta.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_user_ta.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_user_ta.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_ai_ta
screen_ai_ta = lv.textarea(screen_cont_1)
screen_ai_ta.set_text("Waiting...")
screen_ai_ta.set_placeholder_text("")
screen_ai_ta.set_password_bullet("*")
screen_ai_ta.set_password_mode(False)
screen_ai_ta.set_one_line(False)
screen_ai_ta.set_accepted_chars("")
screen_ai_ta.set_max_length(300)
screen_ai_ta.set_pos(52, 63)
screen_ai_ta.set_size(411, 160)
screen_ai_ta.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_ai_ta, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_ai_ta.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_pad_top(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_pad_left(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_ta.set_style_radius(4, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_ai_ta, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_ai_ta.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ai_ta.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ai_ta.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ai_ta.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_ai_label
screen_ai_label = lv.label(screen_cont_1)
screen_ai_label.set_text("A")
screen_ai_label.set_long_mode(lv.label.LONG.WRAP)
screen_ai_label.set_width(lv.pct(100))
screen_ai_label.set_pos(7, 72)
screen_ai_label.set_size(34, 34)
screen_ai_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_ai_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_ai_label.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_text_color(lv.color_hex(0x00eeff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_text_letter_space(3, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_pad_top(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ai_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_led4_sw
screen_led4_sw = lv.switch(screen_cont_1)
screen_led4_sw.set_pos(241, 175)
screen_led4_sw.set_size(40, 20)
screen_led4_sw.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_led4_sw, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_led4_sw.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_sw.set_style_bg_color(lv.color_hex(0xe6e2e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_sw.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_sw.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_sw.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led4_sw.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_led4_sw, Part: lv.PART.INDICATOR, State: lv.STATE.CHECKED.
screen_led4_sw.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_led4_sw.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_led4_sw.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_led4_sw.set_style_border_width(0, lv.PART.INDICATOR|lv.STATE.CHECKED)

# Set style for screen_led4_sw, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_led4_sw.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led4_sw.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led4_sw.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led4_sw.set_style_border_width(0, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led4_sw.set_style_radius(10, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_led3_sw
screen_led3_sw = lv.switch(screen_cont_1)
screen_led3_sw.set_pos(241, 110)
screen_led3_sw.set_size(40, 20)
screen_led3_sw.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_led3_sw, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_led3_sw.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_sw.set_style_bg_color(lv.color_hex(0xe6e2e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_sw.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_sw.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_sw.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_led3_sw.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_led3_sw, Part: lv.PART.INDICATOR, State: lv.STATE.CHECKED.
screen_led3_sw.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_led3_sw.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_led3_sw.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_led3_sw.set_style_border_width(0, lv.PART.INDICATOR|lv.STATE.CHECKED)

# Set style for screen_led3_sw, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_led3_sw.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led3_sw.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led3_sw.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led3_sw.set_style_border_width(0, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_led3_sw.set_style_radius(10, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_chess_img
screen_chess_img = lv.img(screen_cont_1)
screen_chess_img.set_src("B:MicroPython/_chessimg_alpha_48x45.bin")
screen_chess_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_chess_img.set_pivot(50,50)
screen_chess_img.set_angle(0)
screen_chess_img.set_pos(223, 85)
screen_chess_img.set_size(48, 45)
# Set style for screen_chess_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_chess_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_weather_img
screen_weather_img = lv.img(screen_cont_1)
screen_weather_img.set_src("B:MicroPython/_weathimg_alpha_48x45.bin")
screen_weather_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_weather_img.set_pivot(50,50)
screen_weather_img.set_angle(0)
screen_weather_img.set_pos(354, 86)
screen_weather_img.set_size(48, 45)
# Set style for screen_weather_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_weather_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wave_img
screen_wave_img = lv.img(screen_cont_1)
screen_wave_img.set_src("B:MicroPython/_wave_alpha_48x45.bin")
screen_wave_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wave_img.set_pivot(50,50)
screen_wave_img.set_angle(0)
screen_wave_img.set_pos(76, 175)
screen_wave_img.set_size(48, 45)
# Set style for screen_wave_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wave_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_sort_img
screen_sort_img = lv.img(screen_cont_1)
screen_sort_img.set_src("B:MicroPython/_static_alpha_48x45.bin")
screen_sort_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_sort_img.set_pivot(50,50)
screen_sort_img.set_angle(0)
screen_sort_img.set_pos(224, 175)
screen_sort_img.set_size(48, 45)
# Set style for screen_sort_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_sort_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_back_img
screen_back_img = lv.img(screen_cont_1)
screen_back_img.set_src("B:MicroPython/_back_alpha_50x36.bin")
screen_back_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_back_img.set_pivot(50,50)
screen_back_img.set_angle(0)
screen_back_img.set_pos(23, 3)
screen_back_img.set_size(50, 36)
screen_back_img.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_back_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_back_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_chat_img
screen_chat_img = lv.img(screen_cont_1)
screen_chat_img.set_src("B:MicroPython/_aichatimg_alpha_48x45.bin")
screen_chat_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_chat_img.set_pivot(50,50)
screen_chat_img.set_angle(0)
screen_chat_img.set_pos(74, 85)
screen_chat_img.set_size(48, 45)
# Set style for screen_chat_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_chat_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_bsp_img
screen_bsp_img = lv.img(screen_cont_1)
screen_bsp_img.set_src("B:MicroPython/_bspimg_alpha_48x45.bin")
screen_bsp_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_bsp_img.set_pivot(50,50)
screen_bsp_img.set_angle(0)
screen_bsp_img.set_pos(354, 175)
screen_bsp_img.set_size(48, 45)
# Set style for screen_bsp_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_bsp_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_adc_label
screen_adc_label = lv.label(screen_cont_1)
screen_adc_label.set_text("0000")
screen_adc_label.set_long_mode(lv.label.LONG.WRAP)
screen_adc_label.set_width(lv.pct(100))
screen_adc_label.set_pos(190, 210)
screen_adc_label.set_size(100, 32)
screen_adc_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_adc_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_adc_label.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_text_color(lv.color_hex(0x00eeff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_text_letter_space(3, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_pad_top(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_adc_slider
screen_adc_slider = lv.slider(screen_cont_1)
screen_adc_slider.set_range(0, 4096)
screen_adc_slider.set_mode(lv.slider.MODE.NORMAL)
screen_adc_slider.set_value(0, lv.ANIM.OFF)
screen_adc_slider.set_pos(79, 166)
screen_adc_slider.set_size(332, 10)
screen_adc_slider.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_adc_slider, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_adc_slider.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_slider.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_slider.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_slider.set_style_radius(50, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_slider.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_adc_slider.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_adc_slider, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_adc_slider.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_adc_slider.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_adc_slider.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_adc_slider.set_style_radius(50, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_adc_slider, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_adc_slider.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_adc_slider.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_adc_slider.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_adc_slider.set_style_radius(50, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_user_btn
screen_user_btn = lv.btn(screen_cont_1)
screen_user_btn_label = lv.label(screen_user_btn)
screen_user_btn_label.set_text("Q")
screen_user_btn_label.set_long_mode(lv.label.LONG.WRAP)
screen_user_btn_label.set_width(lv.pct(100))
screen_user_btn_label.align(lv.ALIGN.CENTER, 0, 0)
screen_user_btn.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_user_btn.set_pos(424, 244)
screen_user_btn.set_size(41, 58)
screen_user_btn.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_user_btn, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_user_btn.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_btn.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_btn.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_btn.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_btn.set_style_text_color(lv.color_hex(0x00ffee), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_btn.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_btn.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_user_btn.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_temper_img
screen_temper_img = lv.img(screen_cont_1)
screen_temper_img.set_src("B:MicroPython/_temp_alpha_49x46.bin")
screen_temper_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_temper_img.set_pivot(50,50)
screen_temper_img.set_angle(0)
screen_temper_img.set_pos(55, 79)
screen_temper_img.set_size(49, 46)
screen_temper_img.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_temper_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_temper_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_humid_img
screen_humid_img = lv.img(screen_cont_1)
screen_humid_img.set_src("B:MicroPython/_humidness_alpha_48x45.bin")
screen_humid_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_humid_img.set_pivot(50,50)
screen_humid_img.set_angle(0)
screen_humid_img.set_pos(58, 161)
screen_humid_img.set_size(48, 45)
screen_humid_img.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_humid_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_humid_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_weath_img
screen_weath_img = lv.img(screen_cont_1)
screen_weath_img.set_src("B:MicroPython/_weathimg_alpha_49x46.bin")
screen_weath_img.add_flag(lv.obj.FLAG.CLICKABLE)
screen_weath_img.set_pivot(50,50)
screen_weath_img.set_angle(0)
screen_weath_img.set_pos(57, 236)
screen_weath_img.set_size(49, 46)
screen_weath_img.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_weath_img, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_weath_img.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_temperdat_label
screen_temperdat_label = lv.label(screen_cont_1)
screen_temperdat_label.set_text("28")
screen_temperdat_label.set_long_mode(lv.label.LONG.WRAP)
screen_temperdat_label.set_width(lv.pct(100))
screen_temperdat_label.set_pos(144, 84)
screen_temperdat_label.set_size(200, 41)
screen_temperdat_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_temperdat_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_temperdat_label.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_border_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_text_letter_space(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_temperdat_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_humiddat_label
screen_humiddat_label = lv.label(screen_cont_1)
screen_humiddat_label.set_text("85")
screen_humiddat_label.set_long_mode(lv.label.LONG.WRAP)
screen_humiddat_label.set_width(lv.pct(100))
screen_humiddat_label.set_pos(147, 161)
screen_humiddat_label.set_size(200, 41)
screen_humiddat_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_humiddat_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_humiddat_label.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_border_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_text_letter_space(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_humiddat_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_weathdat_label
screen_weathdat_label = lv.label(screen_cont_1)
screen_weathdat_label.set_text("阴")
screen_weathdat_label.set_long_mode(lv.label.LONG.WRAP)
screen_weathdat_label.set_width(lv.pct(100))
screen_weathdat_label.set_pos(146, 240)
screen_weathdat_label.set_size(200, 41)
screen_weathdat_label.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_weathdat_label, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_weathdat_label.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_border_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_text_letter_space(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_weathdat_label.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_cont_2
screen_cont_2 = lv.obj(screen)
screen_cont_2.set_pos(30, 35)
screen_cont_2.set_size(414, 258)
screen_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
screen_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_cont_2.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_border_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_btnm_1
screen_btnm_1 = lv.btnmatrix(screen_cont_2)
screen_btnm_1_text_map = ["1", "2", "3", "+", "\n", "4", "5", "6", "-", "\n", "7", "8", "9", "*", "\n", "C", "0", "=", "/", "",]
screen_btnm_1.set_map(screen_btnm_1_text_map)
screen_btnm_1.set_pos(-6, 109)
screen_btnm_1.set_size(339, 150)
# Set style for screen_btnm_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btnm_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_border_color(lv.color_hex(0xc9c9c9), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_pad_row(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_pad_column(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_radius(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btnm_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_btnm_1, Part: lv.PART.ITEMS, State: lv.STATE.DEFAULT.
screen_btnm_1.set_style_border_width(1, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_border_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_border_color(lv.color_hex(0xc9c9c9), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_text_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_radius(4, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_bg_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_bg_color(lv.color_hex(0xff6500), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_btnm_1.set_style_shadow_width(0, lv.PART.ITEMS|lv.STATE.DEFAULT)

# Create screen_ta_1
screen_ta_1 = lv.textarea(screen_cont_2)
screen_ta_1.set_text("")
screen_ta_1.set_placeholder_text("")
screen_ta_1.set_password_bullet("*")
screen_ta_1.set_password_mode(False)
screen_ta_1.set_one_line(False)
screen_ta_1.set_accepted_chars("")
screen_ta_1.set_max_length(32)
screen_ta_1.set_pos(2, 1)
screen_ta_1.set_size(408, 101)
# Set style for screen_ta_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_ta_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_pad_top(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_pad_left(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_radius(4, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_ta_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_ta_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ta_1.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_btn_1
screen_btn_1 = lv.btn(screen_cont_2)
screen_btn_1_label = lv.label(screen_btn_1)
screen_btn_1_label.set_text("Back")
screen_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_btn_1_label.set_width(lv.pct(100))
screen_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_1.set_pos(338, 108)
screen_btn_1.set_size(67, 145)
# Set style for screen_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_bg_color(lv.color_hex(0xf00000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_calendar_1
screen_calendar_1 = lv.calendar(screen)
screen_calendar_1.set_today_date(time.localtime()[0], time.localtime()[1], time.localtime()[2])
screen_calendar_1.set_showed_date(time.localtime()[0], time.localtime()[1])
screen_calendar_1_highlighted_days=[
lv.calendar_date_t({'year':2025, 'month':5, 'day':12})
]
screen_calendar_1.set_highlighted_dates(screen_calendar_1_highlighted_days, len(screen_calendar_1_highlighted_days))
screen_calendar_1_header = lv.calendar_header_arrow(screen_calendar_1)
screen_calendar_1.add_event_cb(lambda e: calendar_event_handler(e,screen_calendar_1), lv.EVENT.ALL, None)
screen_calendar_1.set_pos(43, 8)
screen_calendar_1.set_size(400, 300)
screen_calendar_1.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_calendar_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_calendar_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_border_color(lv.color_hex(0xc0c0c0), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_calendar_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_calendar_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_calendar_1_extra_header_main_default = lv.style_t()
style_screen_calendar_1_extra_header_main_default.init()
style_screen_calendar_1_extra_header_main_default.set_text_color(lv.color_hex(0xffffff))
style_screen_calendar_1_extra_header_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_calendar_1_extra_header_main_default.set_text_opa(255)
style_screen_calendar_1_extra_header_main_default.set_bg_opa(255)
style_screen_calendar_1_extra_header_main_default.set_bg_color(lv.color_hex(0x2195f6))
style_screen_calendar_1_extra_header_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_calendar_1_header.add_style(style_screen_calendar_1_extra_header_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_calendar_1, Part: lv.PART.ITEMS, State: lv.STATE.DEFAULT.
screen_calendar_1.get_btnmatrix().set_style_bg_opa(0, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_calendar_1.get_btnmatrix().set_style_border_width(1, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_calendar_1.get_btnmatrix().set_style_border_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_calendar_1.get_btnmatrix().set_style_border_color(lv.color_hex(0xc0c0c0), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_calendar_1.get_btnmatrix().set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_calendar_1.get_btnmatrix().set_style_text_color(lv.color_hex(0x0D3055), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_calendar_1.get_btnmatrix().set_style_text_font(test_font("montserratMedium", 12), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_calendar_1.get_btnmatrix().set_style_text_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)

def screen_calendar_1_extra_ctrl_day_names_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id < 7:
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0x0D3055)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)

screen_calendar_1.get_btnmatrix().add_event_cb(screen_calendar_1_extra_ctrl_day_names_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

def screen_calendar_1_extra_ctrl_highlight_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id >= 7 and obj.has_btn_ctrl(dsc.id, lv.btnmatrix.CTRL.CUSTOM_2):
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0x0D3055)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)
        dsc.rect_dsc.bg_opa = 255
        dsc.rect_dsc.bg_color = lv.color_hex(0x2195f6)

screen_calendar_1.get_btnmatrix().add_event_cb(screen_calendar_1_extra_ctrl_highlight_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

def screen_calendar_1_extra_ctrl_today_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id >= 7 and obj.has_btn_ctrl(dsc.id, lv.btnmatrix.CTRL.CUSTOM_1):
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0x0D3055)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)
        dsc.rect_dsc.bg_opa = 255
        dsc.rect_dsc.bg_color = lv.color_hex(0x01a2b1)
        dsc.rect_dsc.border_width = 1
        dsc.rect_dsc.border_color = lv.color_hex(0xc0c0c0)
        dsc.rect_dsc.border_opa = 255

screen_calendar_1.get_btnmatrix().add_event_cb(screen_calendar_1_extra_ctrl_today_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

def screen_calendar_1_extra_ctrl_other_month_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id >= 7 and obj.has_btn_ctrl(dsc.id, lv.btnmatrix.CTRL.DISABLED):
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0xA9A2A2)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)
        dsc.rect_dsc.bg_opa = 0

screen_calendar_1.get_btnmatrix().add_event_cb(screen_calendar_1_extra_ctrl_other_month_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

screen.update_layout()

def screen_cont_1_event_handler(e):
    code = e.get_code()

screen_cont_1.add_event_cb(lambda e: screen_cont_1_event_handler(e), lv.EVENT.ALL, None)

def screen_head_label_event_handler(e):
    code = e.get_code()

screen_head_label.add_event_cb(lambda e: screen_head_label_event_handler(e), lv.EVENT.ALL, None)

def screen_user_ta_event_handler(e):
    code = e.get_code()

screen_user_ta.add_event_cb(lambda e: screen_user_ta_event_handler(e), lv.EVENT.ALL, None)

def screen_led4_sw_event_handler(e):
    code = e.get_code()

screen_led4_sw.add_event_cb(lambda e: screen_led4_sw_event_handler(e), lv.EVENT.ALL, None)

def screen_led3_sw_event_handler(e):
    code = e.get_code()

screen_led3_sw.add_event_cb(lambda e: screen_led3_sw_event_handler(e), lv.EVENT.ALL, None)

def screen_chess_img_event_handler(e):
    code = e.get_code()

screen_chess_img.add_event_cb(lambda e: screen_chess_img_event_handler(e), lv.EVENT.ALL, None)

def screen_weather_img_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        screen_calendar_1.clear_flag(lv.obj.FLAG.HIDDEN);screen_cont_2.add_flag(lv.obj.FLAG.HIDDEN)

screen_weather_img.add_event_cb(lambda e: screen_weather_img_event_handler(e), lv.EVENT.ALL, None)

def screen_wave_img_event_handler(e):
    code = e.get_code()

screen_wave_img.add_event_cb(lambda e: screen_wave_img_event_handler(e), lv.EVENT.ALL, None)

def screen_sort_img_event_handler(e):
    code = e.get_code()

screen_sort_img.add_event_cb(lambda e: screen_sort_img_event_handler(e), lv.EVENT.ALL, None)

def screen_back_img_event_handler(e):
    code = e.get_code()

screen_back_img.add_event_cb(lambda e: screen_back_img_event_handler(e), lv.EVENT.ALL, None)

def screen_chat_img_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        screen_cont_2.clear_flag(lv.obj.FLAG.HIDDEN);screen_cont_1.add_flag(lv.obj.FLAG.HIDDEN)

screen_chat_img.add_event_cb(lambda e: screen_chat_img_event_handler(e), lv.EVENT.ALL, None)

def screen_bsp_img_event_handler(e):
    code = e.get_code()

screen_bsp_img.add_event_cb(lambda e: screen_bsp_img_event_handler(e), lv.EVENT.ALL, None)

def screen_user_btn_event_handler(e):
    code = e.get_code()

screen_user_btn.add_event_cb(lambda e: screen_user_btn_event_handler(e), lv.EVENT.ALL, None)

def screen_btnm_1_event_handler(e):
    code = e.get_code()

screen_btnm_1.add_event_cb(lambda e: screen_btnm_1_event_handler(e), lv.EVENT.ALL, None)

def screen_ta_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.FOCUSED):
        screen_ta_1.set_style_text_font(test_font("montserratMedium", 12), 0)
        screen_ta_1.set_text("")

screen_ta_1.add_event_cb(lambda e: screen_ta_1_event_handler(e), lv.EVENT.ALL, None)

def screen_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        screen_cont_2.add_flag(lv.obj.FLAG.HIDDEN);screen_cont_1.clear_flag(lv.obj.FLAG.HIDDEN)

screen_btn_1.add_event_cb(lambda e: screen_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_calendar_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.LONG_PRESSED):
        screen_calendar_1.add_flag(lv.obj.FLAG.HIDDEN);screen_cont_1.clear_flag(lv.obj.FLAG.HIDDEN)

screen_calendar_1.add_event_cb(lambda e: screen_calendar_1_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.scr_load(screen)

while SDL.check():
    time.sleep_ms(5)

