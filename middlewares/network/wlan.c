#include "network.h"
#include <string.h>
#include <stdio.h>

#define wlan_connect_wifi (dev->ops->connect)

static wlan_ap_list_t s_wlan_ap_list = NULL;

wlan_dev_t wlan_create(const char *name)
{
    wlan_dev_t wlan = (wlan_dev_t)network_malloc(sizeof(struct wlan_dev));
    if (NULL == wlan)
        return NULL;

    wlan->ssid[0] = '\0';
    wlan->pwd[0] = '\0';
    strncpy(wlan->parent.name, name, NETDEV_NAME_LEN);

    if (network_register_netdev(&(wlan->parent)))
        return wlan;

    network_free(wlan);
    return NULL;
}

uint8_t wlan_connect(wlan_dev_t dev, const char *ssid, const char *pwd)
{
    strncpy(dev->ssid, ssid, WLAN_SSID_LEN);
    strncpy(dev->pwd, pwd, WLAN_PWD_LEN);
    
    netdev_clear_flag(NETDEV_FLAG_LINK_UP);
    if (wlan_connect_wifi(dev))
    {
        // netdev_set_flag(NETDEV_FLAG_LINK_UP); /* 由 urc 模式接管 */
        return 1;
    }
    return 0;
}

uint8_t wlan_disconnect(wlan_dev_t dev)
{
    return dev->ops->disconnect(dev);
}

uint8_t wlan_scan_ap(wlan_dev_t dev)
{
    wlan_delete_ap_list();
    return dev->ops->scan(dev);
}

uint8_t wlan_add_ap_to_list(wlan_ap_list_t wlan_ap)
{
    wlan_ap->next = s_wlan_ap_list;
    s_wlan_ap_list = wlan_ap;
    return 1;
}

void wlan_iter_ap_list(void (*func)(wlan_ap_list_t _ap))
{
    wlan_ap_list_t p = s_wlan_ap_list;
    while (p)
    {
        func(p);
        p = p->next;
    }
}

void wlan_delete_ap_list(void)
{
    wlan_ap_list_t p = s_wlan_ap_list;
    wlan_ap_list_t q = NULL;
    while (p)
    {
        q = p;
        p = p->next;
        vPortFree(q);
    }
    s_wlan_ap_list = NULL;
}
