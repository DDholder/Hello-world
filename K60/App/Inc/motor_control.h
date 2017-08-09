#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_
/*---------------电机PWM限幅控制-------------------*/
#define Motor_speed_Max ( 9800)
#define Motor_speed_Min (-9800)
/*-------------------------------------------------*/
extern int16 LeftMotorPulseSigma;          //左边电机计数信号
extern int16 RightMotorPulseSigma;         //右边电机计数信号
/*-----------------电机控制接口函数区域-------------*/
extern void  Motor_PWM_init(void);
extern void  Right_Motor_Control(int32 speed);
extern void  Left_Motor_Control(int32 speed);
/*-----------------编码器控制接口函数区域-------------*/
extern void Encoder_Init(void);
extern void GetMotorPulse(void);
/*-------------------------------------------------*/

/*------------------------------------------
PID结构体
------------------------------------------*/
typedef struct
{
	float  SetPoint; 	//  设定目标 Desired Value 
	double  SumError;		//	误差累计 

	float  Proportion;      //  比例常数 Proportional Const 
	float  Integral;        //  积分常数 Integral Const
	float  Derivative;      //  微分常数 Derivative Const

	float LastError;     //  Error[-1]
	float PrevError;     //  Error[-2]

}PIDTypdDef;

/*------------------------------------------
电机结构体
------------------------------------------*/
typedef struct
{
	float Offset;	  //允许偏差量
	float CurPos;
	float PrevPos;
	float CurAcc;
	float PrevSpeed;

	volatile float SetXPos;	  //设定位置
	volatile float SetYPos;	  //设定位置
	volatile float SetSpeed;  //设定速度

	volatile float CurXPos;	  //当前位置
	volatile float CurYPos;	  //当前位置
	volatile float CurSpeed;  //当前速度矢量

	volatile int32_t  PWM;	      //PWM
	volatile uint8_t  ShootFlag;
	volatile uint8_t  AdjustFlag;
	volatile uint8_t  ErrFlag;

	volatile uint32_t SetMaxPos;	  //软件设定最大位置
	volatile uint32_t SetMaxPower;	  //软件设定最大力量
	volatile int32_t  SetMaxSpeed;	  //软件设定最大速度

}M1TypeDef, M2TypeDef;


int32_t  PID_M1_PosLocCalc(float NextPoint);

void     PID_M1_Init(void);


int32_t  PID_M2_PosLocCalc(float NextPoint);
void     PID_M2_Init(void);

extern PIDTypdDef M1PID;
extern PIDTypdDef M2PID;


#endif