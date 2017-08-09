#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_
/*---------------���PWM�޷�����-------------------*/
#define Motor_speed_Max ( 9800)
#define Motor_speed_Min (-9800)
/*-------------------------------------------------*/
extern int16 LeftMotorPulseSigma;          //��ߵ�������ź�
extern int16 RightMotorPulseSigma;         //�ұߵ�������ź�
/*-----------------������ƽӿں�������-------------*/
extern void  Motor_PWM_init(void);
extern void  Right_Motor_Control(int32 speed);
extern void  Left_Motor_Control(int32 speed);
/*-----------------���������ƽӿں�������-------------*/
extern void Encoder_Init(void);
extern void GetMotorPulse(void);
/*-------------------------------------------------*/

/*------------------------------------------
PID�ṹ��
------------------------------------------*/
typedef struct
{
	float  SetPoint; 	//  �趨Ŀ�� Desired Value 
	double  SumError;		//	����ۼ� 

	float  Proportion;      //  �������� Proportional Const 
	float  Integral;        //  ���ֳ��� Integral Const
	float  Derivative;      //  ΢�ֳ��� Derivative Const

	float LastError;     //  Error[-1]
	float PrevError;     //  Error[-2]

}PIDTypdDef;

/*------------------------------------------
����ṹ��
------------------------------------------*/
typedef struct
{
	float Offset;	  //����ƫ����
	float CurPos;
	float PrevPos;
	float CurAcc;
	float PrevSpeed;

	volatile float SetXPos;	  //�趨λ��
	volatile float SetYPos;	  //�趨λ��
	volatile float SetSpeed;  //�趨�ٶ�

	volatile float CurXPos;	  //��ǰλ��
	volatile float CurYPos;	  //��ǰλ��
	volatile float CurSpeed;  //��ǰ�ٶ�ʸ��

	volatile int32_t  PWM;	      //PWM
	volatile uint8_t  ShootFlag;
	volatile uint8_t  AdjustFlag;
	volatile uint8_t  ErrFlag;

	volatile uint32_t SetMaxPos;	  //����趨���λ��
	volatile uint32_t SetMaxPower;	  //����趨�������
	volatile int32_t  SetMaxSpeed;	  //����趨����ٶ�

}M1TypeDef, M2TypeDef;


int32_t  PID_M1_PosLocCalc(float NextPoint);

void     PID_M1_Init(void);


int32_t  PID_M2_PosLocCalc(float NextPoint);
void     PID_M2_Init(void);

extern PIDTypdDef M1PID;
extern PIDTypdDef M2PID;


#endif