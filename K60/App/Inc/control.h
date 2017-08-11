#ifndef CONTROL_H
#define CONTROL_H


typedef struct
{
	int x;
	int y;
}location;
extern location pos_set[5][5],pos_ball;
extern location pos_set_ID;
extern location  pos_now_ID;
extern location  pos_out_ID;
extern location pos_set_IDs[4];
extern location SelectTarget(location pos_begin, location pos_next);
extern void Correct_Mid(int nowMidX, int nowMidY);
extern int CheckLocation(location SetPosID);
extern void Task_Confirmed_Target(int taskMode);
extern void Task_Changed_Target();
extern void Task_Move_Around();
extern void Task_Advance();
extern void Task_program(int taskMode);
extern int timeCount;
#endif
