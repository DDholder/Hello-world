/*
                 �������������[ �����������[ �����[   �����[ �������������[                   
                �����X�T�T�T�T�a�����X�T�T�����[�����U   �����U�����X�T�T�T�T�a                   
                �����U     ���������������U�����U   �����U�����U                        
                �����U     �����X�T�T�����U�����U   �����U�����U                        
                �^�������������[�����U  �����U�^�������������X�a�^�������������[                   
                 �^�T�T�T�T�T�a�^�T�a  �^�T�a �^�T�T�T�T�T�a  �^�T�T�T�T�T�a                   

*/ 
/*!
* @file       parameter.c
* @brief      ȫ�ֲ���
* @author     JassyLiu
* @date       2016-12-06
* @revision
* @note
*/
#include "common.h"
#include "include.h"
#include "parameter.h"
uint8 SD_Record_Buff[512];
uint8 SD_Send_Buff[512];
//uint16  Block_Index = 0;
uint16  Block_Index_send = 0;
extern uint8 imgbuff[CAMERA_SIZE];
//int fun(int k)
//{
//	int sum = 0, cnt = 0, j;
//	while ((k >= 12) && cnt < 10)
//	{
//		if ((k % 13 == 0) || (k % 17 == 0))
//		{
//			sum = sum + k; cnt++;
//		}
//		k--;
//	}
//	return sum;
//}
/****************ȫ�ֱ�������******************/
uint8 Flag_SpeedControl = 0;
uint8 Flag_Stop = ON;			//=OFFͣ��
uint8 Flag_Speed = ON;			//=ON���ٶȿ���
uint8 Flag_Direction = ON;		//=ON�ӷ������
uint8 Flag_Out = 2;				//����ʾ������ʾ��־
uint8 Flag_Buzz = OFF;			//=OFF����������
/**************************����ͷ���ս�ѹ����******************************/
uint8  imgbuff1[CAMERA_SIZE];         //�ɼ�ͼ������1
uint8  imgbuff2[CAMERA_SIZE];         //�ɼ�ͼ������2
uint8 *  p_imgbuff1 = (uint8 *)((uint8 *)&imgbuff1);   //ͼ���ַ1
uint8 *  p_imgbuff2 = (uint8 *)((uint8 *)&imgbuff2);     //ͼ���ַ2
uint8  flag_imgbuff = 0;
uint8  img[CAMERA_H][CAMERA_W];							//��ѹ֮��ͼ��

uint8  bmp_buff[1024] = { 0 };
uint8  compress_buff[120][128] = { 0 };
void parameter_init(void)
{
	load_config();      //����������
	angle_offset = my_cnf[0].f;
	g_upstandControl_P = my_cnf[1].f;
	g_upstandControl_D = my_cnf[2].f;
	g_speedControl_P   = my_cnf[3].f;
	g_fExpectSpeed	   = my_cnf[4].f;
	g_dirControl_P = my_cnf[5].f;
	g_dirControl_D = my_cnf[6].f;
        g_dirControl_gyro_D = my_cnf[7].f;
		x_x = (int)my_cnf[8].f;
		x_y = (int)my_cnf[9].f;
		M1PID.Proportion = my_cnf[10].f;
		M1PID.Integral = my_cnf[11].f;
		M1PID.Derivative = my_cnf[12].f;
		M2PID.Proportion = my_cnf[13].f;
		M2PID.Integral = my_cnf[14].f;
		M2PID.Derivative = my_cnf[15].f;
		servo_offset1 = my_cnf[16].f;
		servo_offset2 = my_cnf[17].f;
		offset_image[0] = my_cnf[18].f;
			offset_image[1] = my_cnf[19].f ;
			offset_image[2] = my_cnf[20].f ;
			offset_image[3] = my_cnf[21].f ;
			offset_image[4] = my_cnf[22].f ;
			offset_image[5] = my_cnf[23].f ;
			offset_image[6] = my_cnf[24].f ;
			offset_image[7] = my_cnf[25].f ;
			offset_image[8] = my_cnf[26].f ;
			offset_image[9] = my_cnf[27].f ;
			offset_image[10] = my_cnf[28].f ;
			offset_image[11] = my_cnf[29].f ;
			offset_image[12] = my_cnf[30].f ;
			offset_image[13] = my_cnf[31].f ;
			offset_image[14] = my_cnf[32].f ;
			offset_image[15] = my_cnf[33].f ;
			offset_image[16] = my_cnf[34].f ;
			offset_image[17] = my_cnf[35].f ;

			pos_set[0][0].x = offset_image[0];
			pos_set[0][0].y = offset_image[1];
			pos_set[0][2].x = offset_image[2];
			pos_set[0][2].y = offset_image[3];
			pos_set[0][4].x = offset_image[4];
			pos_set[0][4].y = offset_image[5];
			pos_set[2][0].x = offset_image[6];
			pos_set[2][0].y = offset_image[7];
			pos_set[2][2].x = offset_image[8];
			pos_set[2][2].y = offset_image[9];
			pos_set[2][4].x = offset_image[10];
			pos_set[2][4].y = offset_image[11];
			pos_set[4][0].x = offset_image[12];
			pos_set[4][0].y = offset_image[13];
			pos_set[4][2].x = offset_image[14];
			pos_set[4][2].y = offset_image[15];
			pos_set[4][4].x = offset_image[16];
			pos_set[4][4].y = offset_image[17];
//	String_Angle_D = my_cnf[7].f;
//	String_Gyro_P = my_cnf[8].f;
//	String_Gyro_I = my_cnf[9].f;
//	String_Gyro_D = my_cnf[10].f;
//	SPEED_CONTROL_P = my_cnf[11].f;
//	SPEED_CONTROL_I = my_cnf[12].f;
//	SPEED_CONTROL_D = my_cnf[13].f;
//	DIRECTION_CONTROL_P = my_cnf[14].f;
//	DIRECTION_CONTROL_D = my_cnf[15].f;
//	Begin_CarSpeed = my_cnf[16].f;
//	DIRECTION_CONTROL_DD = my_cnf[17].f;
//	DIRECTION_CONTROL_P_L = my_cnf[18].f;
//	DIRECTION_CONTROL_D_L = my_cnf[19].f;
}

void sdcard_record(void)
{
	//float temp;
	unsigned int i=0;
	unsigned int j=0;
	//unsigned int data_num = 600;
	SD_Record_Buff[j++] = 0x11;
	SD_Record_Buff[j++] = 0x22;
	SD_Record_Buff[j++] = 0x33;
	//SD_Record_Buff[j++] = data_num;
	for (i = 0;i < 300; i++)
	{
		//temp = data_num;
		//SD_Record_Buff[j++] = BYTE0(temp);
		//SD_Record_Buff[j++] = BYTE1(temp);
		//SD_Record_Buff[j++] = BYTE2(temp);
		//SD_Record_Buff[j++] = BYTE3(temp);
		SD_Record_Buff[j++] = 2;//imgbuff[i];
	}
	SD_Record_Buff[j++] = 0xdd;
	SD_Record_Buff[j++] = 0xbb;
	SD_Record_Buff[j++] = 0xcc;
	SD_WriteDisk(SD_Record_Buff, BLOCK_BEGIN + Block_Index);
	Block_Index++;

	j = 0;
	SD_Record_Buff[j++] = 0x44;
	SD_Record_Buff[j++] = 0x77;
	SD_Record_Buff[j++] = 0x66;
	for (i = 300;i < 600; i++)
	{
		//temp = data_num;
		//SD_Record_Buff[j++] = BYTE0(temp);
		//SD_Record_Buff[j++] = BYTE1(temp);
		//SD_Record_Buff[j++] = BYTE2(temp);
		//SD_Record_Buff[j++] = BYTE3(temp);
		SD_Record_Buff[j++] = 4;//imgbuff[i];
	}
	SD_Record_Buff[j++] = 0xaa;
	SD_Record_Buff[j++] = 0xbb;
	SD_Record_Buff[j++] = 0xcc;
	SD_WriteDisk(SD_Record_Buff, BLOCK_BEGIN + Block_Index);
	Block_Index++;

}

void sdcard_send(void)
{
	int i;
	SD_ReadDisk(SD_Send_Buff, Block_Index + BLOCK_BEGIN-1);
	for (i = 0;i < 512;i++)
	{
		uart_putchar(UART1, SD_Send_Buff[i]);
		if (i >= 3)
		{
			if (SD_Send_Buff[i - 2] == 0xaa)
			{
				if (SD_Send_Buff[i - 1] == 0xbb)
				{
					if (SD_Send_Buff[i] == 0xcc)
					{
						break; //�����������Ч��û��Ҫ����
					}
				}
			}
		}
	}

	SD_ReadDisk(SD_Send_Buff, Block_Index_send + BLOCK_BEGIN-1);
	for (i = 0;i < 512;i++)
	{
		uart_putchar(UART1, SD_Send_Buff[i]);
		if (i >= 3)
		{
			if (SD_Send_Buff[i - 2] == 0xaa)
			{
				if (SD_Send_Buff[i - 1] == 0xbb)
				{
					if (SD_Send_Buff[i] == 0xcc)
					{
						break; //�����������Ч��û��Ҫ����
					}
				}
			}
		}
	}
	Block_Index_send -= 2;
}