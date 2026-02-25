/**
 * @file esp8266.h
 * @author KiEa
 * @brief esp8266 at device library
 * @version 0.1
 * @date 2023-08-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef ESP8266_H
#define ESP8266_H

/* includes ******************************************************************/

#include "at32f435_437.h"
#include "drv_uart.h"
#include "wlan.h"
#include <time.h>

/* defines *******************************************************************/

#define BSP_USING_ESP8266 1

#define ESP_NETDEV_NAME "esp8266"

#define MAX_AT_CMD 128
#define ESP8266_TX_BUFSIZ 512
#define ESP8266_RX_BUFSIZ 512

/*  */

/* functions *****************************************************************/

uint8_t esp8266_get_netdev_info(netdev_t netdev);
void esp8266_set_up(netdev_t dev);
uint8_t esp8266_set_addr_info(netdev_t netdev, char *ip_addr, char *netmask, char *gw);
uint8_t esp8266_set_dns_server(netdev_t netdev, uint8_t dns_num, char *dns_server);
uint8_t esp8266_set_dhcp(netdev_t netdev, uint8_t is_enabled);
uint8_t esp8266_join_ap(wlan_dev_t dev);
uint8_t esp8266_disconnect_ap(wlan_dev_t dev);
uint8_t esp8266_scan(wlan_dev_t dev);
uint8_t esp8266_ping(netdev_t netdev, const char *domain, uint32_t *time);
// uint8_t esp8266_set_sntp(netdev_t netdev, uint8_t timezone, const char *sntp_server);
// uint8_t esp8266_get_stnp_time(netdev_t netdev, struct tm *time);

/* esp8266 socket function */

int esp8266_socket(netdev_t dev,int domain, int type, int protocol);
int esp8266_connect(netdev_t dev ,int sockfd, const struct network_sockaddr *serv_addr, int addrlen);
int esp8266_recv(netdev_t dev, int sockfd, void *buf, int len, int flags);
int esp8266_send(netdev_t dev, int sockfd, const void *buf, size_t len, int flags);
int esp8266_recvfrom(netdev_t dev, int sockfd, void *buf, int len, int flags, struct network_sockaddr *from, size_t *fromlen);
int esp8266_sendto(netdev_t dev, int sockfd, const void *msg, size_t len, int flags, const struct network_sockaddr *to, size_t tolen);
int esp8266_closesocket(netdev_t dev, int sockfd);
uint32_t esp8266_gethostbyname(netdev_t dev, const char *name);

#endif // ESP8266_H
