#include <stdio.h>
#include "displayChess.h"
#include "bsp_gt911.h"
#include "font.h"
#include "bsp.h"
#include "bsp_lcd.h"
#include "chess.h"

extern int8_t selectIndex;

//这里使用的这些全局变量用来设置界面的参数，实际项目中尽量少使用全局变量，尽量使用宏定义，这里适配两块屏幕不得已使用
u16 width = 320;    //屏幕宽度
u16 height = 480;   //屏幕高度
int16_t offsetX  = 16;   //棋盘距离屏幕左右两边缘的差距
int16_t offsetY1 = 20;   //棋盘距离屏幕上边缘的差距
int16_t offsetY2 = 125;  //棋盘距离屏幕下边缘的差距
u8 pieceR = 19;     //棋子的半径
u8 lengthCH = 32;
u8 lengthCH2 = 64;  //底部提示字的边长
u16 sX = 28;        //提示字的起始位置的横坐标
u16 sY = 380;       //提示字的起始位置的纵坐标
u8 sizeENG = 24;    //英语提示的字形（12，16，24）

unsigned char * Libpiece =  (u8*)GB_3232;//棋子上的字的字库选择
unsigned char * Libbuttom  = (u8*)GB_6464;//下部提示字的字库选择

 
//这两个数组可将棋盘格点坐标转换成屏幕的像素坐标
//棋盘格点：棋盘线的交点9x10
//屏幕的像素坐标:屏幕每一个像素点有一个坐标，比如原子4.3寸液晶屏为480x800个像素点
//数组下标取棋盘格点坐标，就得到对应的像素坐标
u16 turnPiX[9];     
u16 turnPiY[10];


void initUI()
{
    u8 divX,divY,i;
    divX = (width-2*offsetX)/8;            //方格的宽度
    divY = (height-offsetY1-offsetY2)/9;   //方格的高度
    lcd_clear(BROWN);

	 for(i=0;i<=8;i++)                        //为 turnPiX[]，turnPiY[]赋值，执行了这里，后面才能使用这两个数组
    {
        turnPiX[i] = offsetX+i*divX;
    }
    for(i=0;i<=9;i++)
    {
        turnPiY[9-i] = offsetY1+i*divY;
    }
	 
	 for(i=0;i<=8;i++)
	 {
         lcd_draw_line(turnPiX[i], turnPiY[0], turnPiX[i], turnPiY[9], RED); // 画竖线
     }		 
    for(i=0;i<=9;i++)
    {
        lcd_draw_line(turnPiX[0], turnPiY[i], turnPiX[8], turnPiY[i], RED); // 画横线
     }	
	 
    lcd_fill(turnPiX[0]+1,turnPiY[5]+1,turnPiX[8]-1,turnPiY[4]-1,BROWN);//画中间隔开的区域

    lcd_draw_line(turnPiX[3], turnPiY[0], turnPiX[5], turnPiY[2], RED);
    lcd_draw_line(turnPiX[3], turnPiY[2], turnPiX[5], turnPiY[0], RED);
    lcd_draw_line(turnPiX[3], turnPiY[9], turnPiX[5], turnPiY[7], RED);
    lcd_draw_line(turnPiX[3], turnPiY[7], turnPiX[5], turnPiY[9], RED);

    initPiece();        //画每一个棋子，包括字
}




//制作一个新的棋子
void makePiece(int8_t index)   
{
    u16 piX,piY;
    piX = turnPiX[piece[index].x];     //获取像素点坐标
    piY = turnPiY[piece[index].y];

    lcd_fill_circle(piX,piY,pieceR,WHITE);     //画棋子的圆
	pieceWrite(index,Libpiece,lengthCH);      //为棋子写字  

}

// //擦除旧棋子，并把附带擦除的棋盘补回来
void clearPiece(int8_t x,int8_t y)
{
    u16 piX,piY,tempColor;

    piX = turnPiX[x];
    piY = turnPiY[y];

    lcd_fill_circle(piX, piY, pieceR, BROWN); // 擦除旧的棋子的痕迹
	
	 //以(piX,piY)为中心，画两根交叉的红线，以补足刚刚擦除的棋盘，在某些地方这样画会有红线多出来
    lcd_draw_line(piX - pieceR, piY, piX + pieceR, piY, RED);
    lcd_draw_line(piX, piY - pieceR, piX, piY + pieceR, RED);

    // 以下用来补斜线
    if ((x == 3 || x == 5) && (y == 0 || y == 2))
    {
        chess_drawline(piX, piY, turnPiX[4], turnPiY[1], RED);
     }
	 if( (x==3||x==5) && (y==7||y==9) )
	 {
         chess_drawline(piX, piY, turnPiX[4], turnPiY[8], RED);
     }
	 if(x==4 && y==1)
	 {
         chess_drawline(piX, piY, turnPiX[3], turnPiY[0], RED);
         chess_drawline(piX, piY, turnPiX[3], turnPiY[2], RED);
         chess_drawline(piX, piY, turnPiX[5], turnPiY[0], RED);
         chess_drawline(piX, piY, turnPiX[5], turnPiY[2], RED);
     }
	 if(x==4 && y==8)
	 {
         chess_drawline(piX, piY, turnPiX[3], turnPiY[7], RED);
         chess_drawline(piX, piY, turnPiX[3], turnPiY[9], RED);
         chess_drawline(piX, piY, turnPiX[5], turnPiY[7], RED);
         chess_drawline(piX, piY, turnPiX[5], turnPiY[9], RED);
     }

    //棋盘边缘多出的线进行擦除
    if(x==0)
    {
        lcd_draw_line(piX - pieceR, piY, piX, piY, BROWN);
    }
    if(x==8)
    {
        lcd_draw_line(piX, piY, piX + pieceR, piY, BROWN);
    }
    if(y==0)
    {
        lcd_draw_line(piX, piY, piX, piY + pieceR, BROWN);
    }
    if(y==5 && x!=0 && x!=8)
    {
        lcd_draw_line(piX, piY, piX, piY + pieceR, BROWN);
    }
    if(y==9)
    {
        lcd_draw_line(piX, piY, piX, piY - pieceR, BROWN);
    }
    if(y==4 && x!=0 && x!=8 )
    {
        lcd_draw_line(piX, piY, piX, piY - pieceR, BROWN);
    }
	 
}

//初始化棋子，包括画实心圆圆和写字
void initPiece(void)
{
    u8 i;
    for(i=0;i<=31;i++)
    {
       makePiece(i);
    }
}

//当getPath()函数里面检测到有触摸的时候会调用，将对应的棋子涂成绿色
void selectPiece(u8 index)
{
    u16 tempPix,tempPiy;
    tempPix = turnPiX[piece[index].x];
    tempPiy = turnPiY[piece[index].y];
    lcd_fill_circle(tempPix, tempPiy, pieceR, GREEN); // 画一个绿圆并写字
    pieceWrite(index,Libpiece,lengthCH);
}

//和上面的函数刚好相反，当把绿色棋子涂成白色，注意和clearPiece函数的区别
void deSelectPiece(u8 index)
{
    u16 tempPix,tempPiy;
    tempPix = turnPiX[piece[index].x];
    tempPiy = turnPiY[piece[index].y];
    lcd_fill_circle(tempPix, tempPiy, pieceR, WHITE); // 画一个白圆并写字
    pieceWrite(index,Libpiece,lengthCH);
}


// //获得玩家选择的移子的路径，存储到p指向的一个Path类型的变量。
// //一旦进入此函数，如果没有取到一个路径是不会返回的，将一直扫描。
// //注意这里获得的路径并不一定是符和规则的，要进行下一步判断才知。
void getPath(Path *p)
{
    int16_t pressX,pressY,getFlag,index,divX,divY;
	int16_t x,y;
	
	divX = (width-2*offsetX)/8;                  //方格的宽和高
    divY = (height-offsetY1-offsetY2)/9;
	
	
    selectIndex = -2;
    getFlag = 0;
    while(1)
    {
        while(touch_read(&x, &y))
        {
			        
			printf("%d, %d\n", x, y);
			pressX = (x - offsetX + 0.5*divX) / divX;//将触屏位置的像素坐标改为棋局的格点坐标（pressX，pressY）
            pressY = 9 - (int)(y + 0.5*divY - offsetY1) / divY;
			printf("%d, %d\n", pressX, pressX);
			  
            if(pressX > 8  || pressY >9)//超出范围，重新选取位置
            continue;
				
            index = pieceTable[pressX][pressY];
				
            if(!getFlag)//还没有选中棋子
            {
                if(index!=-1 && piece[index].camp==TURN)
                {
                    selectPiece(index);
                    selectIndex = index;
                    getFlag = 1;
                }
            }
            else//已经选中了棋子，要选择将要移动到的位置了
            {
                if(index!=-1 && piece[index].camp == TURN)//如果此时又点击了己方棋子
                {
                    deSelectPiece(selectIndex);//把之前选中的棋子取消选中
						 if(index != selectIndex)//第二次选中的和之前的不是同一个，则把第二次当作选中的棋子
                    {
                        selectIndex = index;
                        selectPiece(index);
                    }
                    else//第二次选中的和之前的是同一个，重新选择棋子
                    getFlag = 0;
                }
                else//成功获取到了玩家输入的路径，赋值到p指向的对象，然后返回
                {
                    p->indexS = selectIndex;
                    p->x = pressX;
                    p->y = pressY;
                    return;
                }
            }
				
        }       
    }
}
    
// /*******end of file*****************/
