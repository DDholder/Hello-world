#include "key.h"
#include "delay.h" 
#include "control.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//按键输入驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//按键初始化函数
void KEY_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOA,GPIOE时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; //KEY0 KEY1 KEY2对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOE2,3,4



}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
int temp = 100;
u8 KEY_Scan(u8 mode)
{
	if (KEYPress == 0)
	{
		delay_ms(10);
		if (KEYPress==0)
		{
			while (!KEYPress);
			runFlag = !runFlag;
		}
		return 0;// 无按键按下
	}
	else if (KEYU == 0)
	{
		delay_ms(10);
		if (KEYU==0)
		{
			while (!KEYU);
			offset_Y+=10;
		}
		return 0;// 无按键按下
	}
	else if (KEYL == 0)
	{
		delay_ms(10);
		if (KEYL==0)
		{
			while (!KEYL);
			offset_X-=10;
		}
		return 0;// 无按键按下
	}
	else if (KEYD == 0)
	{
		delay_ms(10);
		if (KEYD == 0)
		{
			while (!KEYD);
			offset_Y-= 10;
		}
		return 0;// 无按键按下
	}
	else if (KEYR == 0)
	{
		delay_ms(10);
		if (KEYR == 0)
		{
			while (!KEYR);
			offset_X+= 10;
		}
		return 0;// 无按键按下
	}
	else
		return 9;
}




















