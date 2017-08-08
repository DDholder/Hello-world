#include "key.h"
#include "delay.h" 
#include "control.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//������ʼ������
void KEY_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOA,GPIOEʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4; //KEY0 KEY1 KEY2��Ӧ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4



}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
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
		return 0;// �ް�������
	}
	else if (KEYU == 0)
	{
		delay_ms(10);
		if (KEYU==0)
		{
			while (!KEYU);
			offset_Y+=10;
		}
		return 0;// �ް�������
	}
	else if (KEYL == 0)
	{
		delay_ms(10);
		if (KEYL==0)
		{
			while (!KEYL);
			offset_X-=10;
		}
		return 0;// �ް�������
	}
	else if (KEYD == 0)
	{
		delay_ms(10);
		if (KEYD == 0)
		{
			while (!KEYD);
			offset_Y-= 10;
		}
		return 0;// �ް�������
	}
	else if (KEYR == 0)
	{
		delay_ms(10);
		if (KEYR == 0)
		{
			while (!KEYR);
			offset_X+= 10;
		}
		return 0;// �ް�������
	}
	else
		return 9;
}




















