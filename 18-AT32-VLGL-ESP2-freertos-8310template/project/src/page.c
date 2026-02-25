#include "page.h"

unsigned char SENDFLAG,ENTERFLAG=0;

PAGE pageflag=HOME;

void pageHandler()
{
//	switch(pageflag)
//	{
//		case AICHAT:
//			if()
//			break;
//	}
}
//void touch_event_handler(lv_indev_drv_t * indev, lv_indev_data_t * data) {
//    // 获取触摸点的坐标
//    static int16_t last_x = 0;
//    static int16_t last_y = 0;
//    int16_t x = touchpad_get_x();
//    int16_t y = touchpad_get_y();

//    // 检测是否有触摸发生
//    if (touchpad_is_pressed()) {
//        // 计算滑动距离
//        int16_t delta_x = x - last_x;
//        int16_t delta_y = y - last_y;

//        // 判断滑动方向和长度
//        if (abs(delta_x) > abs(delta_y)) {
//            if (abs(delta_x) >= THRESHOLD) {
//                if (delta_x > 0) {
//                    // 向右滑动
//                    // 执行相应的操作
//                } else {
//                    // 向左滑动
//                    // 执行相应的操作
//                }
//            }
//        } else {
//            if (abs(delta_y) >= THRESHOLD) {
//                if (delta_y > 0) {
//                    // 向下滑动
//                    // 执行相应的操作
//                } else {
//                    // 向上滑动
//                    // 执行相应的操作
//                }
//            }
//        }

//        // 更新上一次触摸点的位置
//        last_x = x;
//        last_y = y;
//    } else {
//        // 没有触摸发生时，重置上一次触摸点的位置
//        last_x = 0;
//        last_y = 0;
//    }
//}
