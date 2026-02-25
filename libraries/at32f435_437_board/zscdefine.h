/***************************************************************************
* Tilte : The Globle define file of the system
* 
* File: zscdefine.h 
*
* Discription: This file contain the Globle define. It  depend the system platform,such as CPU
core, complie, linker 

**************************************************************************/

#ifndef _ZSCDEFINE_H
#define _ZSCDEFINE_H



#ifdef INTIS16BIT
typedef unsigned char   U8;       
typedef signed   char  	S8;      
typedef unsigned int   	U16;     
typedef signed   int   	S16;      
typedef unsigned long       U32  ;     
typedef signed   long  	S32;      
typedef float          	FP32;        
typedef double         	FP64;         
#else
typedef signed long      S32;
typedef signed short     S16;
typedef signed char      S8;

typedef volatile signed long      VS32;
typedef volatile signed short     VS16;
typedef volatile signed char      VS8;

typedef unsigned long       U32;
typedef unsigned short      U16;
typedef unsigned char       U8;

typedef volatile unsigned long      VU32;
typedef volatile unsigned short     VU16;
typedef volatile unsigned char      VU8;

typedef volatile unsigned long  const    VUC32;  /* Read Only */
typedef volatile unsigned short const    VUC16;  /* Read Only */
typedef volatile unsigned char  const    VUC8;   /* Read Only */
#endif
typedef	unsigned char 			BYTE;
typedef unsigned short		WORD;
typedef unsigned long 		DWORD;


//--Debug output define
#define DEBUG

#ifdef DEBUG     /* for debug vision*/
#define DBG(_args) 	_args
#define HW_DBG(_args)  _args
#else 
#define DBG(_args)
#define HW_DBG(_args)  
#endif


//-- 功能宏定义
#define LBYTE(i)             (unsigned char)(i&0xFF)
#define HBYTE(i)            (unsigned char)((i>>8)&0xFF)
#define MIN(A,B)           ((A < B) ? A : B)
#define MAX(A,B)           ((A > B) ? A : B)
#define BCD(i)             ((i&0x0f) + ((i>>4)&0x0F)*10)
#define DCB(i)             (((i%10)&0x0f) | ((i/10)<<4)&0xF0)
#define GetBit(i,b)        (i & (0x01<<b))
#define SetBit(i,b)        (i |= (0x01<<b))
#define ClrBit(i,b)        (i &= (~(0x01<<b)))
#define OutPortB(port,cb)  *(unsigned char*)(port) = (unsigned char)(cb)
#define InPortB(port)      *(unsigned char*)(port)
#define InPortW(port)      *(unsigned char*)(port)*0x100 + *(unsigned char*)(port+1)
#define MAKEWORD(a, b)      ((U16)((U8)(b) & 0xff) | ((U16)((U8)(a) & 0xff) << 8)) 


#define MASKBIT0	((U8)0x01)
#define MASKBIT1	((U8)0x02)
#define MASKBIT2	((U8)0x04)
#define MASKBIT3	((U8)0x08)
#define MASKBIT4	((U8)0x10)
#define MASKBIT5	((U8)0x20)
#define MASKBIT6	((U8)0x40)
#define MASKBIT7	((U8)0x80)





#ifdef HOST_ORDER_NOT_AS_NET
extern U32 ntohl(U32 in);
extern U16  ntohs(U16 in);
extern S16 ntohs_S16(S16 in) ;

#define htonl(in)   ntohl( in)
#define htons(in)  ntohs( in)
#else
#define	ntohl(in)		in
#define	ntohs(in)	in
#endif


// --the FLAGS define
//#ifndef FALSE
//#define FALSE           0
//#endif

#ifndef TRUE
#define TRUE            (!FALSE)
#endif

#ifndef  true 
#define false		0
#define true 		(!false)
#endif

#ifndef Invalid
#define Invalid		0
#define Valid			(!Invalid)
#endif

// Define NULL
#ifndef  NULL 
#define NULL ((void *)(0))
#endif

#ifndef FAIL
#define FAIL            0
#endif

#ifndef SUCCESS
#define  SUCCESS      (!FAIL)   
#endif

#ifndef NOTACTIVE
#define NOTACTIVE          0
#endif

#ifndef ACTIVE
#define  ACTIVE           (!NOTACTIVE)
#endif

#define U8_MAX     ((u8)255)
#define S8_MAX     ((s8)127)
#define S8_MIN     ((s8)-128)
#define U16_MAX    ((u16)65535u)
#define S16_MAX    ((s16)32767)
#define S16_MIN	   ((s16)-32768)
#define U32_MAX	   ((u32)4294967295uL)
#define S32_MAX    ((s32)2147483647)
#define S32_MIN    ((s32)-2147483648)

#endif

