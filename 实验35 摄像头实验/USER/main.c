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
//ALIENTEK 探索者STM32F407开发板 实验35
//摄像头 实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

u8 ov2640_mode=0;						//工作模式:0,RGB565模式;1,JPEG模式

int xmax, xmin, ymax, ymin;
int bFindPoint = 0, bFindBall = 0;
unsigned int ToGrey(unsigned int rgb)
{
	unsigned int blue = (rgb & 0x000000FF) >> 0;
	unsigned int green = (rgb & 0x0000FF00) >> 8;
	unsigned int red = (rgb & 0x00FF0000) >> 16;
	return (red * 38 + green * 75 + blue * 15) >> 7;
}

#define jpeg_buf_size 31*1024  			//定义JPEG数据缓存jpeg_buf的大小(*4字节)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG数据缓存buf
volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
//JPEG尺寸支持列表
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
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
const u8*JPEG_SIZE_TBL[9]={"QCIF","QQVGA","CIF","QVGA","VGA","SVGA","XGA","SXGA","UXGA"};	//JPEG图片 9种尺寸 


//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	if(ov2640_mode)//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//停止当前传输 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//得到此次数据传输的长度
				
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//传输长度为jpeg_buf_size*4字节
			DMA_Cmd(DMA2_Stream1, ENABLE);			//重新传输
			jpeg_data_ok=0;						//标记数据未采集
		}
	}
} 
//JPEG测试
//JPEG数据,通过串口2发送给电脑.
void jpeg_test(void)
{
	u32 i; 
	u8 *p;
	u8 key;
	u8 effect=0,saturation=2,contrast=2;
	u8 size=3;		//默认是QVGA 320*240尺寸
	u8 msgbuf[15];	//消息缓存区 
	LCD_Clear(WHITE);
  POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV2640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,120,200,16,16,"KEY1:Saturation"); 		//色彩饱和度
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//分辨率设置 
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	LCD_ShowString(30,180,200,16,16,msgbuf);					//显示当前JPEG分辨率
	
 	OV2640_JPEG_Mode();		//JPEG模式
	My_DCMI_Init();			//DCMI配置
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA配置   
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸 
	DCMI_Start(); 		//启动传输
	while(1)
	{
		if(jpeg_data_ok==1)	//已经采集完一帧图像了
		{  
			p=(u8*)jpeg_buf;
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //提示正在传输数据
			for(i=0;i<jpeg_data_len*4;i++)		//dma传输1次等于4字节,所以乘以4.
			{
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);	//循环发送,直到发送完毕  		
				USART_SendData(USART2,p[i]); 
				key=KEY_Scan(0); 
				if(key)break;
			} 
			if(key)	//有按键按下,需要处理
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//提示退出数据传输
				switch(key)
				{				    
					case KEY0_PRES:	//对比度设置
						contrast++;
						if(contrast>4)contrast=0;
						OV2640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-2);
						break;
					case KEY1_PRES:	//饱和度Saturation
						saturation++;
						if(saturation>4)saturation=0;
						OV2640_Color_Saturation(saturation);
						sprintf((char*)msgbuf,"Saturation:%d",(signed char)saturation-2);
						break;
					case KEY2_PRES:	//特效设置				 
						effect++;
						if(effect>6)effect=0;
						OV2640_Special_Effects(effect);//设置特效
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
						break;
					case WKUP_PRES:	//JPEG输出尺寸设置   
						size++;  
						if(size>8)size=0;   
						OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
						break;
				}
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 				  
			}else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//提示传输结束设置 
			jpeg_data_ok=2;	//标记jpeg数据处理完了,可以让DMA去采集下一帧了.
		}		
	}    
} 
int x, y;
//RGB565测试
//RGB数据直接显示在LCD上面
void rgb565_test(void)
{ 
	u8 key;
	u8 effect=0,saturation=2,contrast=2;
	u8 scale=1;		//默认是全尺寸缩放
	u8 msgbuf[15];	//消息缓存区 
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV2640 RGB565 Mode");
	
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,130,200,16,16,"KEY1:Saturation"); 		//色彩饱和度
	LCD_ShowString(30,150,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,170,200,16,16,"KEY_UP:FullSize/Scale");	//1:1尺寸(显示真实尺寸)/全尺寸缩放
	
	OV2640_RGB565_Mode();	//RGB565模式
	My_DCMI_Init();			//DCMI配置 
	DCMI_DMA_Init((u32)&LCD->LCD_RAM,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置  
 	OV2640_OutSize_Set(lcddev.width,lcddev.height); 
	DCMI_Start(); 		//启动传输
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	TIM2_PWM_Init(9999, 84 * 2 - 1);//50HZ
	TIM3_Int_Init(50 - 1, 8400 - 1);//10Khz计数,1秒钟中断一次
	pwmx_set = offset_X;
	pwmy_set = offset_Y;
	pwmx = offset_X;
	pwmy = offset_Y;
	AngleX = offset_X;
	AngleY = offset_Y;
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart2_init(42,115200);		//初始化串口2波特率为115200
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化 
	AngleX = offset_X;
	AngleY = offset_Y;
 	usmart_dev.init(84);		//初始化USMART
	AngleX = offset_X;
	AngleY = offset_Y;
 	POINT_COLOR=RED;//设置字体为红色 
//	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
//	LCD_ShowString(30,70,200,16,16,"OV2640 TEST");	
//	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,110,200,16,16,"2014/5/14");  	 
//	
//	while(OV2640_Init())//初始化OV2640                                                                                                   
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
