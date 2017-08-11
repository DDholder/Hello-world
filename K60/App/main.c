/*
				 ██████╗ █████╗ ██╗   ██╗ ██████╗
				██╔════╝██╔══██╗██║   ██║██╔════╝
				██║     ███████║██║   ██║██║
				██║     ██╔══██║██║   ██║██║
				╚██████╗██║  ██║╚██████╔╝╚██████╗
				 ╚═════╝╚═╝  ╚═╝ ╚═════╝  ╚═════╝
*/
/*!
 * @file       main.c
 * @brief      2017电子设计大赛
 * @author     Jassy
 * @date       2017-8-4
 */
#include "common.h"
#include "include.h"
#include "../SOLGUI/SOLGUI_Menu.h"
#include "../SOLGUI/SOLGUI_Common.h"
#define SECTOR_NUM  (FLASH_SECTOR_NUM-1)         //尽量用最后面的扇区，确保安全 
#define Bet     ADC1_SE6a                        //定义电池电压测量AD引脚
#define FloatFillStr(x,y)   ((*(float *)&y)=x)   //浮点数转char型数组
#define StrFillFloat(x,y)   (y=(*(float *)&x))   //char型数组转浮点数 
#define SW_1_carflag		PTE3	
#define SW_2_undef			PTE4
#define SW_3_OLEDreflash	PTE5
#define Test_IO				PTE9
#define Beep				PTC1
 //====================函数声明========================
 //内部函数记得加static 
 //使用断言进行安全检查 
void PIT0_IRQHandler(void);
void PIT1_IRQHandler(void);
void PORTA_IRQHandler(void);
void DMA0_IRQHandler(void);
void uart3_handler(void);
void get_point_uart(unsigned char  c);
//====================参数定义========================
//中断引用变量记得加volatile
//只读变量记得加constant 
//减少全局变量定义 加static修饰 如需访问可构造函数
uint8 key_value = 0;		//键值
uint8 imgbuff[CAMERA_SIZE]; //定义存储接收图像的数组
float bet_value = 0;		//电池电压
float vcan_send_buff[8];	//山外上位机虚拟示波器		  
char vcan_send_flag = 0;	//山外上位机发送标志位
int beep_time = 0;			 //蜂鸣器响时间
int beep_cnt = 0;			 //蜂鸣器响计时
KEY_MSG_t keymsg;
char  head_1 = 0;			//串口帧头识别
char  head_2 = 0;
char  head_3 = 0;
char  head_4 = 0;
int g_ball_x = 0;
int g_ball_y = 0;
int point_y_set = 50;

int pwm_temp = 0;
int pwm_temp1 = 0;

int x_x = 0;
int x_y = 0;    //十字叉丝
int servo1_pwm = 0;
int servo2_pwm = 0;
extern int16 point[2];
int16 point_temp[2];
int16 point_temp1[2];
int time_cnt = 0;

int mode = 0;
//++++++++++++++++++++++++++++++++++++++++++++++++++++
/*!
 *  @brief      main函数
 *  @note
 */
 //system_MK60DZ10.c main函数之前start函数内开启硬件浮点
void main()
{
	DisableInterrupts; //关总中断
	printf("Init......begin\n");
	printf("**************************\n");
	/*		调试按键初始化
	*KEY硬件定义在VCAN_key.c文件修改
	*枚举类型KEY_e
	*/
	printf("Key Init......");
	key_init(KEY_MAX);					 //初始化全部按键
	//gpio_init(SW_1_carflag, GPI, 1);	 
	//gpio_init(SW_2_undef  , GPI, 1);
	//gpio_init(SW_3_OLEDreflash, GPI, 1); //初始化拨码开关 
	gpio_init(Test_IO, GPO, 1);			 //测试时序用 
	printf("OK\n");
	printf("Car flag Read......");
	/*初始化电压测量ADC*/
	printf("ADC_bet Init......");
	adc_init(Bet);
	printf("OK\n");
	/*蜂鸣器初始化*/
	printf("Beep Init......");
	gpio_init(Beep, GPO, 0); //开蜂鸣器
	printf("OK\n");
	/*OLED初始化*/
	printf("OLED Init......");
	OLED_Init(); //初始化OLED
	OLED_Fill(0x00); //黑屏
	draw_cauc_logo();
	DELAY_MS(300);
	gpio_set(Beep, 1);
	DELAY_MS(500);
	printf("OK\n");
	/*UI界面初始化*/
	printf("UI Init......");
	SOLGUI_Init(&UI_List);		    //UI初始化界面
	SOLGUI_Refresh();		    	//UI刷新 
	printf("OK\n");
	/*		FTM模块初始化
	*频率 10K 精度 1000
	*精度修改  MK60_FTM.H -> FTM0_PRECISON
	*/
	printf("PWM Init......");
	MotorInit();
	printf("OK\n");
	/*		软件I2C初始化      */
	printf("I2C Init......");
	IMU_IIC_Init();
	printf("OK\n");
	/*		姿态传感器初始化      */
	printf("8451&3050 Init......");
	//UpstandInit();
	printf("OK\n");
	/*       正交解码初始化      */
	printf("Encoder Init......");
	Encoder_Init();
	printf("OK\n");
	/*       FLASH初始化         */
	printf("Flash Init......");
	flash_init();					//初始化flash
	parameter_init();			    //初始化参数  此处装载所有掉电不丢失参数  
	printf("OK\n");
	/*       摄像头初始化        */
	printf("Camera Init......");
	//camera_init(p_imgbuff1);
	printf("OK\n");
	/*        串口初始化        */
	printf("Uart Init......");
	uart_init(UART3, 115200);
	printf("OK\n");
	/*        设置中断向量       */
	printf("Interrupt Init......");
	NVIC_SetPriorityGrouping(4);		//设置中断分组
	//set_irq_priority(DMA0_IRQn, 2);
	//set_irq_priority(PORTA_IRQn, 2);
	//NVIC_SetPriority(PIT0_VECTORn, 2);  //配置1ms中断高优先级
	set_irq_priority(PIT1_VECTORn, 3);    //配置5ms中断低优先级
	set_irq_priority(UART3_RX_TX_VECTORn, 2);

	//set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); 
	//set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler); 	
	set_vector_handler(UART3_RX_TX_VECTORn, uart3_handler);   // 设置中断服务函数到中断向量表里
	uart_rx_irq_en(UART3);                                    //开串口接收中断
	/*    初始化PIT定时器中断    */
	pit_init_ms(PIT0, 5); //初始化PIT0，定时时间为： 1ms
	set_vector_handler(PIT0_VECTORn, PIT0_IRQHandler); //设置PIT0的中断服务函数为 PIT0_IRQHandler
	enable_irq(PIT0_IRQn); //使能PIT0中断  
	pit_init_ms(PIT1, 80);
	set_vector_handler(PIT1_VECTORn, PIT1_IRQHandler);
	enable_irq(PIT1_IRQn);
	ov7725_eagle_img_flag = IMG_START;      //开始采集图像
	enable_irq(PORTA_IRQn);                 //允许PTA的中断
	printf("OK\n");
	printf("**************************\n");
	printf("Init...Over\n");
	EnableInterrupts; //开总中断
	while (1)
	{
		//发送山外上位机参数
		//if (vcan_send_flag == 1)
		//{
		vcan_sendware((uint8_t *)vcan_send_buff, sizeof(vcan_send_buff));
		//}
// 		ftm_pwm_duty(FTM3, FTM_CH0, 200);
// 		ftm_pwm_duty(FTM3, FTM_CH1, 200);
// 		ftm_pwm_duty(FTM3, FTM_CH2,pwm_temp );
// 		ftm_pwm_duty(FTM3, FTM_CH3, pwm_temp1);
		//花屏处理	
// 		if (gpio_get(SW_3_OLEDreflash) == 0)
// 		{
// 			OLED_Init();
// 			OLED_Fill(0x00);
// 		}					
		
		bet_value = (float)(ad_ave(Bet, ADC_12bit, 10)) / 4096 * 3.3 * 3; //测量电池电压                                 
	}
}



/*!
*  @brief      PIT0中断服务函数
*  @author	   Jassy
*  @date	   20161201
*  @note
*/
void PIT0_IRQHandler(void)
{
	point[0] = g_ball_x;
	point[1] = g_ball_y;
	if (mode == 0)
	{
		M1PID.SetPoint = 14;//81;
		M2PID.SetPoint = 100;// 99;
	}
	else
	{
		M1PID.SetPoint = 81;
		M2PID.SetPoint = 99;
	}
	servo1_pwm = PID_M1_PosLocCalc(point[0]);
	servo2_pwm = PID_M2_PosLocCalc(point[1]);
	servo1_pwm = servo1_pwm >= 1200 ? 1200 : servo1_pwm;
	servo1_pwm = servo1_pwm <= -1200 ? -1200 : servo1_pwm;
	servo2_pwm = servo2_pwm >= 1200 ? 1200 : servo2_pwm;
	servo2_pwm = servo2_pwm <= -1200 ? -1200 : servo2_pwm;
//	ftm_pwm_duty(FTM3, FTM_CH2, 4800 + servo2_pwm);
//	ftm_pwm_duty(FTM3, FTM_CH3, 4500 + servo1_pwm);
	ftm_pwm_duty(FTM3, FTM_CH2, 5450 + servo2_pwm);
	ftm_pwm_duty(FTM3, FTM_CH3, 5200 + servo1_pwm);
	PIT_Flag_Clear(PIT0); //清中断标志位
}

void PIT1_IRQHandler(void)
{
	key_value = key_scan();
	SOLGUI_InputKey(key_value);//传入GUI 
	SOLGUI_Menu_PageStage();
	SOLGUI_Refresh(); //OLED前台刷新
	PIT_Flag_Clear(PIT1);
}
#define FILTER_N 3
int Filter(int x, int x1, int x2) {
	int filter_buf[FILTER_N];
	int i, j;
	int filter_temp;

	filter_buf[0] = x;
	filter_buf[1] = x1;
	filter_buf[2] = x2;
	// 采样值从小到大排列（冒泡法）
	for (j = 0; j < FILTER_N - 1; j++) {
		for (i = 0; i < FILTER_N - 1 - j; i++) {
			if (filter_buf[i] > filter_buf[i + 1]) {
				filter_temp = filter_buf[i];
				filter_buf[i] = filter_buf[i + 1];
				filter_buf[i + 1] = filter_temp;
			}
		}
	}
	return filter_buf[(FILTER_N - 1) / 2];
}
/*!
*  @brief      PORTA中断服务函数
*  @since      v5.0
*/
int flagtemp = 1;
void PORTA_IRQHandler()
{
	uint8 n; //引脚号
	uint32 flag;

	while (!PORTA_ISFR);
	flag = PORTA_ISFR;
	PORTA_ISFR = ~0; //清中断标志位

	n = 29; //场中断
	if (flag & (1 << n)) //PTA29触发中断
	{
		//gpio_set(Test_IO, 1);
		camera_vsync();   //开始采集图像(设置图像地址)
		flag_imgbuff != 0 ? img_extract(img, p_imgbuff1, CAMERA_SIZE) : img_extract(img, p_imgbuff2, CAMERA_SIZE);
		param_reset();
		//find_black_point(x_x-10, x_x+75, x_y-10, x_y+75);

		/*point_temp1[0] = point_temp[0];
		point_temp[0] = point[0];
		point_temp1[1] = point_temp[1];
		point_temp[1] = point[1];
		point[1] = Filter(point[1], point_temp[1], point_temp1[1]);*/
		vcan_send_buff[0] = g_ball_x;
		vcan_send_buff[1] = g_ball_y;
		//   
// 				if(flagtemp==1)
// 				{
// 					point[0] = 88;
// 					point_temp[0] = 88;
// 					point[1] = 65;
// 					point_temp[1] = 65;
// 					flagtemp = 0;
// 				}
// 				point[0] = ABS(point[0] - point_temp[0]) >= 10 ? point_temp[0] : point[0];
// 				point[1] = ABS(point[1] - point_temp[1]) >= 10 ? point_temp[1] : point[1];
// 				point_temp[0] = point[0];
// 				point_temp[1] = point[1];





		M1PID.SetPoint = 77;
		M2PID.SetPoint = 80;
		vcan_send_buff[2] = M1PID.SetPoint;
		vcan_send_buff[3] = M2PID.SetPoint;
		servo1_pwm = PID_M1_PosLocCalc(point[0]);
		servo2_pwm = PID_M2_PosLocCalc(point[1]);
		servo1_pwm = servo1_pwm >= 1200 ? 1200 : servo1_pwm;
		servo1_pwm = servo1_pwm <= -1200 ? -1200 : servo1_pwm;
		servo2_pwm = servo2_pwm >= 1200 ? 1200 : servo2_pwm;
		servo2_pwm = servo2_pwm <= -1200 ? -1200 : servo2_pwm;
		ftm_pwm_duty(FTM3, FTM_CH2, 4800 + servo2_pwm);
		ftm_pwm_duty(FTM3, FTM_CH3, 4500 + servo1_pwm);
		//解压上一次图像													 800us
		//DirectionControl();		//方向控制输出							 1.8ms
		//GalAcceAndGyroY();		//采集计算加速度计和陀螺仪					 1ms
		//KalmanFilter();			//卡尔曼滤波，得出角度 时间				 40us
		//BalanceControl();		//直立控制输出
		//PWMOut();				//最终PWM输出给电机（本次的方向输出、直立输出和上次的速度输出叠加）
// 			Flag_SpeedControl++;
// 			if (Flag_SpeedControl >= 20)
// 			{
// 				SpeedControl();		//速度控制周期20*(1/150)s
// 				Flag_SpeedControl = 0;
// 			}
// 			SpeedControlOut();		//速度输出周期（1/150）s，也就是每次的速度控制分20次输出
// 			if(beep_time>0)
// 			{
// 				gpio_init(Beep, GPO, 0); //开蜂鸣器 
// 				beep_time--;
// 			}
// 			else
// 			{
// 				gpio_init(Beep, GPO, 1); //关蜂鸣器
// 				beep_time = 0;
// 			}
	}
}

/*!
*  @brief      DMA0中断服务函数
*  @since      v5.0
*/
void DMA0_IRQHandler()
{
	camera_dma();
	camera_get_img();	//等待图像传输完毕（完成标志变为开始标志）
	flag_imgbuff = !flag_imgbuff;
	ov7725_eagle_img_buff = (flag_imgbuff == 0 ? p_imgbuff1 : p_imgbuff2);//交替初始化图像地址
}
/*!
*  @brief      UART3中断服务函数
*  @since      v5.0
*/
void uart3_handler(void)
{
	char ch;

	if (uart_query(UART3) == 1)   //接收数据寄存器满
	{
		//用户需要处理接收数据
		uart_getchar(UART3, &ch);                    //无限等待接受1个字节
		//处理函数  
		get_point_uart(ch);
		//g_ball_x = 1;
	}
}

void get_point_uart(unsigned char  c)
{
	if (head_1 == 0)
	{

		if (c == 0xFF)
		{

			head_1 = 1;

		}
	}
	else
	{
		if (head_2 == 0)
		{
			if (c == 0xEE)
			{
				head_2 = 1;
				//g_ball_x = c;
			}
		}
		else
		{
			if (head_3 == 0)
			{
				g_ball_x = c;
				head_3 = 1;
			}
			else
			{
				g_ball_y = c;
				head_1 = 0;
				head_2 = 0;
				head_3 = 0;
			}
		}
	}
}