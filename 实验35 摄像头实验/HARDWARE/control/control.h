#ifndef CONTROL_H
#define CONTROL_H
#define AngleX TIM2->CCR3 
#define AngleY TIM2->CCR4

typedef struct
{
	int x;
	int y;
}position;
extern position pos_set[5][5], pos_add[2][2],pos_set_ID,pos_now_ID;

position SelectTarget(position pos_begin, position pos_next);
extern int pos_X, pos_Y, offset_X, offset_Y, runFlag, g_ball_x, g_ball_y;
extern float outputX,setX,setY, pwmx, pwmy , pwmx_set , pwmy_set,lastx, lasty;
 void program_main();
 float pid_X(float Setnum);
 void motorRun(int x, int y);
#endif
