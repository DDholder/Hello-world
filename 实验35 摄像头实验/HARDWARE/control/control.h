#ifndef CONTROL_H
#define CONTROL_H
#define AngleX TIM2->CCR3 
#define AngleY TIM2->CCR4
extern int pos_X , pos_Y , offset_X , offset_Y ,runFlag;
extern float outputX,setX,setY, pwmx, pwmy , pwmx_set , pwmy_set;
 void program_main();
 float pid_X(float Setnum);
 void motorRun(int x, int y);
#endif
