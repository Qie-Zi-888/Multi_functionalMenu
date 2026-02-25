#ifndef __OLEDFONT_H
#define __OLEDFONT_H 	   


typedef struct
{
    unsigned char value[2];   //GBK
	unsigned char data[24];   //
}HZZM_12;

extern const unsigned char chinese_12_len;
extern unsigned char  Hzk[][32];
extern const unsigned char  F6x8[][6];
extern const unsigned char  F8X16[];
extern const HZZM_12 chinese_12[5];

#endif


