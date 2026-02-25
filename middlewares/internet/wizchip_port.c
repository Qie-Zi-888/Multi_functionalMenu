#include "wizchip_port.h"
#include "bsp_spi.h"
#include "bsp_delay.h"

#define WIZCHIP_CS_PORT GPIOC
#define WIZCHIP_CS_PINS GPIO_PINS_15

/* private functions *********************************************************/

static inline void wizchip_select(void)
{
    gpio_bits_reset(WIZCHIP_CS_PORT, WIZCHIP_CS_PINS);
}

static inline void wizchip_deselect(void)
{
    gpio_bits_set(WIZCHIP_CS_PORT, WIZCHIP_CS_PINS);
}

/**
 * @brief 重启 w5500 函数接口
 *
 */
static inline void wizchip_rst_set(void)
{
    gpio_bits_set(GPIOA, GPIO_PINS_15);
}

/**
 * @brief 重启 w5500 函数接口
 */
static inline void wizchip_rst_reset(void)
{
    gpio_bits_reset(GPIOA, GPIO_PINS_15);
}

/**
 * @brief w5500 读函数接口
 *
 * @return uint8_t 从 w5500 中读取的一个字节
 */
static uint8_t wizchip_read(void)
{
    while (spi_i2s_flag_get(SPI3, SPI_I2S_TDBE_FLAG) == RESET)
        ;
    spi_i2s_data_transmit(SPI3, 0xFF);
    while (spi_i2s_flag_get(SPI3, SPI_I2S_RDBF_FLAG) == RESET)
        ;
    return spi_i2s_data_receive(SPI3);
}

/**
 * @brief w5500 写函数接口
 *
 * @param c 向 w5500 写入的一个字节
 */
static void wizchip_write(uint8_t c)
{
    while (spi_i2s_flag_get(SPI3, SPI_I2S_TDBE_FLAG) == RESET)
        ;
    spi_i2s_data_transmit(SPI3, c);
    while (spi_i2s_flag_get(SPI3, SPI_I2S_RDBF_FLAG) == RESET)
        ;
    spi_i2s_data_receive(SPI3);
}

/**
 * @brief 进入临界区函数接口
 *
 */
static void wizchip_cirs_enter(void)
{
    portDISABLE_INTERRUPTS();
}

/**
 * @brief 退出临界区函数接口
 *
 */
static void wizchip_cirs_eixt(void)
{
    portENABLE_INTERRUPTS();
}

void wizchip_lowlevel_setup(void)
{
    gpio_init_type gpio_initstructure;
    exint_init_type exint_init_struct;

    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&gpio_initstructure);

    /* wiz w5500 cs pin (PA15) */
    gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
    gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_initstructure.gpio_pins = GPIO_PINS_15;
    gpio_init(GPIOA, &gpio_initstructure);
    gpio_bits_reset(GPIOA, GPIO_PINS_15);

    /* wiz w5500 cs pin (PC15) */
    gpio_init(GPIOC, &gpio_initstructure);
    gpio_bits_set(GPIOC, GPIO_PINS_15);

    /* wiz int pin (PA3) */
    crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);

    scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE3);

    exint_default_para_init(&exint_init_struct);
    exint_init_struct.line_enable = TRUE;
    exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
    exint_init_struct.line_select = EXINT_LINE_3;
    exint_init_struct.line_polarity = EXINT_TRIGGER_FALLING_EDGE;
    exint_init(&exint_init_struct);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(EXINT3_IRQn, 5, 0);

    /* init spi3 */
    spi3_init();
}

void wizchip_transmit_pbuf(struct pbuf *p)
{
    uint16_t freeSize = getSn_TX_FSR(0);
    uint16_t length = p->tot_len;

    if (freeSize < length)
    {
        /* TODO: Handle insufficent space in buffer */
    }
    while (1)
    {
        wiz_send_data(0, p->payload, p->len);
        if (p->len == p->tot_len)
            break;
        p = p->next;
    }
    setSn_CR(0, Sn_CR_SEND);
}

int wizchip_read_receive_pbuf(struct pbuf **buf)
{
    uint8_t header[6];
    uint16_t length, readlen;
    uint16_t framelen;
    struct pbuf *p;

    if (*buf != NULL)
        return 1;

    uint16_t rxRd = getSn_RX_RD(0);

    length = getSn_RX_RSR(0);
    if (length < 4)
    {
        /* This could be indicative of a crashed (brown-outed?) controller */
        goto end;
    }

    wiz_recv_data(0, (uint8_t *)&framelen, 2);
    setSn_CR(0, Sn_CR_RECV);
    //__bswap16(framelen); //!< didn't work for me
    framelen = (framelen << 8) | (framelen >> 8);

    /* workaround for https://savannah.nongnu.org/bugs/index.php?50040 */
    if (framelen > 32000)
    {
        wiz_recv_ignore(0, framelen);
        setSn_CR(0, Sn_CR_RECV);
        goto end;
    }

    framelen -= 2;

    *buf = pbuf_alloc(PBUF_RAW, (framelen), PBUF_RAM);

    if (*buf == NULL)
    {
        goto end;
    }

    wiz_recv_data(0, (*buf)->payload, framelen);
    setSn_CR(0, Sn_CR_RECV);

end:
    return (*buf == NULL) ? 2 : 0;
}

void wizchip_spi_readburst(uint8_t *pBuf, uint16_t len)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        pBuf[i] = wizchip_read();
    }
}

void wizchip_spi_writeburst(uint8_t *pBuf, uint16_t len)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        wizchip_write(pBuf[i]);
    }
}

void wizchip_reset(void)
{
    wizchip_rst_set();
    delay_ms(20);
    wizchip_rst_reset();
    delay_ms(20);
    wizchip_rst_set();
    delay_ms(20);
}

void wizchip_register(void)
{
    reg_wizchip_cris_cbfunc(wizchip_cirs_enter, wizchip_cirs_eixt); /* 注册进出临界区 */
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);        /* 注册 spi 片选 */
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);            /* 注册 spi 读写 */
}

void wizchip_initialize(void)
{
    /* 片选拉高 */
    wizchip_deselect();

    /* 注册进出临界区函数 */
    reg_wizchip_cris_cbfunc(wizchip_cirs_enter, wizchip_cirs_eixt);

    /* 注册片选函数 */
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);

    /* 注册 SPI 读写函数 */
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);

    uint8_t memsize[2][8] = {{16, 0, 0, 0, 0, 0, 0, 0}, {16, 0, 0, 0, 0, 0, 0, 0}};

    if (ctlwizchip(CW_INIT_WIZCHIP, (void *)memsize) == -1)
    {
        printf("W5500 initialized fail\r\n");
        return;
    }

    uint8_t temp;
    do
    {
        if (ctlwizchip(CW_GET_PHYLINK, (void *)&temp) == -1)
        {
            printf("Unknown PHY link status\r\n");
            return;
        }
    } while (temp == PHY_LINK_OFF);
}

void wizchip_check(void)
{
    if (getVERSIONR() != 0x04)
    {
        printf(" ACCESS ERR : VERSION != 0x04, read value = 0x%02x\r\n", getVERSIONR());
        return;
    }
    else {
        printf("VESRION = 0x04\r\n");
    }
}

void network_initialize(wiz_NetInfo net_info)
{
    ctlnetwork(CN_SET_NETINFO, (void *)&net_info);
}

/**
 * @brief 打印 W5500 配置
 * 
 * @param net_info 配置信息对象
 */
void print_network_information(wiz_NetInfo net_info)
{
    uint8_t tmp_str[8] = {
        0,
    };

    ctlnetwork(CN_GET_NETINFO, (void *)&net_info);
    ctlwizchip(CW_GET_ID, (void *)tmp_str);

    if (net_info.dhcp == NETINFO_DHCP)
    {
        printf("====================================================================================================\n");
        printf(" %s network configuration : DHCP\n\n", (char *)tmp_str);
    }
    else
    {
        printf("====================================================================================================\n");
        printf(" %s network configuration : static\n\n", (char *)tmp_str);
    }

    printf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf(" IP          : %d.%d.%d.%d\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf(" Subnet Mask : %d.%d.%d.%d\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf(" Gateway     : %d.%d.%d.%d\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf(" DNS         : %d.%d.%d.%d\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    printf("====================================================================================================\n\n");
}