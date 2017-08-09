/*
				 ██████╗ █████╗ ██╗   ██╗ ██████╗                   
				██╔════╝██╔══██╗██║   ██║██╔════╝                   
				██║     ███████║██║   ██║██║                        
				██║     ██╔══██║██║   ██║██║                        
				╚██████╗██║  ██║╚██████╔╝╚██████╗                   
				 ╚═════╝╚═╝  ╚═╝ ╚═════╝  ╚═════╝                   
																	
*/ 
/*!
* @file       moror_control.c
* @brief      电机控制
* @author     JassyLiu
* @date       2016-12-05
* @revision   
* @note 
*/

#include "common.h"
#include "include.h"
#include "motor_control.h"

int16 LeftMotorPulseSigma = 0;          //左边电机计数信号
int16 RightMotorPulseSigma = 0;         //右边电机计数信号

/*!
*  @brief     
*  @param     
*  @date   2016-12-05
*  @revision
*  @note
*/
/*!
*  @brief  初始化PWM通道
*  @date   2016-12-05
*  @revision
*  @note   占空比精度在ftm.h里修改
*/
void Motor_PWM_init(void)
{
	/* 初始化左电机PWM通道 */
	ftm_pwm_init(FTM3, FTM_CH0, 10 * 1000, 0);   
	ftm_pwm_init(FTM3, FTM_CH1, 10 * 1000, 0);       
	/* 初始化右电机PWM通道 */
	ftm_pwm_init(FTM3, FTM_CH2, 10 * 1000, 0);   
	ftm_pwm_init(FTM3, FTM_CH3, 10 * 1000, 0);    
}

/*!
*  @brief  右边电机控制
*  @param  int32 speed 占空比设置
*  @date   2016-12-05
*  @revision 
*  @note
*/
void Right_Motor_Control(int32 speed)
{
	if (speed > Motor_speed_Max)           //限幅处理
		speed = Motor_speed_Max;
	if (speed < Motor_speed_Min)
		speed = Motor_speed_Min;

	if (speed >= 0)                        //方向控制
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
*  @brief  左边电机控制
*  @param  int32 speed 占空比设置
*  @date   2016-12-05
*  @revision
*  @note
*/
void Left_Motor_Control(int32 speed)
{
	if (speed > Motor_speed_Max)           //限幅处理
		speed = Motor_speed_Max;
	if (speed < Motor_speed_Min)
		speed = Motor_speed_Min;

	if (speed >= 0)                        //方向控制
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
*  @brief  正交解码初始化
*  @param  
*  @date   2016-12-05
*  @revision
*  @note
*/
void Encoder_Init(void)
{
	ftm_quad_init(FTM1);		//PTA12 PTA13
	ftm_quad_init(FTM2);		//PTA10 PTA11
	port_init_NoALT(PTA12, PF | PULLUP);//内部配置上拉
	port_init_NoALT(PTA13, PF | PULLUP);
	port_init_NoALT(PTA10, PF | PULLUP);
	port_init_NoALT(PTA11, PF | PULLUP);
}

/*!
*  @brief  获取正交解码脉冲数 
*  @param 
*  @date   2016-12-05
*  @revision
*  @note  参数记录在全局变量 LeftMotorPulseSigma&RightMotorPulseSigma
*/
void GetMotorPulse(void)
{

	LeftMotorPulseSigma = 0-ftm_quad_get(FTM2);  //获取FTM 正交解码 的脉冲数(负数表示反方向)
	ftm_quad_clean(FTM2);
	RightMotorPulseSigma = ftm_quad_get(FTM1); //获取FTM 正交解码 的脉冲数(负数表示反方向)
	ftm_quad_clean(FTM1);
}

/*------------------------------------------
声明变量
------------------------------------------*/
extern M1TypeDef  M1;
extern M2TypeDef  M2;

PIDTypdDef M1PID;
PIDTypdDef M2PID;
/*------------------------------------------
函数功能:初始化M1PID结构体参数
函数说明:
------------------------------------------*/
void PID_M1_Init(void)
{
	M1PID.LastError = 0;			//Error[-1]
	M1PID.PrevError = 0;			//Error[-2]
	M1PID.Proportion = 0;			//比例常数 Proportional Const
	M1PID.Integral = 0;			//积分常数 Integral Const
	M1PID.Derivative = 0;			//微分常数 Derivative Const
	M1PID.SetPoint = 0;
	M1PID.SumError = 0;

}
/*------------------------------------------
函数功能:初始化M2PID结构体参数
函数说明:
------------------------------------------*/
void PID_M2_Init(void)
{
	M2PID.LastError = 0;			//Error[-1]
	M2PID.PrevError = 0;			//Error[-2]
	M2PID.Proportion = 0;			//比例常数 Proportional Const
	M2PID.Integral = 0;			//积分常数 Integral Const
	M2PID.Derivative = 0;			//微分常数 Derivative Const
	M2PID.SetPoint = 0;
	M2PID.SumError = 0;

}

/*------------------------------------------
函数功能:电机1位置式PID计算
函数说明:
------------------------------------------*/
int32_t PID_M1_PosLocCalc(float NextPoint)
{
	register float  iError, dError;

	iError = M1PID.SetPoint - NextPoint;        // 偏差
	M1PID.SumError += iError;				    // 积分
	if (M1PID.SumError > 2300.0)					//积分限幅
		M1PID.SumError = 2300.0;
	else if (M1PID.SumError < -2300.0)
		M1PID.SumError = -2300.0;
	dError = iError - M1PID.LastError; 			// 当前微分
	M1PID.LastError = iError;

	return(int32_t)(M1PID.Proportion * iError           	// 比例项
		+ M1PID.Integral   * M1PID.SumError 		// 积分项
		+ M1PID.Derivative * dError);
}


/*------------------------------------------
函数功能:电机2位置式PID计算
函数说明:
------------------------------------------*/
int32_t PID_M2_PosLocCalc(float NextPoint)
{
	register float  iError, dError;

	iError = M2PID.SetPoint - NextPoint;        // 偏差
	M2PID.SumError += iError;
	if (M2PID.SumError > 2300.0)					//积分限幅
		M2PID.SumError = 2300.0;
	else if (M2PID.SumError < -2300.0)
		M2PID.SumError = -2300.0;
	dError = iError - M2PID.LastError; 			// 当前微分
	M2PID.LastError = iError;

	return(int32_t)(M2PID.Proportion * iError           	// 比例项
		+ M2PID.Integral   * M2PID.SumError 		// 积分项
		+ M2PID.Derivative * dError);
}

