#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"  
#include "usart2.h"  
#include "timer.h" 
#include "ov2640.h" 
#include "dcmi.h" 
#include "control.h"
//ALIENTEK ̽����STM32F407������ ʵ��35
//����ͷ ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

u8 ov2640_mode=0;						//����ģʽ:0,RGB565ģʽ;1,JPEGģʽ

int xmax, xmin, ymax, ymin;
int bFindPoint = 0, bFindBall = 0;
unsigned int ToGrey(unsigned int rgb)
{
	unsigned int blue = (rgb & 0x000000FF) >> 0;
	unsigned int green = (rgb & 0x0000FF00) >> 8;
	unsigned int red = (rgb & 0x00FF0000) >> 16;
	return (red * 38 + green * 75 + blue * 15) >> 7;
}

#define jpeg_buf_size 31*1024  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
//JPEG�ߴ�֧���б�
const u16 jpeg_img_size_tbl[][2]=
{
	176,144,	//QCIF
	160,120,	//QQVGA
	352,288,	//CIF
	320,240,	//QVGA
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,1024,	//SXGA
	1600,1200,	//UXGA
}; 
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
const u8*JPEG_SIZE_TBL[9]={"QCIF","QQVGA","CIF","QVGA","VGA","SVGA","XGA","SXGA","UXGA"};	//JPEGͼƬ 9�ֳߴ� 


//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
	if(ov2640_mode)//ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//ֹͣ��ǰ���� 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//�õ��˴����ݴ���ĳ���
				
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA_Cmd(DMA2_Stream1, ENABLE);			//���´���
			jpeg_data_ok=0;						//�������δ�ɼ�
		}
	}
} 
//JPEG����
//JPEG����,ͨ������2���͸�����.
void jpeg_test(void)
{
	u32 i; 
	u8 *p;
	u8 key;
	u8 effect=0,saturation=2,contrast=2;
	u8 size=3;		//Ĭ����QVGA 320*240�ߴ�
	u8 msgbuf[15];	//��Ϣ������ 
	LCD_Clear(WHITE);
  POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV2640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,120,200,16,16,"KEY1:Saturation"); 		//ɫ�ʱ��Ͷ�
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//�ֱ������� 
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	LCD_ShowString(30,180,200,16,16,msgbuf);					//��ʾ��ǰJPEG�ֱ���
	
 	OV2640_JPEG_Mode();		//JPEGģʽ
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA����   
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ� 
	DCMI_Start(); 		//��������
	while(1)
	{
		if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
		{  
			p=(u8*)jpeg_buf;
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //��ʾ���ڴ�������
			for(i=0;i<jpeg_data_len*4;i++)		//dma����1�ε���4�ֽ�,���Գ���4.
			{
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);	//ѭ������,ֱ���������  		
				USART_SendData(USART2,p[i]); 
				key=KEY_Scan(0); 
				if(key)break;
			} 
			if(key)	//�а�������,��Ҫ����
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//��ʾ�˳����ݴ���
				switch(key)
				{				    
					case KEY0_PRES:	//�Աȶ�����
						contrast++;
						if(contrast>4)contrast=0;
						OV2640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-2);
						break;
					case KEY1_PRES:	//���Ͷ�Saturation
						saturation++;
						if(saturation>4)saturation=0;
						OV2640_Color_Saturation(saturation);
						sprintf((char*)msgbuf,"Saturation:%d",(signed char)saturation-2);
						break;
					case KEY2_PRES:	//��Ч����				 
						effect++;
						if(effect>6)effect=0;
						OV2640_Special_Effects(effect);//������Ч
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
						break;
					case WKUP_PRES:	//JPEG����ߴ�����   
						size++;  
						if(size>8)size=0;   
						OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ�  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
						break;
				}
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//��ʾ��ʾ����
				delay_ms(800); 				  
			}else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//��ʾ����������� 
			jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
		}		
	}    
} 
int x, y;
//RGB565����
//RGB����ֱ����ʾ��LCD����
void rgb565_test(void)
{ 
	u8 key;
	u8 effect=0,saturation=2,contrast=2;
	u8 scale=1;		//Ĭ����ȫ�ߴ�����
	u8 msgbuf[15];	//��Ϣ������ 
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV2640 RGB565 Mode");
	
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,130,200,16,16,"KEY1:Saturation"); 		//ɫ�ʱ��Ͷ�
	LCD_ShowString(30,150,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,170,200,16,16,"KEY_UP:FullSize/Scale");	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/ȫ�ߴ�����
	
	OV2640_RGB565_Mode();	//RGB565ģʽ
	My_DCMI_Init();			//DCMI���� 
	DCMI_DMA_Init((u32)&LCD->LCD_RAM,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA����  
 	OV2640_OutSize_Set(lcddev.width,lcddev.height); 
	DCMI_Start(); 		//��������
	while(1)
	{ 
		KEY_Scan(0);
		//delay_ms(500);
		xmax = 0; xmin = 240; ymax = 0; ymin = 320;
		DCMI_Stop();
		bFindPoint = 0;
		bFindBall = 0;
		for (x = 60; x< 230; x += 4)
		{
			bFindPoint = 0;
			for (y = 10;y< 310; y += 4)
			{
				if (ToGrey(LCD_ReadPoint(x, y))< 40)
				{
					if (x > xmax)xmax = x;
					if (y> ymax)ymax = y;
					if (x<xmin)xmin = x;
					if (y< ymin)ymin = y;
					bFindPoint = 1;
					if (bFindBall == 0)bFindBall = 1;
				}
			}
			if (bFindBall == 1&& bFindPoint == 0)break;
		}
		pos_X = (xmax + xmin) / 2;
		pos_Y = (ymin + ymax) / 2;
		clacFlag = 1;
//		LCD_DrawLine(0, 160, 240, 160);
//		LCD_DrawLine(145, 0, 145, 320);
		LCD_DrawLine(setX, 0, setX, 320);
		LCD_DrawLine(0, setY, 240, setY);
		LCD_DrawRectangle(xmin, ymax, xmax, ymin);
		LCD_DrawRectangle(60, 310, 230, 10);
//		LCD_ShowNum(50, 50, pos_X, 3, 16);
		LCD_ShowNum(50, 80, outputX, 3, 16);
		LCD_ShowNum(50, 110,offset_X, 3, 16);
		LCD_ShowNum(50, 140, offset_Y, 3, 16);
		DCMI_Start();
	}    
} 
int main(void)
{ 
	u8 key;
	u8 t;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	TIM2_PWM_Init(9999, 84 * 2 - 1);//50HZ
	TIM3_Int_Init(50 - 1, 8400 - 1);//10Khz����,1�����ж�һ��
	pwmx_set = offset_X;
	pwmy_set = offset_Y;
	pwmx = offset_X;
	pwmy = offset_Y;
	AngleX = offset_X;
	AngleY = offset_Y;
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart2_init(42,115200);		//��ʼ������2������Ϊ115200
	LED_Init();					//��ʼ��LED 
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ�� 
	AngleX = offset_X;
	AngleY = offset_Y;
 	usmart_dev.init(84);		//��ʼ��USMART
	AngleX = offset_X;
	AngleY = offset_Y;
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
//	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
//	LCD_ShowString(30,70,200,16,16,"OV2640 TEST");	
//	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,110,200,16,16,"2014/5/14");  	 
//	
//	while(OV2640_Init())//��ʼ��OV2640                                                                                                   
//	{
//		LCD_ShowString(30,130,240,16,16,"OV2640 ERR");
//		delay_ms(200);
//	    LCD_Fill(30,130,239,170,WHITE);
//		delay_ms(200);
//	}
//	LCD_ShowString(30,130,200,16,16,"OV2640 OK");  	  

	if(0)jpeg_test();
	pwmx=offset_X;
	LCD_Draw_Circle(32, 164, 12);
	LCD_ShowNum(32-6, 164-6, 1, 3, 16);
	LCD_Draw_Circle(30, 99, 12);
	LCD_ShowNum(30 - 6, 99 - 6, 2, 3, 16);
	LCD_Draw_Circle(31, 34, 12);
	LCD_ShowNum(31 - 6, 34 - 6, 3, 3, 16);
	LCD_Draw_Circle(97, 164, 12);
	LCD_ShowNum(97 - 6, 164 - 6, 4, 3, 16);
	LCD_Draw_Circle(98, 99, 12);
	LCD_ShowNum(98 - 6, 99 - 6, 5, 3, 16);
	LCD_Draw_Circle(96, 31, 12);
	LCD_ShowNum(96 - 6, 31 - 6, 6, 3, 16);
	LCD_Draw_Circle(163, 162, 12);
	LCD_ShowNum(163 - 6, 162 - 6, 7, 3, 16);
	LCD_Draw_Circle(164, 98, 12);
	LCD_ShowNum(164 - 6, 98 - 6, 8, 3, 16);
	LCD_Draw_Circle(163, 31, 12);
	LCD_ShowNum(163 - 6, 31 - 6, 9, 3, 16);
	LCD_ShowString(10, 220, 200, 16, 16, "Renesas Cup");
	LCD_ShowString(5, 240, 4, 16, 16, "X:");
	LCD_ShowString(5, 260, 4, 16, 16, "Y:");
	
	//LCD_DrawRectangle(0, 0, 196, 196);
	while (1)
	{
		KEY_Scan(0);
		//LCD_Clear(WHITE);
		LCD_ShowNum(10, 240, g_ball_x, 3, 16);
		LCD_ShowNum(10, 260, g_ball_y, 3, 16);
		LCD_Fast_DrawPoint(g_ball_x, g_ball_y,0x00ff);
		delay_ms(10);
	}
	//else rgb565_test(); 
}
