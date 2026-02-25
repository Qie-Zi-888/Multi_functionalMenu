#include "network.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
/* enums *********************************************************************/

enum network_task_state_e
{
    NTASK_LINK_UP,
    NTASK_LINK_DOWN,
    NTASK_INTERNET_UP,
    NTASK_INTERNET_DOWN,
};

/* private variables *********************************************************/

TaskHandle_t network_handler = NULL;
static netdev_t s_netdev = NULL;
static network_socket_t s_network_socket = NULL;
static uint8_t s_network_status = NTASK_LINK_UP;

/* private functions *********************************************************/

static void _network_link_up_cb(void *param)
{ /* nothing to do. */
    printf("link up\r\n");
}

static void _network_link_down_cb(void *param)
{ /* nothing to do. */
    printf("link down\r\n");
}

static void _network_internet_up_cb(void *param)
{ /* nothing to do. */
    printf("internet up\r\n");
}

static void _network_internet_down_cb(void *param)
{ /* nothing to do. */
    printf("internet down\r\n");
}

static void _network_link_up_handle(netdev_t dev)
{
    uint32_t ping_time = 0;
    while (!netdev_is_flag(NETDEV_FLAG_LINK_UP))
        vTaskDelay(pdMS_TO_TICKS(500));

    /* 获取网卡信息 */
    netdev_getinfo(dev);
    netdev_link_up_cb(dev->callback.link_up_arg);

    if (netdev_is_flag(NETDEV_FLAG_LINK_UP))
    {
        /* 检查网络状态 */
        if (netdev_ping(dev, "www.baidu.com", &ping_time))
            s_network_status = NTASK_INTERNET_UP;
        else
            s_network_status = NTASK_INTERNET_DOWN;
    }
    else
    {
        s_network_status = NTASK_LINK_DOWN;
    }
}

static void _network_internet_up_handle(netdev_t dev)
{
    uint32_t ping_time = 0;
    uint8_t cnt = 50;
    netdev_set_flag(NETDEV_FLAG_INTERNET_UP);
    netdev_internet_up_cb(dev->callback.internet_up_arg);

    while (1)
    {
        if (netdev_is_flag(NETDEV_FLAG_LINK_UP))
        {
            if (cnt >= 50)
            {
                cnt = 0;
                if (!netdev_ping(dev, "www.baidu.com", &ping_time))
                { /* 网络不可用 */
                    s_network_status = NTASK_INTERNET_DOWN;
                    break;
                }
            }
        }
        else
        { /* link down */
            s_network_status = NTASK_LINK_DOWN;
            break;
        }
        cnt++;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void _network_internet_down_handle(netdev_t dev)
{
    uint32_t ping_time = 0;
    uint8_t cnt = 50;
    netdev_clear_flag(NETDEV_FLAG_INTERNET_UP);
    netdev_internet_down_cb(dev->callback.internet_down_arg);
    while (1)
    {
        if (netdev_is_flag(NETDEV_FLAG_LINK_UP))
        {
            if (cnt >= 50) /* 5s */
            {
                cnt = 0;
                if (netdev_ping(dev, "www.baidu.com", &ping_time))
                {
                    s_network_status = NTASK_INTERNET_UP;
                    break;
                }
            }
        }
        else
        { /* link down */
            s_network_status = NTASK_LINK_DOWN;
            break;
        }
        cnt++;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* public functions **********************************************************/

/**
 * @brief 网络线程，用于维护网络状态
 *
 * @param pvParameters NULL
 */
void network_task_functoin(void *pvParameters)
{
    netdev_t dev = s_netdev;
    if (dev == NULL)
    {
        printf("no netdev registered\r\n");
        return;
    }

    netdev_setup(dev);

    while (!netdev_is_flag(NETDEV_FLAG_UP))
    {
        vTaskDelay(5000);
    }
    printf("netdev set up\r\n");

    uint32_t ping_time = 0;
    while (1)
    {
        switch (s_network_status)
        {
        case NTASK_LINK_UP: /* link up state */
            _network_link_up_handle(dev);
            break;
        case NTASK_LINK_DOWN: /* link down state */
            netdev_internet_down_cb(dev->callback.internet_down_arg);
            netdev_link_down_cb(dev->callback.link_down_arg);
            netdev_clear_flag(NETDEV_FLAG_LINK_UP);
            s_network_status = NTASK_LINK_UP;
            break;
        case NTASK_INTERNET_UP: /* internet up state */
            _network_internet_up_handle(dev);
            break;
        case NTASK_INTERNET_DOWN: /* internet down state */
            _network_internet_down_handle(dev);
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void network_notice_link_up(netdev_t dev)
{
    netdev_set_flag(NETDEV_FLAG_LINK_UP);
}

void network_notice_link_down(netdev_t dev)
{
    dev->flags = NETDEV_FLAG_UP;
}

void network_netdev_cb_param_init(struct netdev *dev)
{
    dev->callback.link_up_arg = NULL;
    dev->callback.link_up = _network_link_up_cb;

    dev->callback.link_down_arg = NULL;
    dev->callback.link_down = _network_link_down_cb;

    dev->callback.internet_up_arg = NULL;
    dev->callback.internet_up = _network_internet_up_cb;

    dev->callback.internet_down_arg = NULL;
    dev->callback.internet_down = _network_internet_down_cb;
}

netdev_t network_get_netdev(void)
{
    return s_netdev;
}

uint8_t network_register_netdev(struct netdev *dev)
{
    if (NULL == dev)
        return 0;

    dev->ops = NULL;
    dev->work_data = NULL;
    network_netdev_cb_param_init(dev);
    s_netdev = dev;

    return 1;
}

uint8_t network_set_netdev_status_cb(netdev_t dev, uint8_t cb_type, void (*callback)(void *param), void *param)
{
    uint8_t ret = 1;
    switch (cb_type)
    {
    case NETWORK_LINK_UP_CB:
        dev->callback.link_up = callback;
        dev->callback.link_up_arg = param;
        break;
    case NETWORK_LINK_DOWN_CB:
        dev->callback.link_down = callback;
        dev->callback.link_down_arg = param;
        break;
    case NETWORK_INTERNET_UP_CB:
        dev->callback.internet_up = callback;
        dev->callback.internet_up_arg = param;
        break;
    case NETWORK_INTERNET_DOWN_CB:
        dev->callback.internet_down = callback;
        dev->callback.internet_down_arg = param;
        break;
    default:
        ret = 0;
        printf("Not have this %d callback type\r\n", cb_type);
        break;
    }
    return ret;
}

uint8_t network_get_ntptime(time_t *timestamp)
{
    netdev_t dev = network_get_netdev();
    if (!netdev_is_flag(NETDEV_FLAG_INTERNET_UP))
        return 0;

    int sockfd = network_socket(AF_INET, SOCK_SDGRAM, 0); /* udp */
    if (-1 == sockfd)
    {
        printf("create socket error!\r\n");
        return 0;
    }

    network_set_socket_timeout(sockfd, 3000);

    struct network_sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = network_htons(NTP_ALIYUN_PORT);
    serv.sin_addr.S_un.S_addr = network_inet_addr(NTP_ALIYUN_HOST);
    size_t servlen = sizeof(serv);

    /* ntp udp 报文定义 */
    char *ntp_udp_msg = (char *)pvPortMalloc(48);
    memset(ntp_udp_msg, 0x00, sizeof(ntp_udp_msg));
    ntp_udp_msg[0] = 0x1B;

    if (-1 == network_sendto(sockfd, ntp_udp_msg, 48, 0,
                             (struct network_sockaddr *)&serv, servlen))
    {
        printf("send dtat error\r\n");
        network_closesocket(sockfd);
        vPortFree(ntp_udp_msg);
        return 0;
    }
    vPortFree(ntp_udp_msg);

    int n = -1;
    char *buf = (char *)pvPortMalloc(128);
    n = network_recvfrom(sockfd, buf, 128, 0,
                         (struct network_sockaddr *)&serv, &servlen);

    network_closesocket(sockfd);

    if (n > 0)
    {
        uint32_t *receive_time = (uint32_t *)&buf[40]; // 从响应包中获取Receive Timestamp字段
        time_t ntp_timestamp = network_ntohl(*receive_time) - 2208988800;

        *timestamp = ntp_timestamp + NETWORK_NTP_TIMEZONE * 3600;
    }
    vPortFree(buf);

    return 1;
}

void network_timestamp_to_tm(time_t *timestamp, struct tm *time)
{
    struct tm *time_info = localtime(timestamp);
    time_info->tm_year += 1900;
    time_info->tm_mon += 1;
    memcpy((void *)time, (const void *)time_info, sizeof(struct tm));
}

/* lowlevel socket ***********************************************************/

/**
 * @brief 创建 socket
 *
 * @param socket 返回的 socket 对象
 * @return flase:-1, ture: >= 0 (sockfd)
 */
int network_lowlevel_create_socket(network_socket_t *socket)
{
    *socket =
        (network_socket_t)network_malloc(sizeof(struct network_socket));
    (*socket)->buf = (char *)network_malloc(2049);
    if ((*socket)->buf == NULL)
        printf("socket buff create fail\r\n");

    vPortEnterCritical();
    (*socket)->head = 0;
    (*socket)->tail = 0;
    (*socket)->capacity = 2049;
    (*socket)->status = SOCK_STATUS_INIT;
    (*socket)->timeout = portMAX_DELAY;
    vPortExitCritical();

    (*socket)->notice = xSemaphoreCreateBinary();
    if ((*socket)->notice == NULL)
        printf("create socket notice sem fail\r\n");

    if (*socket == NULL)
        return -1;

    (*socket)->next = NULL;

    if (s_network_socket == NULL || s_network_socket->sockfd != 0)
    {
        (*socket)->sockfd = 0;
        (*socket)->next = s_network_socket;
        s_network_socket = (*socket);
        return 0;
    }
    else
    {
        network_socket_t p = s_network_socket;
        int sockfd = 1;

        while (p->next && p->next->sockfd == sockfd)
        {
            p = p->next;
            sockfd++;
        }

        (*socket)->sockfd = sockfd;
        (*socket)->next = p->next;
        p->next = (*socket);
        return sockfd;
    }
}

uint8_t network_lowlevel_socket_write_byte(network_socket_t socket, const char ch)
{
    if ((socket->tail + 1) % socket->capacity == socket->head) /* 数据队列满 */
    {
        printf("socket buff is full\r\n");
        return 0;
    }
    socket->tail = (socket->tail + 1) % socket->capacity;
    socket->buf[socket->tail] = ch;
    return 1;
}

uint8_t network_lowlevel_socket_read_byte(network_socket_t socket, char *ch)
{
    if (socket->head == socket->tail) /* 数据队列空 */
        return 0;
    socket->head = (socket->head + 1) % socket->capacity;
    *ch = socket->buf[socket->head];
    return 1;
}

network_socket_t network_lowlevel_get_socket(int sockfd)
{
    network_socket_t socket = s_network_socket;
    while (socket && socket->sockfd != sockfd)
    {
        socket = socket->next;
    }
    return socket;
}

void network_lowlevel_closesocket(int sockfd)
{
    network_socket_t socket = network_lowlevel_get_socket(sockfd);
    if (NULL != socket)
    {
        socket->status = SOCK_STATUS_DISCONN;
        xSemaphoreGive(socket->notice);
    }
}

uint8_t network_lowlevel_delete_socket(int sockfd)
{
    if (NULL == s_network_socket)
        return 0;

    network_socket_t prev = NULL;
    network_socket_t current = s_network_socket;

    while (current)
    {
        if (current->sockfd == sockfd)
        {
            if (prev)
                prev->next = current->next;
            else
                s_network_socket = current->next;

            network_free(current->buf);
            vSemaphoreDelete(current->notice);
            network_free(current);
            return 1;
        }

        prev = current;
        current = current->next;
    }

    return 0;
}

/* network socket ************************************************************/

/**
 * @brief 将主机字节序（小端模式）的 16 位整数转换为网络字节序
 *
 * @param hostshort 主机字节序 16 位整数
 * @return uint16_t 网络字节序 16 位整数
 */
uint16_t network_htons(uint16_t hostshort)
{
    return ((hostshort & 0xFF00) >> 8) | ((hostshort & 0x00FF) << 8);
}

/**
 * @brief 将网络字节序的 16 位整数转换为主机字节序（小端模式）
 *
 * @param netshort 网络字节序 16 位整数
 * @return uint16_t 主机字节序 16 位整数
 */
uint16_t network_ntohs(uint16_t netshort)
{
    return network_htons(netshort); // 在小端主机字节序下，ntohs 与 htons 等效
}

/**
 * @brief 将主机字节序（小端模式）的 32 位整数转换为网络字节序
 *
 * @param hostlong 主机字节序 32 位整数
 * @return uint32_t 网络字节序 32 位整数
 */
uint32_t network_htonl(uint32_t hostlong)
{
    return ((hostlong & 0xFF000000) >> 24) | ((hostlong & 0x00FF0000) >> 8) |
           ((hostlong & 0x0000FF00) << 8) | ((hostlong & 0x000000FF) << 24);
}

/**
 * @brief 将网络字节序的 32 位整数转换为主机字节序（小端模式）
 *
 * @param netlong 网络字节序 32 位整数
 * @return uint32_t 主机字节序 32 位整数
 */
uint32_t network_ntohl(uint32_t netlong)
{
    return network_htonl(netlong); // 在小端主机字节序下，ntohl 与 htonl 等效
}

/**
 * @brief 将点分十进制转换为网络字节序 32 位地址
 *
 * @param addr "xxx.xxx.xxx.xxx"
 * @return uint32_t 网络字节序 32 位地址
 */
uint32_t network_inet_addr(const char *addr)
{
    struct network_in_addr tmp;
    int ret = sscanf(addr, "%hhu.%hhu.%hhu.%hhu",
                     &tmp.S_un.S_un_b.s_b4,
                     &tmp.S_un.S_un_b.s_b3,
                     &tmp.S_un.S_un_b.s_b2,
                     &tmp.S_un.S_un_b.s_b1);
    return (ret != 4) ? 0 : tmp.S_un.S_addr;
}

/**
 * @brief 将网络字节序 32 位地址转换为点分十进制
 *
 * @param addr 网络字节序 32 位整数
 * @return char* "xxx.xxx.xxx.xxx"
 */
char *network_inet_ntoa(uint32_t addr)
{
    char *ip_str = (char *)network_malloc(16);
    if (ip_str != NULL)
    {
        sprintf(ip_str, "%hhu.%hhu.%hhu.%hhu",
                (uint8_t)((addr >> 24) & 0xFF),
                (uint8_t)((addr >> 16) & 0xFF),
                (uint8_t)((addr >> 8) & 0xFF),
                (uint8_t)(addr & 0xFF));
    }
    return ip_str;
}

/**
 * @brief 通过域名获取网络字节序 32 位 ip 地址
 *
 * @param name 域名
 * @return uint32_t 网络字节序 32 位 ip 地址, 失败则返回 0
 */
uint32_t network_gethostbyname(const char *name)
{
    return s_netdev->sockops->gethostbyname(s_netdev, name);
}

int network_socket(int domain, int type, int protocol)
{
    return s_netdev->sockops->socket(s_netdev, domain, type, protocol);
}

int network_connect(int sockfd, const struct network_sockaddr *serv_addr, int addrlen)
{
    return s_netdev->sockops->connect(s_netdev, sockfd, serv_addr, addrlen);
}

int network_recv(int sockfd, void *buf, int len, int flags)
{
    return s_netdev->sockops->recv(s_netdev, sockfd, buf, len, flags);
}

int network_send(int sockfd, const void *buf, size_t len, int flags)
{
    return s_netdev->sockops->send(s_netdev, sockfd, buf, len, flags);
}

int network_recvfrom(int sockfd, void *buf, int len, int flags, struct network_sockaddr *from, size_t *fromlen)
{
    return s_netdev->sockops->recvfrom(s_netdev, sockfd, buf, len, flags, from, fromlen);
}

int network_sendto(int sockfd, const void *msg, size_t len, int flags, const struct network_sockaddr *to, size_t tolen)
{
    return s_netdev->sockops->sendto(s_netdev, sockfd, msg, len, flags, to, tolen);
}

int network_closesocket(int sockfd)
{
    network_socket_t socket = NULL;

    // vTaskDelay(5);

    /* socket 对象是否删除 */
    socket = network_lowlevel_get_socket(sockfd);
    if (socket == NULL)
        return -1;

    if (socket->status == SOCK_STATUS_CONN) /* 仍然在连接状态 */
    {
        if (-1 == s_netdev->sockops->closesocket(s_netdev, sockfd))
            return -1;
    }

    network_lowlevel_delete_socket(sockfd);
    return 0;
}

int network_socket_list_num(void)
{
    int count = 0;
    network_socket_t p = s_network_socket;
    while (p)
    {
        count++;
        p = p->next;
    }
    return count;
}

uint8_t network_set_socket_timeout(int sockfd, uint32_t timeout)
{
    network_socket_t socket = network_lowlevel_get_socket(sockfd);
    if (NULL == socket)
        return 0;

    socket->timeout = pdMS_TO_TICKS(timeout);
    return 0;
}
