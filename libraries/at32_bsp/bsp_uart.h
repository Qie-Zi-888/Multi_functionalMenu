
/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart.h
*********************************************************************************************************
*/

#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "bsp.h"

#define	UART3_FIFO_EN	1


/**************** define print uart ******************/
#define ESP12_UART                       USART3
#define ESP12_UART_CRM_CLK               CRM_USART3_PERIPH_CLOCK
#define ESP12_UART_TX_PIN                GPIO_PINS_10
#define ESP12_UART_TX_GPIO               GPIOC
#define ESP12_UART_TX_GPIO_CRM_CLK       CRM_GPIOC_PERIPH_CLOCK
#define ESP12_UART_TX_PIN_SOURCE         GPIO_PINS_SOURCE10
#define ESP12_UART_TX_PIN_MUX_NUM        GPIO_MUX_7

#define ESP12_UART_RX_PIN                GPIO_PINS_11
#define ESP12_UART_RX_GPIO               GPIOC
#define ESP12_UART_RX_GPIO_CRM_CLK       CRM_GPIOC_PERIPH_CLOCK
#define ESP12_UART_RX_PIN_SOURCE         GPIO_PINS_SOURCE11
#define ESP12_UART_RX_PIN_MUX_NUM        GPIO_MUX_7





/* PB2 控制RS485芯片的发送使能 */
#define RS485_TXEN_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define RS485_TXEN_GPIO_PORT              GPIOB
#define RS485_TXEN_PIN                    GPIO_PIN_2

#define RS485_RX_EN()	RS485_TXEN_GPIO_PORT->BSRRH = RS485_TXEN_PIN
#define RS485_TX_EN()	RS485_TXEN_GPIO_PORT->BSRRL = RS485_TXEN_PIN

/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1 */
	COM2 = 1,	/* USART2 */
	COM3 = 2,	/* USART3 */
	COM4 = 3,	/* UART4 */
	COM5 = 4,	/* UART5 */
	COM6 = 5,	/* USART6 */
	COM7 = 6,	/* UART7 */	
	COM8 = 7	/* UART8 */	
}COM_PORT_E;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART3_FIFO_EN == 1
	#define UART3_BAUD			9600
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif

/* 串口设备结构体 */
typedef struct
{
	usart_type *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;	/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;	/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
	uint8_t Sending;			/* 正在发送中 */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);

void USART_SetBaudRate(usart_type* USARTx, uint32_t BaudRate);
void bsp_SetUartParam(usart_type *Instance,  uint32_t BaudRate, uint32_t Parity, uint32_t Mode);

void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(char *_pBuf);
void RS485_SetBaud(uint32_t _baud);
uint8_t UartTxEmpty(COM_PORT_E _ucPort);
void esp12_uart_init(uint32_t baudrate);
#endif