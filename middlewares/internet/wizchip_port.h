#ifndef __WIZCHIP_PORT_H
#define __WIZCHIP_PORT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/

#include "at32f435_437.h"
#include "lwip/pbuf.h"
#include "wizchip_conf.h"
#include "w5500.h"

/* public functions **********************************************************/

/**
 * @brief w5500 Íø¿¨Çý¶¯³õÊ¼»¯
 * 
 */
void wizchip_lowlevel_setup(void);

/**
 * @brief w5500 Íø¿¨ÖØÆô
 * 
 */
void wizchip_reset(void);

/**
 * @brief w5500 Íø¿¨º¯Êý×¢²á
 * 
 */
void wizchip_register(void);

void wizchip_transmit_pbuf(struct pbuf *p);
int wizchip_read_receive_pbuf(struct pbuf **buf);

void wizchip_initialize(void);
void wizchip_check(void);
void network_initialize(wiz_NetInfo net_info);
void print_network_information(wiz_NetInfo net_info);

#ifdef __cplusplus
}
#endif

#endif //__WIZCHIP_PORT_H