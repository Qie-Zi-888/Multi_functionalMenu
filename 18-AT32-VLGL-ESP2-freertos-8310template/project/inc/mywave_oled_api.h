#ifndef _OLED_H_
#define _OLED_H_

#include <stdint.h>
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void show_title(void);
void show_gif(void);
	
void Before_State_Update(uint8_t y);
void Current_State_Update(uint8_t y);
void OLED_DrawWave(uint8_t x,uint8_t y);

#endif
