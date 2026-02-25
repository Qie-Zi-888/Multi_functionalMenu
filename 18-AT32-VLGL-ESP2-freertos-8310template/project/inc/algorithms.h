#ifndef __ALGORITHMS_H
#define __ALGORITHMS_H

#include "cJSON.h"
#include "FreeRTOS.h"
#include "semphr.h"
typedef struct weather *weather_t;
typedef struct chatGpt *chatGpt_t;
struct weather
{
    /* data */
    const char *city;
    const char *temperature;
    const char *weather_phenomena;
    const char *humidity;
};
typedef struct 
{
	const char *city;
    const char *temperature;
    const char *weather_phenomena;
    const char *humidity;
}MyWeather;

struct chatGpt {
    const char *txt;
};

typedef struct 
{
	const char *txt;
}MyAI;
extern MyWeather myweather;
extern MyAI myaichat;
extern QueueHandle_t MutexSemaphore;
extern unsigned char showflag,recvflag;
void bubbleSortVisualization_task(void *pvParameters);
void parsing_Weather(cJSON *root);
void parsing_Chatgpt(cJSON *root);
void parsing_JsonData(char *json_str);
void parsing_OnePack();
void ChessGame_task(void *pvParameters);

#endif