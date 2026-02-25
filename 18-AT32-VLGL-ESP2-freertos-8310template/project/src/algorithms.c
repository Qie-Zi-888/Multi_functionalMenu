#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "displayChess.h"
#include "chess.h"
#include "check.h"
#include "bsp.h"
#include "bsp_uart.h"
#include "bsp_esp12.h"
#include "gui_guider.h"
QueueHandle_t MutexSemaphore;
extern QueueHandle_t Queue_RequestData;
unsigned char showflag=0;
// 随机数据生成
void generateRandomArray(int arr[], int size)
{
    srand(2); // 设置随机种子
    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % 300; // 生成0-99之间的随机数
    }

}

// 冒泡排序可视化
void bubbleSortVisualization_task(void *pvParameters)
{
	lcd_fill(0, 0, 320,  480,BLACK);
	// 原点坐标（480, 0）
	char size = 15;
	int width = 30;
	//lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
    int arr[20];
	while(1){
		generateRandomArray(arr, size);
		for (int i = 0; i < size; i++) {
			printf("%d,", arr[i]);
			lcd_fill(480 - (i + 1) * width, 0, 480 - i * width - 5, arr[i], RED);
		}
		printf("\n");
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size - i - 1; j++)
			{
				if (arr[j] > arr[j + 1])
				{
					int temp = arr[j];
					// 要交换的两个矩形标记为绿色
					lcd_fill(480 - (j + 1) * width, 0, 480 - j * width - 5, arr[j], GREEN); // Assuming 100 is max height and BLACK is the background color
					lcd_fill(480 - (j + 2) * width, 0, 480 - (j + 1) * width - 5, arr[j + 1], GREEN);
					arr[j] = arr[j + 1];
					arr[j + 1] = temp;

					delay_ms(800);
					// Draw the new bars
					lcd_fill(480 - (j + 1) * width, 0, 480 - j * width - 5, 320, BLACK);
					lcd_fill(480 - (j + 2) * width, 0, 480 - (j + 1) * width - 5, 320, BLACK);
					lcd_fill(480 - (j + 1) * width, 0, 480 - j * width - 5, arr[j], RED);
					lcd_fill(480 - (j + 2) * width, 0, 480 - (j + 1) * width - 5, arr[j + 1], RED);
				}
			}
			// 将排好序的矩形标记为蓝色
			lcd_fill(480 - (size  - i) * width, 0, 480 - (size - i - 1) * width - 5, arr[size - i - 1], BLUE);
			delay_ms(1000);
		}
		vTaskDelay(20);
	}
}

MyWeather myweather;

void parsing_Weather(cJSON *root)
{
    weather_t weather;
    cJSON *lives = cJSON_GetObjectItem(root, "lives");
    // 获取第一个元素
    cJSON *first_element = cJSON_GetArrayItem(lives, 0);

    // 获取humidity字段的值
    cJSON *humidity_item = cJSON_GetObjectItem(first_element, "humidity");
    weather->humidity = humidity_item->valuestring;
	myweather.humidity=weather->humidity;
	
	
	 // 获取humidity字段的值
    cJSON *weather_phenomena_item = cJSON_GetObjectItem(first_element, "weather");
    weather->weather_phenomena = weather_phenomena_item->valuestring;
	myweather.weather_phenomena=weather->weather_phenomena;
	
	 // 获取humidity字段的值
    cJSON *temperature_item = cJSON_GetObjectItem(first_element, "temperature");
    weather->temperature = temperature_item->valuestring;
	myweather.temperature=weather->temperature;
	
	showflag=1;

}
unsigned char recvflag=0;

MyAI myaichat;
void parsing_Chatgpt(cJSON * root) {
   chatGpt_t zhipuAI;
    cJSON *txt = cJSON_GetObjectItem(root, "ZhipuAi");
    zhipuAI->txt = txt->valuestring;
	printf("zhipuai: %s\n", zhipuAI->txt);
	myaichat.txt=zhipuAI->txt;
	printf("%s\n",myaichat.txt);
	recvflag=1;
	xSemaphoreTake(MutexSemaphore, portMAX_DELAY); /* 获取互斥信号量 */
	lv_textarea_set_text(guider_ui.screen_ai_ta, zhipuAI->txt);
	xSemaphoreGive(MutexSemaphore); /* 释放互斥信号量 */
}

// 分发json数据包
void parsing_JsonData(char *json_str) {
    cJSON *root = cJSON_Parse(json_str);
	// if (root == NULL)
    //     printf("json fmt error:%s\r\n.", cJSON_GetErrorPtr());
	// printf("%s\r\n", cJSON_Print(root));
    cJSON *lives = cJSON_GetObjectItem(root, "lives");
    if(lives != NULL) {;}//parsing_Weather(root);}
    else parsing_Chatgpt(root);
    cJSON_Delete(root);
}

extern int queueflag;
char onePacket[2000];

// 解析出一个json数据
void parsing_OnePack()
{
	//printf("timers1");
    static int count = 0;
    char temp = 0;
    static int bracketsCount=0;
    static char state = 0;
    //while (1)
	//if(queueflag)
    {
//        xQueueReceive(Queue_RequestData, &temp, 0);
		//printf("Queee:%ld",uxQueueMessagesWaiting(Queue_RequestData));
		if (comGetChar(COM3, &temp))
		{		
			printf("%c",temp);
			switch (state)
			{
			case 0:
				if (temp == '{')
				{
					onePacket[count++] = temp;
					bracketsCount++;
					state = 1;
				}
				break;
			case 1:
				if (temp == '{')
					bracketsCount++;
				if (temp == '}')
					bracketsCount--;
				onePacket[count++] = temp;
				if (bracketsCount == 0)
				{
					parsing_JsonData(onePacket);
					count = 0;
					state = 0;
				}

			default:
				break;
			}
		}
    }
}


// 象棋游戏
int8_t selectIndex = -2;
void ChessGame_task(void *pvParameters)
{
    lcd_init(0x02);
    lcd_clear(BLACK);
    initUI(); // 初始化界面
    touch_init(0x00);
    Path path;
    PieceTypedef tempPiece;
    ChangeBuffer changeBuffer;
    int8_t clearFlag;
    int8_t assassin; // 刺客，是哪个棋子将军的
    while (1)
    {
        getPath(&path);
        tempPiece = piece[path.indexS];

        /*判断得到的path是否符合规则，符合则返回1*/
        if (tempPiece.judge(tempPiece.x, tempPiece.y, path.x, path.y, TURN))
        {
            change(path.indexS, path.x, path.y, &changeBuffer); // 先把棋子的数据和棋局数据更改，界面并没有改变，因为还没判断这样走是否被将军

            /*判断执行change()过后，是否被将军，被将军则移动无效，需要backChange()*/
            if (check(!tempPiece.camp, &assassin) || generalLine())
            {
                backChange(&changeBuffer);  // 这样走不行，会被将军，所以撤销移动
                deSelectPiece(path.indexS); // 把屏幕中选中的棋子取消选中
                printf("无效");
                write_Chinese(sX, sY, '无', DARKBLUE, Libbuttom, lengthCH2);
                write_Chinese(sX + lengthCH2, sY, '效', DARKBLUE, Libbuttom, lengthCH2);
                clearFlag = 1;
            }
            else // 这样走可行
            {
                clearPiece(changeBuffer.AX, changeBuffer.AY); // 把旧棋子擦除,注意，到这里才改动界面
                makePiece(path.indexS);                       // 在移动路径的目的地画一个新棋子
                changeBuffer2 = changeBuffer;                 // 结构体赋值，changeBuffer2专用来悔棋，它存储的是真正移子过程的数据
                undoEn = 1;                                   // 悔棋允许标志位，每次移子之后使能，悔棋之后禁止，

                if (check(tempPiece.camp, &assassin)) // 每次移子后判断是否将军
                {
                    // PBout(8) = 1;           // 蜂鸣器响
                    if (judgeWin(assassin)) // 如果将军了接着进一步判断是否将死
                    {

                        if (TURN)
                            write_Chinese(sX, sY, '黑', BLACK, Libbuttom, lengthCH2);
                        else
                            write_Chinese(sX, sY, '红', RED, Libbuttom, lengthCH2);
                        write_Chinese(sX + lengthCH2, sY, '方', CYAN, Libbuttom, lengthCH2);
                        write_Chinese(sX + lengthCH2 * 2, sY, '胜', CYAN, Libbuttom, lengthCH2);

                        // PBout(8) = 0;
                        changeBuffer2.indexSrc = -2; // 中断里会检测这个值，检测到就return
                        while (1)
                            ;
                    }
                    write_Chinese(sX, sY, '将', TURN ? BLACK : RED, Libbuttom, lengthCH2);
                    write_Chinese(sX + lengthCH2, sY, '军', TURN ? BLACK : RED, Libbuttom, lengthCH2);
                    clearFlag = 1; // 下一次可以擦除提示
                                   // PBout(8) = 0;  // 关闭蜂鸣器
                }
                else if (clearFlag) // 是否有提示信息要擦除
                {
                    lcd_fill(sX, sY, sX + 2 * lengthCH2, sY + lengthCH2, BROWN);
                    clearFlag = 0;
                }

                TURN = !TURN;
            }
        }
        else // 得到的path路径不符合规则
        {
            deSelectPiece(path.indexS); // 取消选中状态
            printf("无效");
            write_Chinese(sX, sY, '无', DARKBLUE, Libbuttom, lengthCH2);
            write_Chinese(sX + lengthCH2, sY, '效', DARKBLUE, Libbuttom, lengthCH2);
            clearFlag = 1;
        }
		vTaskDelay(20);
    }
}
