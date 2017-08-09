/*
				 �������������[ �����������[ �����[   �����[ �������������[                   
				�����X�T�T�T�T�a�����X�T�T�����[�����U   �����U�����X�T�T�T�T�a                   
				�����U     ���������������U�����U   �����U�����U                        
				�����U     �����X�T�T�����U�����U   �����U�����U                        
				�^�������������[�����U  �����U�^�������������X�a�^�������������[                   
				 �^�T�T�T�T�T�a�^�T�a  �^�T�a �^�T�T�T�T�T�a  �^�T�T�T�T�T�a                   
																	
*/ 
/*!
* @file       moror_control.c
* @brief      �������
* @author     JassyLiu
* @date       2016-12-05
* @revision   
* @note 
*/

#include "common.h"
#include "include.h"
#include "motor_control.h"

int16 LeftMotorPulseSigma = 0;          //��ߵ�������ź�
int16 RightMotorPulseSigma = 0;         //�ұߵ�������ź�

/*!
*  @brief     
*  @param     
*  @date   2016-12-05
*  @revision
*  @note
*/
/*!
*  @brief  ��ʼ��PWMͨ��
*  @date   2016-12-05
*  @revision
*  @note   ռ�ձȾ�����ftm.h���޸�
*/
void Motor_PWM_init(void)
{
	/* ��ʼ������PWMͨ�� */
	ftm_pwm_init(FTM3, FTM_CH0, 10 * 1000, 0);   
	ftm_pwm_init(FTM3, FTM_CH1, 10 * 1000, 0);       
	/* ��ʼ���ҵ��PWMͨ�� */
	ftm_pwm_init(FTM3, FTM_CH2, 10 * 1000, 0);   
	ftm_pwm_init(FTM3, FTM_CH3, 10 * 1000, 0);    
}

/*!
*  @brief  �ұߵ������
*  @param  int32 speed ռ�ձ�����
*  @date   2016-12-05
*  @revision 
*  @note
*/
void Right_Motor_Control(int32 speed)
{
	if (speed > Motor_speed_Max)           //�޷�����
		speed = Motor_speed_Max;
	if (speed < Motor_speed_Min)
		speed = Motor_speed_Min;

	if (speed >= 0)                        //�������
	{
		ftm_pwm_duty(FTM3, FTM_CH0, 0);
		ftm_pwm_duty(FTM3, FTM_CH1, speed);
	}
	if (speed < 0)
	{
		ftm_pwm_duty(FTM3, FTM_CH0, (-speed));
		ftm_pwm_duty(FTM3, FTM_CH1, 0);
	}
}

/*!
*  @brief  ��ߵ������
*  @param  int32 speed ռ�ձ�����
*  @date   2016-12-05
*  @revision
*  @note
*/
void Left_Motor_Control(int32 speed)
{
	if (speed > Motor_speed_Max)           //�޷�����
		speed = Motor_speed_Max;
	if (speed < Motor_speed_Min)
		speed = Motor_speed_Min;

	if (speed >= 0)                        //�������
	{
		ftm_pwm_duty(FTM3, FTM_CH2, 0);
		ftm_pwm_duty(FTM3, FTM_CH3, speed);
	}
	if (speed < 0)
	{
		ftm_pwm_duty(FTM3, FTM_CH2, (-speed));
		ftm_pwm_duty(FTM3, FTM_CH3, 0);
	}
}

/*!
*  @brief  ���������ʼ��
*  @param  
*  @date   2016-12-05
*  @revision
*  @note
*/
void Encoder_Init(void)
{
	ftm_quad_init(FTM1);		//PTA12 PTA13
	ftm_quad_init(FTM2);		//PTA10 PTA11
	port_init_NoALT(PTA12, PF | PULLUP);//�ڲ���������
	port_init_NoALT(PTA13, PF | PULLUP);
	port_init_NoALT(PTA10, PF | PULLUP);
	port_init_NoALT(PTA11, PF | PULLUP);
}

/*!
*  @brief  ��ȡ�������������� 
*  @param 
*  @date   2016-12-05
*  @revision
*  @note  ������¼��ȫ�ֱ��� LeftMotorPulseSigma&RightMotorPulseSigma
*/
void GetMotorPulse(void)
{

	LeftMotorPulseSigma = 0-ftm_quad_get(FTM2);  //��ȡFTM �������� ��������(������ʾ������)
	ftm_quad_clean(FTM2);
	RightMotorPulseSigma = ftm_quad_get(FTM1); //��ȡFTM �������� ��������(������ʾ������)
	ftm_quad_clean(FTM1);
}

/*------------------------------------------
��������
------------------------------------------*/
extern M1TypeDef  M1;
extern M2TypeDef  M2;

PIDTypdDef M1PID;
PIDTypdDef M2PID;
/*------------------------------------------
��������:��ʼ��M1PID�ṹ�����
����˵��:
------------------------------------------*/
void PID_M1_Init(void)
{
	M1PID.LastError = 0;			//Error[-1]
	M1PID.PrevError = 0;			//Error[-2]
	M1PID.Proportion = 0;			//�������� Proportional Const
	M1PID.Integral = 0;			//���ֳ��� Integral Const
	M1PID.Derivative = 0;			//΢�ֳ��� Derivative Const
	M1PID.SetPoint = 0;
	M1PID.SumError = 0;

}
/*------------------------------------------
��������:��ʼ��M2PID�ṹ�����
����˵��:
------------------------------------------*/
void PID_M2_Init(void)
{
	M2PID.LastError = 0;			//Error[-1]
	M2PID.PrevError = 0;			//Error[-2]
	M2PID.Proportion = 0;			//�������� Proportional Const
	M2PID.Integral = 0;			//���ֳ��� Integral Const
	M2PID.Derivative = 0;			//΢�ֳ��� Derivative Const
	M2PID.SetPoint = 0;
	M2PID.SumError = 0;

}

/*------------------------------------------
��������:���1λ��ʽPID����
����˵��:
------------------------------------------*/
int32_t PID_M1_PosLocCalc(float NextPoint)
{
	register float  iError, dError;

	iError = M1PID.SetPoint - NextPoint;        // ƫ��
	M1PID.SumError += iError;				    // ����
	if (M1PID.SumError > 2300.0)					//�����޷�
		M1PID.SumError = 2300.0;
	else if (M1PID.SumError < -2300.0)
		M1PID.SumError = -2300.0;
	dError = iError - M1PID.LastError; 			// ��ǰ΢��
	M1PID.LastError = iError;

	return(int32_t)(M1PID.Proportion * iError           	// ������
		+ M1PID.Integral   * M1PID.SumError 		// ������
		+ M1PID.Derivative * dError);
}


/*------------------------------------------
��������:���2λ��ʽPID����
����˵��:
------------------------------------------*/
int32_t PID_M2_PosLocCalc(float NextPoint)
{
	register float  iError, dError;

	iError = M2PID.SetPoint - NextPoint;        // ƫ��
	M2PID.SumError += iError;
	if (M2PID.SumError > 2300.0)					//�����޷�
		M2PID.SumError = 2300.0;
	else if (M2PID.SumError < -2300.0)
		M2PID.SumError = -2300.0;
	dError = iError - M2PID.LastError; 			// ��ǰ΢��
	M2PID.LastError = iError;

	return(int32_t)(M2PID.Proportion * iError           	// ������
		+ M2PID.Integral   * M2PID.SumError 		// ������
		+ M2PID.Derivative * dError);
}

