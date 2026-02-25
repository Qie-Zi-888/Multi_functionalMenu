#ifndef AT_H
#define AT_H

/* includes ******************************************************************/

#include "at32f435_437.h"
#include "drv_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* defines *******************************************************************/

enum at_resp_e
{
    AT_RESP_OK,
    AT_RESP_ERROR,
    AT_RESP_TIMEOUT
};

enum at_work_model_e
{
    AT_MODEL_URC,
    AT_MODEL_KW_CMD,
    AT_MODEL_CMP_CMD,
};


/* structures ****************************************************************/

typedef struct at_resp *at_resp_t;
typedef struct at_urc *at_urc_t;
typedef struct at_master *at_master_t;
typedef struct at_urc_table* at_urc_table_t;

struct at_resp
{
    uint16_t line_count;
    uint16_t bufsiz;
    uint32_t timeout;
    char *buf;
};

struct at_urc
{
    char *prefix;
    char *suffix;
    void (*func)(at_master_t master, void *param);
};


struct at_urc_table
{
    uint16_t szie;
    struct at_urc *urc;
};

struct at_master
{
    uint8_t work_model;
    uint8_t resp_status;
    TaskHandle_t parser;
    at_resp_t resp;
    serial_t serial;
    SemaphoreHandle_t lock;
    SemaphoreHandle_t notice;
    struct at_urc_table urc_table;
};

/* functions *****************************************************************/

uint8_t at_master_param_init(at_master_t master);

at_resp_t at_create_resp(uint16_t bufsiz, uint32_t timeout);
void at_free_resp(at_resp_t resp);

at_urc_t at_get_urc_obj(at_master_t master, const char *line);
char *at_get_resp_line_by_kw(at_resp_t resp, const char *kw);
uint8_t at_parse_str(const char *str, const char *fmt, ...);
uint8_t at_recv_line(at_master_t master, char *line, int capacity, uint32_t timeout);
uint8_t at_exec_cmd(at_master_t master, at_resp_t resp, const char *cmd, int len);
uint8_t at_parse_resp_by_kw(at_resp_t resp, const char *expr, const char *kw, const char *dist);
uint8_t at_exec_cmd_wait_expr(at_master_t at_master, const char *expr, const char *cmd, int len, uint32_t timeout);

#endif // AT_H
