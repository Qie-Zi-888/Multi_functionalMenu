/**
 * @file network.h
 * @author KiEa
 * @brief network framework library
 * @version 0.1
 * @date 2023-08-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef NETWORK_H
#define NETWORK_H

/* includes ******************************************************************/

#include "at32f435_437.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <time.h>

/* defines *******************************************************************/

#define BSP_USING_ESP12

#define NETDEV_NAME_LEN             20

#define WLAN_SSID_LEN               30
#define WLAN_PWD_LEN                30

#define network_malloc              pvPortMalloc
#define network_free                vPortFree

#if !defined(network_malloc)
    #define network_malloc malloc
#endif

#if !defined(network_free)
    #define network_free free
#endif

/* ntp */
#define NTP_ALIYUN_HOST             "120.25.115.20"
#define NETWORK_NTP_TIMEZONE        8
#define NTP_ALIYUN_PORT             123

/* 用下面的宏， 必须确保 netdev_t 对象（指针类型）变量名为 'dev' */
#define netdev_setup                (((netdev_t)dev)->ops->set_up)
#define netdev_getinfo              (((netdev_t)dev)->ops->get_netinfo)
#define netdev_set_dns              ()
#define netdev_ping                 (((netdev_t)dev)->ops->ping)

#define netdev_link_up_cb           (((netdev_t)dev)->callback.link_up)
#define netdev_link_down_cb         (((netdev_t)dev)->callback.link_down)
#define netdev_internet_up_cb       (((netdev_t)dev)->callback.internet_up)
#define netdev_internet_down_cb     (((netdev_t)dev)->callback.internet_down)

#define netdev_is_flag(flag)        ((((netdev_t)dev)->flags) & (flag))
#define netdev_set_flag(flag)       ((((netdev_t)dev)->flags) |= (flag))
#define netdev_clear_flag(flag)     ((((netdev_t)dev)->flags) &= ~(flag))

/* enums *********************************************************************/

enum netdev_flag_e
{
    NETDEV_FLAG_UP = 0x01,
    NETDEV_FLAG_LINK_UP = 0x02,
    NETDEV_FLAG_INTERNET_UP = 0x04,
    NETDEV_FLAG_DHCP = 0x08,
};

enum netdev_cb_e
{
    NETWORK_LINK_UP_CB,
    NETWORK_LINK_DOWN_CB,
    NETWORK_INTERNET_UP_CB,
    NETWORK_INTERNET_DOWN_CB,
};

enum network_socket_domain_e
{
    AF_INET = 0x01,    /* ipv4 */
    AF_INET6 = 0x02,   /* ipv6 */
    AF_AT = 0x04,      /* at cmd */
};

enum network_socket_type_e
{
    SOCK_STREAM = 0x01, /* tcp */
    SOCK_SDGRAM = 0x02, /* udp */
    SOCK_RAW = 0x04,    /* mac raw (may be) */
};

enum network_socket_status_e
{
    SOCK_STATUS_INIT,
    SOCK_STATUS_CONN,
    SOCK_STATUS_DISCONN
};

/* structures ****************************************************************/
typedef struct netdev *netdev_t;
typedef struct wlan_dev *wlan_dev_t;
typedef struct network_socket *network_socket_t;
typedef struct wlan_ap_list  *wlan_ap_list_t;

struct netdev_ops
{
    void (*set_up)(netdev_t netdev);
    uint8_t (*set_addr_info)(netdev_t netdev, char *ip_addr, char *netmask, char *gw);
    uint8_t (*set_dns_server)(netdev_t netdev, uint8_t dns_num, char *dns_server);
    uint8_t (*set_dhcp)(netdev_t netdev, uint8_t is_enabled);
    uint8_t (*get_netinfo)(netdev_t netdev);    
    uint8_t (*ping)(netdev_t netdev, const char *domain, uint32_t *time);
};

struct netdev_cb
{
    void *link_up_arg;
    void (*link_up)(void *param);
    void *link_down_arg;
    void (*link_down)(void *param);
    void *internet_up_arg;
    void (*internet_up)(void *parma);
    void *internet_down_arg;
    void (*internet_down)(void *param);
};

struct network_in_addr {
    union
    {
        struct {uint8_t s_b1, s_b2, s_b3, s_b4;} S_un_b;
        struct {uint16_t s_w1, s_w2;} S_un_w;
        uint32_t S_addr;
    } S_un;
};

struct network_sockaddr
{
    uint16_t sa_family;
    uint8_t sa_data[14];
};

struct network_sockaddr_in
{
    uint16_t sin_family;
    uint16_t sin_port;
    struct network_in_addr sin_addr;
    uint8_t sin_zero[8];
};

struct network_socket_ops
{
    int (*socket)(netdev_t dev, int domain, int type, int protocol);
    int (*connect)(netdev_t dev, int sockfd, const struct network_sockaddr *serv_addr, int addrlen);
    int (*recv)(netdev_t dev, int sockfd, void *buf, int len, int flags);
    int (*send)(netdev_t dev, int sockfd, const void *buf, size_t len, int flags);
    int (*recvfrom)(netdev_t dev, int sockfd, void *buf, int len, int flags, struct network_sockaddr *from, size_t *fromlen);
    int (*sendto)(netdev_t dev, int sockfd, const void *msg, size_t len, int flags, const struct network_sockaddr *to, size_t tolen);
    int (*closesocket)(netdev_t dev, int sockfd);
    uint32_t (*gethostbyname)(netdev_t dev, const char *name);
};

struct network_socket
{
    int sockfd;
    uint8_t type;
    uint8_t domain;

    uint8_t status;
    char *buf;
    uint16_t head;
    uint16_t tail;
    uint16_t capacity;
    uint32_t timeout;

    SemaphoreHandle_t notice;
    
    struct network_socket *next;
};


struct netdev
{
    char name[NETDEV_NAME_LEN]; // the name of network card
    char hwaddr[18];            // Formatted MAC address "xx:xx:xx:xx:xx:xx"
    char ip[16];                // IPv4 address "xxx.xxx.xxx.xxx"
    char netmask[16];           // IPv4 subnet mask "xxx.xxx.xxx.xxx"
    char gatway[16];            // IPv4 gateway "xxx.xxx.xxx.xxx"
    char dns[16];               // IPv4 DNS server "xxx.xxx.xxx.xxx"
    void *work_data;

    uint16_t flags; // status flags
    struct netdev_ops *ops;
    struct network_socket_ops *sockops;
    struct netdev_cb callback;
};

struct wlan_ops
{
    uint8_t (*connect)(wlan_dev_t dev);
    uint8_t (*disconnect)(wlan_dev_t dev);
    uint8_t (*scan)(wlan_dev_t dev);
};

struct wlan_dev
{
    struct netdev parent;
    char ssid[WLAN_SSID_LEN];
    char pwd[WLAN_PWD_LEN];
    
    struct wlan_ops *ops;
};

struct wlan_ap_list
{
	char ssid[33];	/* SSID是一个无线局域网络（WLAN）的名称。SSID是区分大小写的文本字符串，最大长度32个字符 */
	uint8_t ecn;	/* 加密方式 
						0   OPEN
						1   WEP
						2   WPA_PSK
						3   WPA2_PSK
						4   WPA_WPA2_PSK
					*/
	int32_t rssi;		/* 信号强度 */
	uint8_t mac[20];	/* MAC地址字符串*/
	uint8_t ch;			/* 信道 */

    struct wlan_ap_list *next;
};

/* variables *****************************************************************/ 

extern TaskHandle_t network_handler;

/* functions *****************************************************************/

/* wlan  */
wlan_dev_t wlan_create(const char *name);
uint8_t wlan_connect(wlan_dev_t dev, const char *ssid, const char *pwd);
uint8_t wlan_disconnect(wlan_dev_t dev);
uint8_t wlan_scan_ap(wlan_dev_t dev);
uint8_t wlan_add_ap_to_list(wlan_ap_list_t wlan_ap);
void wlan_iter_ap_list(void (*func)(wlan_ap_list_t _ap));
void wlan_delete_ap_list(void);

/* network framework */

void network_task_functoin(void *pvParameters);
void network_notice_link_up(netdev_t dev);
void network_notice_link_down(netdev_t dev);
uint8_t network_set_netdev_status_cb(netdev_t dev, uint8_t cb_type, void (*callback)(void *param), void *param);
uint8_t network_register_netdev(struct netdev *dev);
netdev_t network_get_netdev(void);
uint8_t network_get_ntptime(time_t *timestamp);
void network_timestamp_to_tm(time_t *timestamp, struct tm *time);

/* lowlevel socket */

int network_lowlevel_create_socket(network_socket_t *socket);
network_socket_t network_lowlevel_get_socket(int sockfd);
void network_lowlevel_closesocket(int sockfd);
uint8_t network_lowlevel_delete_socket(int sockfd);
uint8_t network_lowlevel_socket_write_byte(network_socket_t socket, const char ch);
uint8_t network_lowlevel_socket_read_byte(network_socket_t socket, char *ch);

/* socket */

uint16_t network_htons(uint16_t hostshort);
uint16_t network_ntohs(uint16_t netshort);
uint32_t network_htonl(uint32_t hostlong);
uint32_t network_ntohl(uint32_t netlong);
uint32_t network_inet_addr(const char *addr);
uint32_t network_gethostbyname(const char *name);
char *network_inet_ntoa(uint32_t addr);

int network_socket(int domain, int type, int protocol);
int network_connect(int sockfd, const struct network_sockaddr *serv_addr, int addrlen);
int network_recv(int sockfd, void *buf, int len, int flags);
int network_send(int sockfd, const void *buf, size_t len, int flags);
int network_recvfrom(int sockfd, void *buf, int len, int flags, struct network_sockaddr *from, size_t *fromlen);
int network_sendto(int sockfd, const void *msg, size_t len, int flags, const struct network_sockaddr *to, size_t tolen);
int network_closesocket(int sockfd);
int network_socket_list_num(void);
uint8_t network_set_socket_timeout(int sockfd, uint32_t timeout);

#endif // NETWORK_H
