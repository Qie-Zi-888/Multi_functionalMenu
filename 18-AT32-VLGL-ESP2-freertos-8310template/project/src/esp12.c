#include "esp12.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"

void esp12_connect()
{

//      ESP12_SendAT("AT+RESTORE"); // 恢复出厂设置
//     if (ESP12_WaitResponse("ready", 50) == 1)
//     {
//       printf("\r\n ESP12 RESTRE OK\r\n");
//        delay_ms(1000);
//     }
    ESP12_SendAT("AT+CWMODE=1");
    if (ESP12_WaitResponse("OK", 1000) == 1)
    {
        printf("\r\n ESP12 CWMODE1 OK\r\n");
        vTaskDelay(1000);
    }

    ESP12_SendAT("AT+CWJAP=\"lj\",\"12345679\""); // 连接wifi
    if (ESP12_WaitResponse("WIFI CONNECTED", 6000) == 1)
    {
        printf("\r\n WIFI CONNECTED\r\n");
        vTaskDelay(1000);
    }

    ESP12_SendAT("AT+CIPSTART=\"TCP\",\"192.168.239.189\",8890"); // 连接tcp服务器
    if (ESP12_WaitResponse("OK", 1000) == 1)
    {
        printf("\r\n server connect\r\n");
        vTaskDelay(3000);
    }

    ESP12_SendAT("AT+CIPMODE=1"); // 透传模式
    if (ESP12_WaitResponse("OK", 1000) == 1)
    {
        printf("\r\n CIPMODE=1\r\n");
        vTaskDelay(3000);
    }

    ESP12_SendAT("AT+CIPSEND"); // 进入透传
    if (ESP12_WaitResponse("OK", 1000) == 1)
    {
        printf("\r\n CIPSEND\r\n");
        vTaskDelay(1000);
    }
}
