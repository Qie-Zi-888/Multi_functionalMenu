#ifndef __PAGE_H__
#define __PAGE_H__

typedef enum {
	HOME,
	WEATHER,
	CHESS,
    AICHAT,   
    BSP_CRTL,
	WAVE,
	SORT
} PAGE;
extern PAGE pageflag;
extern unsigned char SENDFLAG,ENTERFLAG;
extern unsigned char chatcount;
#endif