#include "sys.h"
#include "control.h"
#include "timer.h"
#include "math.h"
int runFlag = 0;
int pos_X = 0, pos_Y = 0;
int offset_X = 740, offset_Y = 910;//+-12
float KpX = -0.8, KiX = -0, KdX = -6;//-0.6  0   -9
float outputX,setX,setY;

float KpY = 0.6, KiY = 0, KdY = 5;//0.3  0  8
float outputY;
int lastx, lasty;
float pwmx=0, pwmy=0,pwmx_set=0,pwmy_set=0;
float n = 0;
float pid_X(float Setnum)
{
	float err = Setnum - pos_X;
	static float err_last = 0, pid_I = 0;
	outputX = err*KpX + pid_I + (err - err_last)*KdX;

	if (pid_I > 100)pid_I = 100;
	if (pid_I < -100)pid_I = -100;
	if (err*err > 100 && err*err < 3600)pid_I += err*KiX;
	else
	{
		pid_I = 0;
	}
	err_last = err;
	if (outputX > 150)outputX = 150;
	if (outputX < -150)outputX = -150;
	return outputX;
}

float pid_Y(float Setnum)
{
	float err = Setnum - pos_Y;
	static float err_last = 0, pid_I = 0;
	outputY = err*KpY + pid_I + (err - err_last)*KdY;
	
	if (pid_I > 100)pid_I = 100;
	if (pid_I < -100)pid_I = -100;
	if (err*err > 100&&err*err<3600)pid_I += err*KiY;
	else 
	{
		pid_I = 0;
	}
	err_last = err;
	if (outputY > 150)outputY = 150;
	if (outputY < -150)outputY = -150;
	return outputY;
}

void program_main()
{
	setX=sin(n)*30+145;
	setY = sin(n/2) * 80+160;
	//setY=160;
	setX=145;
	setY=160;
	pwmx_set = offset_X + pid_X(setX)*runFlag ;
	pwmy_set = offset_Y + pid_Y(setY)*runFlag;
	lastx = AngleX;
	lasty = AngleY;
//	AngleX = offset_X + pid_X(setX)*runFlag;
//	AngleY = offset_Y + pid_Y(setY)*runFlag;
	n += 0.2;	
}

void motorRun(int x, int y)
{
	static int countx=0, county=0;
	if (countx < 20)
	{
		pwmx += (x - lastx) / 25;
	}
	else
	{
		lastx = x;
		countx = 0;
	}

	if (county < 20)
	{
		pwmy += (y - lasty) / 25;
	}
	else
	{
		lasty = y;
		county = 0;
	}
	AngleX = pwmx;
	AngleY = pwmy;
}