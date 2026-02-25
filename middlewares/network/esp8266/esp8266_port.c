#include "esp8266_port.h"
#include "esp8266.h"
#include "at.h"
#include <stdio.h>
#include "network.h"
#include "bsp_uart.h"
#include "drv_uart.h"
#include <string.h>
#include <stdlib.h>

static wlan_dev_t s_esp_dev = NULL;
static struct at32_uart *s_esp_serial = NULL;
static struct at_master s_esp_at_master;

/* defines private functions *************************************************/

static void _esp8266_wifi_disconnect_cb(at_master_t master, void *param);
static void _esp8266_wifi_connect_cb(at_master_t master, void *param);
static void _esp8266_socket_recv_cb(at_master_t master, void *param);
static void _esp8266_closesocket_cb(at_master_t master, void *param);
static void _esp8266_scan_ap_cb(at_master_t master, void *param);

/* private variables *********************************************************/

static struct at_urc urc_table[] = {
    {"WIFI DISCONNECT", "", _esp8266_wifi_disconnect_cb},
    {"WIFI GOT IP", "", _esp8266_wifi_connect_cb},
    {"+CWLAP:", "", _esp8266_scan_ap_cb},
    {"+IPD", ":", _esp8266_socket_recv_cb},
    {"", ",CLOSED", _esp8266_closesocket_cb},
};

static struct netdev_ops esp_netdev_ops = {
    .ping = esp8266_ping,
    .set_up = esp8266_set_up,
    .set_dhcp = esp8266_set_dhcp,
    .get_netinfo = esp8266_get_netdev_info,
    .set_addr_info = esp8266_set_addr_info,
    .set_dns_server = esp8266_set_dns_server,
};

static struct wlan_ops esp_wlan_ops = {
    .scan = esp8266_scan,
    .connect = esp8266_join_ap,
    .disconnect = esp8266_disconnect_ap,
};

static struct network_socket_ops esp_sock_ops = {
    .send = esp8266_send,
    .recv = esp8266_recv,
    .socket = esp8266_socket,
    .sendto = esp8266_sendto,
    .connect = esp8266_connect,
    .recvfrom = esp8266_recvfrom,
    .closesocket = esp8266_closesocket,
    .gethostbyname = esp8266_gethostbyname,
};

/* private functions *********************************************************/

void _esp8266_wifi_disconnect_cb(at_master_t master, void *param)
{
    network_notice_link_down((netdev_t)s_esp_dev);
}

void _esp8266_wifi_connect_cb(at_master_t master, void *param)
{
    network_notice_link_up((netdev_t)s_esp_dev);
}

void _esp8266_socket_recv_cb(at_master_t master, void *param)
{
    int sockfd = -1;
    int data_len = -1;
    sscanf((const char *)param, "+IPD,%d,%d:", &sockfd, &data_len);
    network_socket_t socket = network_lowlevel_get_socket(sockfd);
    if (socket == NULL)
        printf("socket is not exists!\r\n");

    uint8_t ch;
    for (int i = 0; i < data_len; i++)
    {
        xSemaphoreTake(master->serial->rx_sem, portMAX_DELAY);
        serial_recv_byte(master->serial, &ch);
        network_lowlevel_socket_write_byte(socket, ch);
    }
    xSemaphoreGive(socket->notice);
}

void _esp8266_closesocket_cb(at_master_t master, void *param)
{
    int sockfd = -1;
    sscanf((const char *)param, "%d,CLOSED", &sockfd);

    network_lowlevel_closesocket(sockfd);
}

void _esp8266_scan_ap_cb(at_master_t master, void *param)
{ /* ssid 不能存在 ) 右括号 */
    uint8_t ch = 0;
    wlan_ap_list_t temp_wifi;
    int i = 0;
    char buf[256] = {0};
    while (1)
    {
        xSemaphoreTake(master->serial->rx_sem, portMAX_DELAY);
        serial_recv_byte(master->serial, &ch);
        buf[i++] = ch;
        if (ch == ')')
            break;
    }
    buf[i] = '\0';

    temp_wifi = (wlan_ap_list_t)pvPortMalloc(sizeof(struct wlan_ap_list));
    memset(temp_wifi, 0, sizeof(struct wlan_ap_list));

    sscanf(buf, "(%d,\"%32[^\"]\",%d,\"%19[^\"]\",%d,%*d,%*d,%*d,%*d,%*d,%*d)",
        &temp_wifi->ecn, &temp_wifi->ssid, &temp_wifi->rssi, temp_wifi->mac, &temp_wifi->ch);

    if (strlen(temp_wifi->ssid) > 0)
        wlan_add_ap_to_list(temp_wifi);
    else
        vPortFree(temp_wifi);
}

/* public functions **********************************************************/

/**
 * @brief 初始化 esp8266，并创建 at 解析线程
 *
 */
void esp8266_init(void)
{
    s_esp_serial = serial_create(USART3, ESP8266_TX_BUFSIZ, ESP8266_RX_BUFSIZ);

   // uart3_init(115200);
	bsp_InitESP12();
	
    s_esp_at_master.serial = s_esp_serial;

    s_esp_dev = wlan_create(ESP_NETDEV_NAME);
    if (s_esp_dev)
    {
        s_esp_dev->parent.ops = &esp_netdev_ops;
        s_esp_dev->parent.sockops = &esp_sock_ops;
        s_esp_dev->parent.work_data = (void *)&s_esp_at_master;

        s_esp_dev->ops = &esp_wlan_ops;

        s_esp_at_master.urc_table.urc = urc_table;
        s_esp_at_master.urc_table.szie = sizeof(urc_table) / sizeof(urc_table[0]);

        /* 初始化 AT 主机参数, 创建解析线程 */
        at_master_param_init(&s_esp_at_master);
    }
}
#ifdef _BSP_USART_H_
#define _BSP_ESP_UART_IRQ_ 0
#elif  _BSP_ESP_UART_IRQ_ 1 
#endif

#if _BSP_ESP_UART_IRQ_
void USART3_IRQHandler(void)
{
#if ESP12_DEBUG
    if (USART3->ctrl1_bit.rdbfien != RESET)
    {
        if (usart_flag_get(USART3, USART_RDBF_FLAG) != RESET)
        {
            usart_data_transmit(PRINT_UART, usart_data_receive(USART3));
        }
    }

#else
    serial_irq(s_esp_serial);
#endif
}
#endif