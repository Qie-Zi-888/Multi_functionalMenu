#ifndef __OLEDWork_H
#define __OLEDWork_H			  	 

 #include "at32f435_437_board.h" 

#define OLED_CMD  0	//
#define OLED_DATA 1	//
#define OLED_MODE 0




//OLED模式设置
//0:4线串行模式
//1:并行8080模式

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED???��?����?----------------  					   

//void delay_ms(unsigned int ms);

typedef unsigned long       U32;
typedef unsigned short      U16;
typedef unsigned char       U8;
	     


void oledWriteByte(u8 dat,u8 cmd);	    
void oledDisplayOn(void);
void oledDisplayOff(void);	   							   		    
void oledInit(void);
void oledClear(void);
void oledDrawPoint(u8 x,u8 y,u8 t);
void oledFill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void oledShowChar(u8 x,u8 y,u8 chr);

void oledShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2);

void oledShowString(u8 x,u8 y, u8 *p);	 
void oledSetPos(unsigned char x, unsigned char y);
void oledShowCHinese(u8 x,u8 y,u8 no);
void oledDrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void oledShowCHStr(unsigned char X , unsigned char Y, unsigned char *pstr);
#endif  
	 



