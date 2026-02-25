#include "esp8266.h"
#include "at.h"
#include "bsp_timer.h"
#include "at32f435_437.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define ESP8266_STD_TIMEOUT 3000 /* standard timeout is 3s */

static SemaphoreHandle_t dtu_mutex = NULL; /* 透传锁 */

/* public functions **********************************************************/

void esp8266_set_up(netdev_t dev)
{
    at_master_t master = (at_master_t)dev->work_data;
    dtu_mutex = xSemaphoreCreateMutex();

    /* 恢复出厂设置 */
    if (!at_exec_cmd_wait_expr(master, "ready", "AT+RESTORE", 10, ESP8266_STD_TIMEOUT))
        return;
    netdev_set_flag(NETDEV_FLAG_UP);

    /* 关闭回显 */
    if (!at_exec_cmd_wait_expr(master, "OK", "ATE0", 4, ESP8266_STD_TIMEOUT))
        return;

    /* 设置 ESP8266 为 Station 模式 */
    if (!at_exec_cmd_wait_expr(master, "OK", "AT+CWMODE_CUR=1", 15, ESP8266_STD_TIMEOUT))
        return;

    /* 设置为多连接模式 */
    if (!at_exec_cmd_wait_expr(master, "OK", "AT+CIPMUX=1", 12, ESP8266_STD_TIMEOUT))
        return;
}

uint8_t esp8266_set_addr_info(netdev_t netdev, char *ip_addr, char *netmask, char *gw)
{
    return 0;
}

uint8_t esp8266_get_netdev_info(netdev_t netdev)
{
    uint8_t ret = 0;
    char *expr = "%*[^\"]\"%[^\"]\"";
    at_resp_t resp = at_create_resp(128, ESP8266_STD_TIMEOUT);
    at_master_t master = (at_master_t)netdev->work_data;

    /* 获取 mac 地址 */
    // +CIPSTAMAC_CUR:"xx:xx:xx:xx:xx:xx"
    if (!at_exec_cmd(master, resp, "AT+CIPSTAMAC_CUR?", 17))
        goto _exit_func;
    at_parse_resp_by_kw(resp, expr, "+CIPSTAMAC_CUR", netdev->hwaddr);

    /* 获取 ip, gateway, netmask */
    if (!at_exec_cmd(master, resp, "AT+CIPSTA_CUR?", 14))
        goto _exit_func;

    // +CIPSTA_CUR:ip:"xxx.xxx.xxx.xxx"
    at_parse_resp_by_kw(resp, (const char *)expr, "+CIPSTA_CUR:ip", netdev->ip);
    // +CIPSTA_CUR:gateway:"xxx.xxx.xxx.xxx"
    at_parse_resp_by_kw(resp, (const char *)expr, "+CIPSTA_CUR:gateway", netdev->gatway);
    // +CIPSTA_CUR:netmask:"xxx.xxx.xxx.xxx"
    at_parse_resp_by_kw(resp, (const char *)expr, "+CIPSTA_CUR:netmask", netdev->netmask);

    /* 获取 dns */
    if (!at_exec_cmd(master, resp, "AT+CIPDNS_CUR?", 14))
        goto _exit_func;
    at_parse_resp_by_kw(resp, "%*[^:]:%s", "+CIPDNS_CUR", netdev->dns);
    ret = 1;

_exit_func:
    at_free_resp(resp);
    return ret;
}

uint8_t esp8266_set_dns_server(netdev_t netdev, uint8_t dns_num, char *dns_server)
{
    return 0;
}

uint8_t esp8266_set_dhcp(netdev_t netdev, uint8_t is_enabled)
{
    return 0;
}

uint8_t esp8266_ping(netdev_t netdev, const char *domain, uint32_t *time)
{
    uint8_t ret = 0;
    at_resp_t resp = at_create_resp(64, 15000); // wait 15s
    at_master_t master = (at_master_t)netdev->work_data;
    *time = 0;

    char cmd[32] = {0};
    sprintf(cmd, "AT+PING=\"%s\"", domain);

    xSemaphoreTake(dtu_mutex, portMAX_DELAY);

    if (at_exec_cmd(master, resp, (const char *)cmd, strlen(cmd)))
    {
        at_parse_resp_by_kw(resp, "+%u", "+", (const char *)time);

        char result[10];
        at_parse_resp_by_kw(resp, "%s", "O", (const char *)result);
        if (strstr(result, "OK"))
            ret = 1;
    }

    at_free_resp(resp);
    xSemaphoreGive(dtu_mutex);
    return ret;
}

uint8_t esp8266_join_ap(wlan_dev_t dev)
{
    uint8_t ret = 0;
    at_master_t master = (at_master_t)dev->parent.work_data;
    if (!(((netdev_t)dev)->flags & NETDEV_FLAG_UP))
        return 0;

    /* 拼接 WiFi 连接的 AT 命令， AT+CWJAP_CUR=<ssid>,<pwd>,[<bssid>][,<pci_en>] */
    char cmd[64];
    sprintf(cmd, "AT+CWJAP_CUR=\"%s\",\"%s\"", dev->ssid, dev->pwd);

    xSemaphoreTake(dtu_mutex, portMAX_DELAY);
    ret = at_exec_cmd_wait_expr(master, "OK", (const char *)cmd, strlen(cmd), 15000);
    xSemaphoreGive(dtu_mutex);
    return ret;
}

uint8_t esp8266_disconnect_ap(wlan_dev_t dev)
{
    at_master_t master = (at_master_t)dev->parent.work_data;
    if (netdev_is_flag(NETDEV_FLAG_LINK_UP))
    {
        netdev_clear_flag(NETDEV_FLAG_LINK_UP);

        xSemaphoreTake(dtu_mutex, portMAX_DELAY);
        uint8_t ret = at_exec_cmd_wait_expr(master, "OK", "AT+CWQAP", 8, ESP8266_STD_TIMEOUT);
        xSemaphoreGive(dtu_mutex);
        return ret;
    }
    return 0;
}

uint8_t esp8266_scan(wlan_dev_t dev)
{
    if (!(((netdev_t)dev)->flags & NETDEV_FLAG_UP))
        return 0;
    at_master_t master = (at_master_t)dev->parent.work_data;

    xSemaphoreTake(dtu_mutex, portMAX_DELAY);
    uint8_t ret = at_exec_cmd_wait_expr(master, "OK", "AT+CWLAP", 8, 10000);
    xSemaphoreGive(dtu_mutex);
    return ret;
}

// uint8_t esp8266_set_sntp(netdev_t netdev, uint8_t timezone, const char *sntp_server)
// {
//     at_master_t master = (at_master_t)netdev->work_data;

//     char cmd[64] = {0};
//     sprintf(cmd, "AT+CIPSNTPCFG=1,%d,\"%s\"", timezone, sntp_server);
//     return at_exec_cmd_wait_expr(master, "OK", (const char *)cmd, strlen(cmd), ESP8266_STD_TIMEOUT);
// }

// uint8_t esp8266_get_stnp_time(netdev_t netdev, struct tm *time)
// {
//     uint8_t ret = 0;
//     at_master_t master = (at_master_t)netdev->work_data;

//     /* 动态创建 resp 对象 */
//     at_resp_t resp = at_create_resp(128, ESP8266_STD_TIMEOUT);
//     if (NULL == resp)
//         goto _exit_func;

//     /* 发送获取 sntp 时间的 AT 命令，AT+CIPSNTPTIME? */
//     if (!at_exec_cmd(master, resp, "AT+CIPSNTPTIME?", 15))
//         goto _exit_func;

//     /* 从接收对象中找到时间所在的行 */
//     char *kw_line = at_get_resp_line_by_kw(resp, "+CIPSNTPTIME");
//     if (NULL == kw_line)
//         goto _exit_func;

//     /* 解析，返回数据格式：+CIPSNTPTIME:Thu	Aug	04 14:48:05 2016 */
//     char wday_str[10];
//     char month_str[10];
//     if (0 == at_parse_str(kw_line, "%*[^:]:%s %s %d %d:%d:%d %d",
//                           wday_str, month_str, &(time->tm_mday), &(time->tm_hour),
//                           &(time->tm_min), &(time->tm_sec), &(time->tm_year)))
//         goto _exit_func;

//     /* 数据转换 */
//     /* 周天 */
//     const char *wdays[] = {
//         "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
//     int i = 0;
//     for (i = 0; i < 7; i++)
//     {
//         if (0 == strcmp(wday_str, wdays[i]))
//             break;
//     }

//     if (i == 7)
//         goto _exit_func;

//     time->tm_wday = i;

//     /* 月份 */
//     const char *months[] = {
//         "Jan", "Feb", "Mar", "Apr", "May", "Jun",
//         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
//     for (i = 0; i < 12; i++)
//     {
//         if (0 == strcmp(month_str, months[i]))
//             break;
//     }

//     if (i == 12)
//         goto _exit_func;

//     time->tm_mon = i;
//     ret = 1;
// _exit_func:
//     at_free_resp(resp);
//     return ret;
// }

int esp8266_socket(netdev_t dev, int domain, int type, int protocol)
{
    if (domain & AF_INET6 || type & SOCK_RAW)
        return -1;

    network_socket_t socket;
    int sockfd = -1;
    if ((sockfd = (network_lowlevel_create_socket(&socket))) != -1)
    {
        if (sockfd >= 5) /* esp8266 最大只能 5 个 socket通道（0-4） */
        {
            printf("sockfd over 5: %d\r\n", sockfd);
            network_lowlevel_delete_socket(sockfd);
            return -1;
        }

        socket->domain = domain;
        socket->type = type;
        socket->status = SOCK_STATUS_INIT;
        return sockfd;
    }
    return -1;
}

int esp8266_connect(netdev_t dev, int sockfd, const struct network_sockaddr *serv_addr, int addrlen)
{
    int ret = -1;
    network_socket_t socket;
    at_master_t master = (at_master_t)dev->work_data;

    if ((socket = network_lowlevel_get_socket(sockfd)) == NULL)
        goto _exit_func;

    char cmd[64] = {0};
    uint16_t port = network_ntohs(*(uint16_t *)(serv_addr->sa_data));
    char *ip_str = network_inet_ntoa(*(uint32_t *)&(serv_addr->sa_data[2]));

    xSemaphoreTake(dtu_mutex, portMAX_DELAY);

    if (socket->status == SOCK_STATUS_CONN)
    {
        return 0;
    }
    else if (socket->type == SOCK_STREAM) /* tcp */
    {
        sprintf(cmd, "AT+CIPSTART=%u,\"TCP\",\"%s\",%u", sockfd, ip_str, port);
        if (!at_exec_cmd_wait_expr(master, "OK", cmd, strlen(cmd), ESP8266_STD_TIMEOUT))
            goto _exit_func;
        socket->status = SOCK_STATUS_CONN;
    }
    else if (socket->type == SOCK_SDGRAM) /* udp */
    {
        sprintf(cmd, "AT+CIPSTART=%u,\"UDP\",\"%s\",%u", sockfd, ip_str, port);
        if (!at_exec_cmd_wait_expr(master, "OK", cmd, strlen(cmd), ESP8266_STD_TIMEOUT))
            goto _exit_func;
        socket->status = SOCK_STATUS_CONN;
    }
    else
    {
        network_lowlevel_closesocket(sockfd);
        goto _exit_func;
    }

    ret = 0;

_exit_func:
    vPortFree(ip_str);
    xSemaphoreGive(dtu_mutex);
    return ret;
}

int esp8266_recv(netdev_t dev, int sockfd, void *buf, int len, int flags)
{
    if (!netdev_is_flag(NETDEV_FLAG_LINK_UP))
        return -1;

    network_socket_t socket = network_lowlevel_get_socket(sockfd);
    if (socket == NULL)
        printf("socket is null\r\n");
    if (pdFALSE == xSemaphoreTake(socket->notice, socket->timeout))
        return 0;

    if (socket->status == SOCK_STATUS_DISCONN)
        return -1;

    int i = 0;
    for (i = 0; i < len; i++)
    {
        if (!network_lowlevel_socket_read_byte(socket, &(((char *)buf)[i])))
            break;
    }
    return i;
}

int esp8266_send(netdev_t dev, int sockfd, const void *buf, size_t len, int flags)
{
    at_master_t master = (at_master_t)dev->work_data;
    /* AT+CIPSEND=<link	ID>,<length> */
    char cmd[32];
    sprintf(cmd, "AT+CIPSEND=%d,%d", sockfd, len);

    /* 这里应该等待的是 > ，但无法等到 */
    xSemaphoreTake(dtu_mutex, portMAX_DELAY);
    if (at_exec_cmd_wait_expr(master, "OK", (const char *)cmd, strlen(cmd), ESP8266_STD_TIMEOUT))
    {
        uint8_t ret = at_exec_cmd_wait_expr(master, "OK", (const char *)buf, len, ESP8266_STD_TIMEOUT);
        xSemaphoreGive(dtu_mutex);
        return ret;
    }
    xSemaphoreGive(dtu_mutex);
    return -1;
}

int esp8266_recvfrom(netdev_t dev, int sockfd, void *buf, int len, int flags, struct network_sockaddr *from, size_t *fromlen)
{
    return esp8266_recv(dev, sockfd, buf, len, flags);
}

int esp8266_sendto(netdev_t dev, int sockfd, const void *msg, size_t len, int flags, const struct network_sockaddr *to, size_t tolen)
{
    if (0 == esp8266_connect(dev, sockfd, to, tolen)) /* true */
    {
        return esp8266_send(dev, sockfd, msg, len, flags);
    }
    return -1;
}

int esp8266_closesocket(netdev_t dev, int sockfd)
{
    at_master_t master = (at_master_t)dev->work_data;
    /* cmd: AT+CIPCLOSE=<link	ID> */
    char cmd[20] = {0};
    sprintf(cmd, "AT+CIPCLOSE=%d", sockfd);
    xSemaphoreTake(dtu_mutex, portMAX_DELAY);
    if (at_exec_cmd_wait_expr(master, "OK", cmd, strlen(cmd), ESP8266_STD_TIMEOUT))
    {
        xSemaphoreGive(dtu_mutex);
        return 0;
    }
    xSemaphoreGive(dtu_mutex);
    return -1;
}

uint32_t esp8266_gethostbyname(netdev_t dev, const char *name)
{
    uint32_t ret = 0;
    // cmd: AT+CIPDOMAIN= <domain name>
    char cmd[64];
    at_master_t master = (at_master_t)dev->work_data;
    at_resp_t resp = at_create_resp(64, ESP8266_STD_TIMEOUT);
    if (NULL == resp)
        return 0;

    sprintf(cmd, "AT+CIPDOMAIN=\"%s\"", name);
    xSemaphoreTake(dtu_mutex, portMAX_DELAY);
    if (!at_exec_cmd(master, resp, cmd, strlen(cmd)))
        goto _exit_func;

    char ip[16];
    if (!at_parse_resp_by_kw(resp, "%*[^:]:%s", "+CIPDOMAIN", ip))
        goto _exit_func;

    ret = network_inet_addr(ip);

_exit_func:
    at_free_resp(resp);
    xSemaphoreGive(dtu_mutex);
    return ret;
}
