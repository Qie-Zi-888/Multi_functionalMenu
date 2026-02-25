#include "cJSON_port.h"
#include "cJSON.h"
#include "FreeRTOS.h"

static cJSON_Hooks s_cjson_hooks;

void cJSON_config(void)
{
    s_cjson_hooks.malloc_fn = pvPortMalloc;
    s_cjson_hooks.free_fn = vPortFree;
    cJSON_InitHooks(&s_cjson_hooks);
}
