#include "at.h"
#include "at32f435_437_board.h"
#include <string.h>
#include <stdarg.h>

/* private variables *********************************************************/

static at_urc_t s_urc = NULL;

/* private functions *********************************************************/

/**
 * @brief 发送 AT 命令
 *
 * @param master AT 主机对象
 * @param resp AT 响应对象
 * @param cmd AT 命令
 * @param len AT 命令长度
 * @param work_model AT 解析命令的工作模式
 * @return uint8_t 0: flase, 1: true
 */
static uint8_t _at_exec_cmd_with_model(
    at_master_t master, at_resp_t resp,
    const char *cmd, int len, uint8_t work_model)
{
    uint8_t ret = 0;
    xSemaphoreTake(master->lock, portMAX_DELAY);

    if (NULL != resp)
    {
        ret = 1;
        resp->line_count = 0;
    }

    master->resp = resp;
    master->work_model = work_model;

    serial_send(master->serial, (char *)cmd, len);
    serial_send(master->serial, "\r\n", 2);

    if (NULL != resp)
    {
        // 等待超时
        if (pdFALSE == xSemaphoreTake(master->notice, resp->timeout))
        {
            printf("[Timeout]: %s\r\n", cmd);
            master->resp_status = AT_RESP_TIMEOUT;
            ret = 0;
            goto _exit_func;
        }
        else
        {
            if (master->resp_status != AT_RESP_OK)
            {
                ret = 0;
                goto _exit_func;
            }
        }
    }

_exit_func:
    master->resp = NULL;
    master->work_model = AT_MODEL_URC;
    xSemaphoreGive(master->lock);
    return ret;
}

/**
 * @brief AT 解析线程入口函数
 *
 * @param pvParameter 输入大参数
 */
static void at_master_parser(void *pvParameter)
{
    char buf[1500] = {0};
    at_master_t master = (at_master_t)pvParameter;
    uint16_t ind = 0;
    while (1)
    {
        memset(buf, 0, 1500);
        at_recv_line(master, buf, 2048, portMAX_DELAY);
        if (s_urc == NULL)
        {
            switch (master->work_model)
            {
            case AT_MODEL_KW_CMD:
                // printf("[kw] %s\r\n", buf);
                strncpy(master->resp->buf + ind, buf, master->resp->bufsiz + 1 - ind);
                ind += strlen(buf) + 1;
                master->resp->line_count++;
                if (strstr(buf, "OK"))
                {
                    ind = 0;
                    master->resp_status = AT_RESP_OK;
                    xSemaphoreGive(master->notice);
                }
                else if (strstr(buf, "ERROR") || strstr(buf, "FAIL"))
                {
                    ind = 0;
                    master->resp_status = AT_RESP_ERROR;
                    xSemaphoreGive(master->notice);
                }
                break;
            case AT_MODEL_CMP_CMD:
                // printf("[cmp] %s\r\n", buf);
                if (strstr(buf, master->resp->buf))
                {
                    master->resp_status = AT_RESP_OK;
                    xSemaphoreGive(master->notice);
                }
                else if (strstr(buf, "ERROR") || strstr(buf, "FAIL"))
                {
                    master->resp_status = AT_RESP_ERROR;
                    xSemaphoreGive(master->notice);
                }
                break;
            }
        }
        else
        {
            // printf("urc] %s\r\n", buf);
            s_urc->func(master, buf);
        }
    }
}

/* public functions **********************************************************/

/**
 * @brief 创建 resp 对象（动态申请内存）
 *
 * @param bufsiz resp 缓冲区大小
 * @param timeout 超时时间（ms）
 * @return at_resp_t 内存申请不成功时，返回 NULL，否则返回 resp 对象
 */
at_resp_t at_create_resp(uint16_t bufsiz, uint32_t timeout)
{
    at_resp_t resp = (at_resp_t)pvPortMalloc(sizeof(struct at_resp));
    if (NULL == resp)
        return NULL;

    resp->buf = (char *)pvPortMalloc(bufsiz);
    if (NULL == resp->buf)
    {
        vPortFree(resp);
        return NULL;
    }
    memset(resp->buf, 0, bufsiz);

    resp->line_count = 0;
    resp->bufsiz = bufsiz;
    resp->timeout = pdMS_TO_TICKS(timeout);

    return resp;
}

/**
 * @brief 释放响应对象所申请的内存
 *
 * @param resp 需要释放内存的响应对象
 */
void at_free_resp(at_resp_t resp)
{
    if (NULL == resp)
        return;

    vPortFree(resp->buf);
    vPortFree(resp);
    resp = NULL;
}

/**
 * @brief
 *
 * @param master 发送 AT 命令
 * @param resp AT 响应对象
 * @param cmd AT 命令
 * @param len AT 长度
 * @return uint8_t 1: 成功，0：失败
 */
uint8_t at_exec_cmd(at_master_t master, at_resp_t resp, const char *cmd, int len)
{
    return _at_exec_cmd_with_model(master, resp, cmd, len, AT_MODEL_KW_CMD);
}

/**
 * @brief 发送 AT 命令，并等待响应
 *
 * @param at_master AT 主机对象
 * @param expr 等待的响应字符串
 * @param cmd AT 命令
 * @param len AT 命令
 * @param timeout 超时时间
 * @return uint8_t 1: 成功，0：失败
 */
uint8_t at_exec_cmd_wait_expr(at_master_t at_master,
                              const char *expr,
                              const char *cmd, int len, uint32_t timeout)
{
    uint16_t len2 = strlen(expr) + 1;
    at_resp_t resp = at_create_resp(len2, timeout);
    // printf("at cmd : %s\r\n", cmd);

    vPortEnterCritical();
    strcpy(resp->buf, expr);
    vPortExitCritical();

    uint8_t ret = _at_exec_cmd_with_model(at_master,
                                          resp, cmd, len,
                                          AT_MODEL_CMP_CMD);
    at_free_resp(resp);
    return ret;
}

/**
 * @brief 通过比较 line 字符串，从 urc table 中获取 urc 对象
 *
 * @param master AT master 对象
 * @param line 接收到的字符串
 * @return at_urc_t flase: NULL, ture: 匹配到的 urc 对象
 */
at_urc_t at_get_urc_obj(at_master_t master, const char *line)
{
    uint16_t prefix_len = 0;
    uint16_t suffix_len = 0;
    at_urc_t urc = NULL;
    uint16_t bufsz = strlen(line);
    for (int i = 0; i < master->urc_table.szie; i++)
    {
        urc = master->urc_table.urc + i;

        prefix_len = strlen(urc->prefix);
        suffix_len = strlen(urc->suffix);

        if (bufsz < (prefix_len + suffix_len))
            continue;

        if ((prefix_len ? !strncmp(line, urc->prefix, prefix_len) : 1) &&
            (suffix_len ? !strncmp(line + bufsz - suffix_len, urc->suffix, suffix_len) : 1))
            return urc;
    }
    return NULL;
}

/**
 * @brief 从 AT 主机所属的串口中读取一行数据（阻塞）
 *
 * @param master AT 主机
 * @param line 存放数据的缓冲区
 * @param capacity 存放数据的缓冲区的大小
 * @param timeout 超时时间 (unit:os tick)
 * @return uint8_t 0: 容量为0 或 超时，1：成功
 */
uint8_t at_recv_line(at_master_t master, char *line, int capacity, uint32_t timeout)
{
    BaseType_t ret = pdFALSE;
    uint8_t ch = 0;
    uint8_t last_ch = 0;
    uint16_t i = 0;

    if (capacity == 0)
    {
        printf("%s buff is not enought.\r\n", __FUNCTION__);
        return 0;
    }

    while (1)
    {
        ret = xSemaphoreTake(master->serial->rx_sem, timeout);
        if (pdFALSE == ret)
        {
            printf("sem fail\r\n");
            return 0;
        }
        if (serial_recv_byte(master->serial, &ch))
        {
            line[i++] = ch;

            if (i > capacity)
                return 1;

            /* 去除字符串头部的 \r\n */
            if (i == 2 && line[0] == '\r' && line[1] == '\n')
            {
                i = 0;
                line[0] = '\0';
                line[1] = '\0';
            }

            /* 去除字符串尾部的 \r\n */
            if (i > 2 && line[i - 2] == '\r' && line[1] == '\n')
            {
                i = i - 2;
                line[i - 2] = '\0';
                line[i - 1] = '\0';
            }

            if ((s_urc = at_get_urc_obj(master, line)) != NULL ||(i > 2 && (ch == '\n' && last_ch == '\r')))
                return 1;
        }
        last_ch = ch;
    }
    return 0;
}

/**
 * @brief 从响应对象中返回具有输入的关键字的那一行
 *
 * @param resp 响应对象
 * @param kw 关键字
 * @return char* 失败时，返回 NULL
 */
char *at_get_resp_line_by_kw(at_resp_t resp, const char *kw)
{
    int i = 0, ind = 0;
    for (i = 0; i < resp->line_count; i++)
    {
        if (strstr(resp->buf + ind, kw))
            return resp->buf + ind;

        ind += strlen(resp->buf + ind) + 1;
    }
    return NULL;
}

/**
 * @brief 通过输入的格式解析字符串
 *
 * @param str 字符串
 * @param fmt 解析格式
 * @param ... 解析后数据存放位置（指针）
 * @return uint8_t 0: flase, 1: true
 */
uint8_t at_parse_str(const char *str, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    uint8_t ret = vsscanf(str, fmt, args);
    va_end(args);
    return (ret == -1) ? 0 : ret;
}

/* 在这里无法使用，存在 bug 导致解析的结果异常（“”） */
// uint8_t at_parse_resp_by_kw(at_resp_t resp, const char *expr, const char *kw, ...)

/**
 * @brief 解析数据（一个）
 *
 * @param resp 响应对象
 * @param expr 解析格式
 * @param kw 关键字
 * @param dist 解析后数据存放的位置
 * @return uint8_t 0: flase, 1: true
 */
uint8_t at_parse_resp_by_kw(at_resp_t resp, const char *expr,
                            const char *kw, const char *dist)
{
    // va_list args;
    char *line = NULL;
    if (NULL == resp)
        return 0;

    line = at_get_resp_line_by_kw(resp, kw);
    if (NULL == line)
        return 0;

    // va_start(args, expr);
    // uint8_t ret = vsscanf((const char *)line, expr, args);
    // va_end(args);
    uint8_t ret = sscanf(line, expr, dist);

    return (ret == -1) ? 0 : ret;
}

/**
 * @brief AT 主机对象参数初始化（进行该初始化前，需要先设置 serial 对象）
 *
 * @param master
 * @return uint8_t
 */
uint8_t at_master_param_init(at_master_t master)
{
    uint8_t ret = 0;
    master->work_model = AT_MODEL_URC;

    master->lock = xSemaphoreCreateMutex();
    if (NULL == master->lock)
        goto _exit_func;

    master->notice = xSemaphoreCreateBinary();
    if (NULL == master->notice)
        goto _exit_func;

    taskENTER_CRITICAL();

    xTaskCreate((TaskFunction_t)at_master_parser,
                (const char *)"at_parser_task",
                (uint16_t)3072,
                (void *)master,
                (UBaseType_t)2,
                (TaskHandle_t *)(master->parser));

    taskEXIT_CRITICAL();

    ret = 1;
_exit_func:
    if (ret == 0)
    {
        if (master->lock)
            vSemaphoreDelete(master->lock);

        if (master->notice)
            vSemaphoreDelete(master->notice);
    }
    return ret;
}
