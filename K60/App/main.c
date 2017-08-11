/*
				 �������������[ �����������[ �����[   �����[ �������������[
				�����X�T�T�T�T�a�����X�T�T�����[�����U   �����U�����X�T�T�T�T�a
				�����U     ���������������U�����U   �����U�����U
				�����U     �����X�T�T�����U�����U   �����U�����U
				�^�������������[�����U  �����U�^�������������X�a�^�������������[
				 �^�T�T�T�T�T�a�^�T�a  �^�T�a �^�T�T�T�T�T�a  �^�T�T�T�T�T�a
*/
/*!
 * @file       main.c
 * @brief      2017������ƴ���
 * @author     Jassy
 * @date       2017-8-4
 */
#include "common.h"
#include "include.h"
#include "../SOLGUI/SOLGUI_Menu.h"
#include "../SOLGUI/SOLGUI_Common.h"
#define SECTOR_NUM  (FLASH_SECTOR_NUM-1)         //������������������ȷ����ȫ 
#define Bet     ADC1_SE6a                        //�����ص�ѹ����AD����
#define FloatFillStr(x,y)   ((*(float *)&y)=x)   //������תchar������
#define StrFillFloat(x,y)   (y=(*(float *)&x))   //char������ת������ 
#define SW_1_carflag		PTE3	
#define SW_2_undef			PTE4
#define SW_3_OLEDreflash	PTE5
#define Test_IO				PTE9
#define Beep				PTC1
 //====================��������========================
 //�ڲ������ǵü�static 
 //ʹ�ö��Խ��а�ȫ��� 
void PIT0_IRQHandler(void);
void PIT1_IRQHandler(void);
void PORTA_IRQHandler(void);
void DMA0_IRQHandler(void);
void uart3_handler(void);
void get_point_uart(unsigned char  c);
//====================��������========================
//�ж����ñ����ǵü�volatile
//ֻ�������ǵü�constant 
//����ȫ�ֱ������� ��static���� ������ʿɹ��캯��
uint8 key_value = 0;		//��ֵ
uint8 imgbuff[CAMERA_SIZE]; //����洢����ͼ�������
float bet_value = 0;		//��ص�ѹ
float vcan_send_buff[8];	//ɽ����λ������ʾ����		  
char vcan_send_flag = 0;	//ɽ����λ�����ͱ�־λ
int beep_time = 0;			 //��������ʱ��
int beep_cnt = 0;			 //���������ʱ
KEY_MSG_t keymsg;
char  head_1 = 0;			//����֡ͷʶ��
char  head_2 = 0;
char  head_3 = 0;
char  head_4 = 0;
int g_ball_x = 0;
int g_ball_y = 0;
int point_y_set = 50;

int pwm_temp = 0;
int pwm_temp1 = 0;

int x_x = 0;
int x_y = 0;    //ʮ�ֲ�˿
int servo1_pwm = 0;
int servo2_pwm = 0;
extern int16 point[2];
int16 point_temp[2];
int16 point_temp1[2];
int time_cnt = 0;

int mode = 0;
//++++++++++++++++++++++++++++++++++++++++++++++++++++
/*!
 *  @brief      main����
 *  @note
 */
 //system_MK60DZ10.c main����֮ǰstart�����ڿ���Ӳ������
void main()
{
	DisableInterrupts; //�����ж�
	printf("Init......begin\n");
	printf("**************************\n");
	/*		���԰�����ʼ��
	*KEYӲ��������VCAN_key.c�ļ��޸�
	*ö������KEY_e
	*/
	printf("Key Init......");
	key_init(KEY_MAX);					 //��ʼ��ȫ������
	//gpio_init(SW_1_carflag, GPI, 1);	 
	//gpio_init(SW_2_undef  , GPI, 1);
	//gpio_init(SW_3_OLEDreflash, GPI, 1); //��ʼ�����뿪�� 
	gpio_init(Test_IO, GPO, 1);			 //����ʱ���� 
	printf("OK\n");
	printf("Car flag Read......");
	/*��ʼ����ѹ����ADC*/
	printf("ADC_bet Init......");
	adc_init(Bet);
	printf("OK\n");
	/*��������ʼ��*/
	printf("Beep Init......");
	gpio_init(Beep, GPO, 0); //��������
	printf("OK\n");
	/*OLED��ʼ��*/
	printf("OLED Init......");
	OLED_Init(); //��ʼ��OLED
	OLED_Fill(0x00); //����
	draw_cauc_logo();
	DELAY_MS(300);
	gpio_set(Beep, 1);
	DELAY_MS(500);
	printf("OK\n");
	/*UI�����ʼ��*/
	printf("UI Init......");
	SOLGUI_Init(&UI_List);		    //UI��ʼ������
	SOLGUI_Refresh();		    	//UIˢ�� 
	printf("OK\n");
	/*		FTMģ���ʼ��
	*Ƶ�� 10K ���� 1000
	*�����޸�  MK60_FTM.H -> FTM0_PRECISON
	*/
	printf("PWM Init......");
	MotorInit();
	printf("OK\n");
	/*		���I2C��ʼ��      */
	printf("I2C Init......");
	IMU_IIC_Init();
	printf("OK\n");
	/*		��̬��������ʼ��      */
	printf("8451&3050 Init......");
	//UpstandInit();
	printf("OK\n");
	/*       ���������ʼ��      */
	printf("Encoder Init......");
	Encoder_Init();
	printf("OK\n");
	/*       FLASH��ʼ��         */
	printf("Flash Init......");
	flash_init();					//��ʼ��flash
	parameter_init();			    //��ʼ������  �˴�װ�����е��粻��ʧ����  
	printf("OK\n");
	/*       ����ͷ��ʼ��        */
	printf("Camera Init......");
	//camera_init(p_imgbuff1);
	printf("OK\n");
	/*        ���ڳ�ʼ��        */
	printf("Uart Init......");
	uart_init(UART3, 115200);
	printf("OK\n");
	/*        �����ж�����       */
	printf("Interrupt Init......");
	NVIC_SetPriorityGrouping(4);		//�����жϷ���
	//set_irq_priority(DMA0_IRQn, 2);
	//set_irq_priority(PORTA_IRQn, 2);
	//NVIC_SetPriority(PIT0_VECTORn, 2);  //����1ms�жϸ����ȼ�
	set_irq_priority(PIT1_VECTORn, 3);    //����5ms�жϵ����ȼ�
	set_irq_priority(UART3_RX_TX_VECTORn, 2);

	//set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); 
	//set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler); 	
	set_vector_handler(UART3_RX_TX_VECTORn, uart3_handler);   // �����жϷ��������ж���������
	uart_rx_irq_en(UART3);                                    //�����ڽ����ж�
	/*    ��ʼ��PIT��ʱ���ж�    */
	pit_init_ms(PIT0, 5); //��ʼ��PIT0����ʱʱ��Ϊ�� 1ms
	set_vector_handler(PIT0_VECTORn, PIT0_IRQHandler); //����PIT0���жϷ�����Ϊ PIT0_IRQHandler
	enable_irq(PIT0_IRQn); //ʹ��PIT0�ж�  
	pit_init_ms(PIT1, 80);
	set_vector_handler(PIT1_VECTORn, PIT1_IRQHandler);
	enable_irq(PIT1_IRQn);
	ov7725_eagle_img_flag = IMG_START;      //��ʼ�ɼ�ͼ��
	enable_irq(PORTA_IRQn);                 //����PTA���ж�
	printf("OK\n");
	printf("**************************\n");
	printf("Init...Over\n");
	EnableInterrupts; //�����ж�
	while (1)
	{
		//����ɽ����λ������
		//if (vcan_send_flag == 1)
		//{
		vcan_sendware((uint8_t *)vcan_send_buff, sizeof(vcan_send_buff));
		//}
// 		ftm_pwm_duty(FTM3, FTM_CH0, 200);
// 		ftm_pwm_duty(FTM3, FTM_CH1, 200);
// 		ftm_pwm_duty(FTM3, FTM_CH2,pwm_temp );
// 		ftm_pwm_duty(FTM3, FTM_CH3, pwm_temp1);
		//��������	
// 		if (gpio_get(SW_3_OLEDreflash) == 0)
// 		{
// 			OLED_Init();
// 			OLED_Fill(0x00);
// 		}					
		
		bet_value = (float)(ad_ave(Bet, ADC_12bit, 10)) / 4096 * 3.3 * 3; //������ص�ѹ                                 
	}
}



/*!
*  @brief      PIT0�жϷ�����
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
	PIT_Flag_Clear(PIT0); //���жϱ�־λ
}

void PIT1_IRQHandler(void)
{
	key_value = key_scan();
	SOLGUI_InputKey(key_value);//����GUI 
	SOLGUI_Menu_PageStage();
	SOLGUI_Refresh(); //OLEDǰ̨ˢ��
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
	// ����ֵ��С�������У�ð�ݷ���
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
*  @brief      PORTA�жϷ�����
*  @since      v5.0
*/
int flagtemp = 1;
void PORTA_IRQHandler()
{
	uint8 n; //���ź�
	uint32 flag;

	while (!PORTA_ISFR);
	flag = PORTA_ISFR;
	PORTA_ISFR = ~0; //���жϱ�־λ

	n = 29; //���ж�
	if (flag & (1 << n)) //PTA29�����ж�
	{
		//gpio_set(Test_IO, 1);
		camera_vsync();   //��ʼ�ɼ�ͼ��(����ͼ���ַ)
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
		//��ѹ��һ��ͼ��													 800us
		//DirectionControl();		//����������							 1.8ms
		//GalAcceAndGyroY();		//�ɼ�������ٶȼƺ�������					 1ms
		//KalmanFilter();			//�������˲����ó��Ƕ� ʱ��				 40us
		//BalanceControl();		//ֱ���������
		//PWMOut();				//����PWM�������������εķ��������ֱ��������ϴε��ٶ�������ӣ�
// 			Flag_SpeedControl++;
// 			if (Flag_SpeedControl >= 20)
// 			{
// 				SpeedControl();		//�ٶȿ�������20*(1/150)s
// 				Flag_SpeedControl = 0;
// 			}
// 			SpeedControlOut();		//�ٶ�������ڣ�1/150��s��Ҳ����ÿ�ε��ٶȿ��Ʒ�20�����
// 			if(beep_time>0)
// 			{
// 				gpio_init(Beep, GPO, 0); //�������� 
// 				beep_time--;
// 			}
// 			else
// 			{
// 				gpio_init(Beep, GPO, 1); //�ط�����
// 				beep_time = 0;
// 			}
	}
}

/*!
*  @brief      DMA0�жϷ�����
*  @since      v5.0
*/
void DMA0_IRQHandler()
{
	camera_dma();
	camera_get_img();	//�ȴ�ͼ������ϣ���ɱ�־��Ϊ��ʼ��־��
	flag_imgbuff = !flag_imgbuff;
	ov7725_eagle_img_buff = (flag_imgbuff == 0 ? p_imgbuff1 : p_imgbuff2);//�����ʼ��ͼ���ַ
}
/*!
*  @brief      UART3�жϷ�����
*  @since      v5.0
*/
void uart3_handler(void)
{
	char ch;

	if (uart_query(UART3) == 1)   //�������ݼĴ�����
	{
		//�û���Ҫ�����������
		uart_getchar(UART3, &ch);                    //���޵ȴ�����1���ֽ�
		//������  
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