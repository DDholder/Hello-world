#include "sys.h"
#include "control.h"
#include "timer.h"
#include "math.h"
position pos_ball;
position pos_set[5][5];
position pos_add[2][2];
position pos_set_ID = { 2,4 }, pos_now_ID = {0,0};
position SelectTarget(position pos_begin, position pos_next)
{
	position posvalue = pos_next;
	if ((pos_begin.x - pos_next.x)*(pos_begin.x - pos_next.x) <= 4)
	{
		if ((pos_begin.y - pos_next.y)*(pos_begin.y - pos_next.y) <= 4)
		{
			return pos_next;
		}
	}
if (pos_begin.x - pos_next.x > 2)
	{
		posvalue.x = pos_begin.x == 3 ? 1 : 3;
		posvalue.y = pos_begin.y == 2 ? pos_next.y < 2 ? 1 : 3 : pos_begin.y < 2 ? 1 : 3;
		return posvalue;
	}
	else if (pos_begin.x - pos_next.x < -2)
	{
		posvalue.x = pos_begin.x == 1 ? 3 : 1;
		posvalue.y = pos_begin.y == 2 ? pos_next.y < 2 ? 1 : 3 : pos_begin.y < 2 ? 1 : 3;
		return posvalue;
	}
	if (pos_begin.y - pos_next.y > 2)
	{
		posvalue.y = pos_begin.y == 3 ? 1 : 3;

		posvalue.x = pos_begin.x == 2 ? pos_next.x < 2 ? 1 : 3 : pos_begin.x < 2 ? 1 : 3;
	}
	else if (pos_begin.y - pos_next.y < -2)
	{
		posvalue.y = pos_begin.y == 1 ? 3 : 1;

		posvalue.x = pos_begin.x == 2 ? pos_next.x < 2 ? 1 : 3 : pos_begin.x < 2 ? 1 : 3;
	}

	return posvalue;
}


int runFlag = 0;
int pos_X = 0, pos_Y = 0;
int offset_X = 740, offset_Y = 910;//+-12
float KpX = -0.8, KiX = -0, KdX = -6;//-0.6  0   -9
float outputX,setX,setY;

float KpY = 0.6, KiY = 0, KdY = 5;//0.3  0  8
float outputY;
float lastx=0, lasty=0;
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
	n += 0.2;	
}

void motorRun(int x, int y)
{
	static int countx=0, county=0;
	if (countx < 25)
	{
		pwmx += (x - lastx) / 25;
		countx++;
	}
	else
	{
			lastx=pwmx;
		countx = 0;
	}

	if (county < 25)
	{
		pwmy += (y - lasty) / 25;
		county++;
	}
	else
	{
			lasty=pwmy;
		county = 0;
	}
	AngleX = pwmx;
	AngleY = pwmy;
}