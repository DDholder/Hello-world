#include "control.h"
#include "common.h"
#include "include.h"
location pos_ball;
/*version for djh*/
// location pos_set[5][5] = {
// 	{ { 34,32 },{ 0,0 },{ 99,33 },{ 0,0 },{ 166,34 } },
// 	{ { 0,0 },{ 64,64 },{ 0,0 },{ 136,64 },{ 0,0 } },
// 	{ { 32,99 },{ 0,0 },{ 98,100 },{ 0,0 },{ 166,101 } },
// 	{ { 0,0 },{ 63,134 },{ 0,0 },{ 135,135 },{ 0,0 } },
// 	{ { 33,163 },{ 0,0 },{ 98,166 },{ 0,0 },{ 136,166 } },
// 
// };
location pos_set[5][5] = {
	{ { 32,32 },{ 0,0 },{ 98,32 },{ 0,0 },{ 164,33 } },
	{ { 0,0 },{ 62,134 },{ 0,0 },{ 61,60 },{ 0,0 } },
	{ { 31,98 },{ 0,0 },{ 97,100 },{ 0,0 },{ 164,100 } },
	{ { 0,0 },{ 133,134 },{ 0,0 },{ 135,62 },{ 0,0 } },
	{ { 31,162 },{ 0,0 },{ 96,165 },{ 0,0 },{ 161,164 } },
};
location pos_set_ID = { 4,2 }, pos_now_ID = { 0,0 }, pos_out_ID = { 0,0 };
location pos_set_IDs[4] = { {2,2},{0,4},{2,0},{4,4} };
int timeCount = 0;
int step = 0;
int taskMode_last;       //�ж�״̬�ı�

//���룺Ŀ���λ��ID������λ��ID
//������滮��λ��ID
//�Ե��������㼰��Χ4��Ŀ���Ϊһ��С����
//���ƶ����ƶ�С��������ƶ���Ŀ���
//x�������˶�
location SelectTarget(location pos_begin, location pos_next)
{
	location posvalue = pos_next;
	//����С�����ڣ�ֱ�ӵ�Ŀ���
	if (ABS(pos_begin.x - pos_next.x) <= 2)
		if (ABS(pos_begin.y - pos_next.y) <= 2)
			return pos_next;
	//��С����->��С����
	if (pos_begin.x - pos_next.x > 2)
	{
		//������x��λ
		posvalue.x = pos_begin.x == 3 ? 1 : 3;
		//������y��λ
		posvalue.y = pos_begin.y == 2 ? pos_next.y < 2 ? 1 : 3 : pos_begin.y < 2 ? 1 : 3;
		return posvalue;
	}
	//��С����->��С����
	else if (pos_begin.x - pos_next.x < -2)
	{
		//������x��λ
		posvalue.x = pos_begin.x == 1 ? 3 : 1;
		//������y��λ
		posvalue.y = pos_begin.y == 2 ? pos_next.y < 2 ? 1 : 3 : pos_begin.y < 2 ? 1 : 3;
		return posvalue;
	}
	//��С����->��С����
	if (pos_begin.y - pos_next.y > 2)
	{
		//������y��λ
		posvalue.y = pos_begin.y == 3 ? 1 : 3;
		//������x��λ
		posvalue.x = pos_begin.x == 2 ? pos_next.x < 2 ? 1 : 3 : pos_begin.x < 2 ? 1 : 3;
	}
	//��С����->��С����
	else if (pos_begin.y - pos_next.y < -2)
	{
		//������y��λ
		posvalue.y = pos_begin.y == 1 ? 3 : 1;
		//������x��λ
		posvalue.x = pos_begin.x == 2 ? pos_next.x < 2 ? 1 : 3 : pos_begin.x < 2 ? 1 : 3;
	}
	return posvalue;
}
void Correct_Mid(int nowMidX, int nowMidY)
{
	int i, j;
	int offsetx = nowMidX - pos_set[2][2].x, offsety = nowMidY - pos_set[2][2].y;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			pos_set[i][j].x += offsetx;
			pos_set[i][j].y += offsety;
		}
	}
}
//�ж��Ƿ񵽵�
//�����pos_ball
int CheckLocation(location SetPosID)
{

	if (ABS(pos_set[SetPosID.y][SetPosID.x].x - pos_ball.x)
		+ ABS(pos_set[SetPosID.y][SetPosID.x].y - pos_ball.y) < 12)
	{
		timeCount++;
	}
	else
	{
		timeCount = 0;
	}
	if (SetPosID.y % 2 != 0)
	{
		if (timeCount > 100)
		{
			timeCount = 0;
			return 1;
		}

	}
	else
	{
		if (timeCount > 500)			//3��ʱ�䵽
		{
			timeCount = 0;
			return 1;
		}
	}
	return 0;
}
//taskMode:
//			1:����1
//			2:����2
//			3:����3
//			4:����4
//			11:����1
void Task_Confirmed_Target(int taskMode)
{
	//static int step = 0;
	location pos_set_temp_IDs[4];
	//static int taskMode_last;       //�ж�״̬�ı�
// 	if (taskMode != taskMode_last)
// 	{
// 		step = 0;
// 	}
// 	taskMode_last = taskMode;
	switch (taskMode)
	{
	case 1:
		pos_set_temp_IDs[0].x = 2;
		pos_set_temp_IDs[1].x = 2;
		pos_set_temp_IDs[2].x = 2;
		pos_set_temp_IDs[3].x = 2;

		pos_set_temp_IDs[0].y = 0;
		pos_set_temp_IDs[1].y = 0;
		pos_set_temp_IDs[2].y = 0;
		pos_set_temp_IDs[3].y = 0;
		break;
	case 2:
		pos_set_temp_IDs[0].x = 0;
		pos_set_temp_IDs[1].x = 2;
		pos_set_temp_IDs[2].x = 2;
		pos_set_temp_IDs[3].x = 2;

		pos_set_temp_IDs[0].y = 0;
		pos_set_temp_IDs[1].y = 2;
		pos_set_temp_IDs[2].y = 2;
		pos_set_temp_IDs[3].y = 2;
		break;
	case 3:
		pos_set_temp_IDs[0].x = 0;
		pos_set_temp_IDs[1].x = 0;
		pos_set_temp_IDs[2].x = 2;
		pos_set_temp_IDs[3].x = 2;

		pos_set_temp_IDs[0].y = 0;
		pos_set_temp_IDs[1].y = 2;
		pos_set_temp_IDs[2].y = 2;
		pos_set_temp_IDs[3].y = 2;
		break;
	case 4:
		pos_set_temp_IDs[0].x = 0;
		pos_set_temp_IDs[1].x = 4;
		pos_set_temp_IDs[2].x = 4;
		pos_set_temp_IDs[3].x = 4;

		pos_set_temp_IDs[0].y = 0;
		pos_set_temp_IDs[1].y = 4;
		pos_set_temp_IDs[2].y = 4;
		pos_set_temp_IDs[3].y = 4;
		break;
	case 5:
		pos_set_temp_IDs[0].x = 0;
		pos_set_temp_IDs[1].x = 2;
		pos_set_temp_IDs[2].x = 4;
		pos_set_temp_IDs[3].x = 4;

		pos_set_temp_IDs[0].y = 0;
		pos_set_temp_IDs[1].y = 0;
		pos_set_temp_IDs[2].y = 2;
		pos_set_temp_IDs[3].y = 4;
		break;
	default:
		break;
	}
	if (step == 0)
	{
		pos_now_ID = pos_set_temp_IDs[0];
	}
	pos_set_ID = pos_set_temp_IDs[step];
	//pos_set_ID.x = pos_set[2][0].x;
	//pos_set_ID.y = pos_set[2][0].y;
	pos_out_ID = SelectTarget(pos_now_ID, pos_set_ID);
	if (CheckLocation(pos_out_ID))
	{
		if (step < 3)step++;
		pos_now_ID = pos_out_ID;

		if (pos_out_ID.x == pos_set_temp_IDs[0].x&&pos_out_ID.y == pos_set_temp_IDs[0].y)
		{
			time_cnt_en = 1;
		}
		if (pos_out_ID.x == pos_set_temp_IDs[3].x&&pos_out_ID.y == pos_set_temp_IDs[3].y)
		{
			time_cnt_en = 0;
		}
	}
	// 	if(step >=3)
	// 	{
	// 		time_cnt_en = 0;   //ֹͣ��ʱ
	// 	}
}
//����2
void Task_Changed_Target()
{
	//static int step = 0;

	if (step == 0)
	{
		pos_now_ID = pos_set_IDs[0];
	}
	pos_set_ID = pos_set_IDs[step];
	pos_out_ID = SelectTarget(pos_now_ID, pos_set_ID);
	if (CheckLocation(pos_out_ID))
	{
		if (pos_out_ID.y % 2 == 0)
		{
			if (step < 3)step++;

		}
		pos_now_ID = pos_out_ID;
		if (pos_out_ID.x == pos_set_IDs[0].x&&pos_out_ID.y == pos_set_IDs[0].y)
		{
			time_cnt_en = 1;
		}
		if (pos_out_ID.x == pos_set_IDs[3].x&&pos_out_ID.y == pos_set_IDs[3].y&&step >= 3)
		{
			time_cnt_en = 0;
		}
	}
	// 	if (step >= 3)
	// 	{
	// 		time_cnt_en = 0;   //ֹͣ��ʱ
	// 	}
}
//����3
void Task_Move_Around()
{
	//static int step = 0;
	location pos_set_temp_IDs[6] = {
		{ 0,2 },{ 1,1 },{ 3,1 },{ 3,3 },{ 1,3 },{ 4,4 }
	};
	if (step == 0)
	{
		pos_set_ID = pos_set_temp_IDs[0];
		pos_now_ID = pos_set_temp_IDs[0];
	}
	else if (step == 16)
		pos_set_ID = pos_set_temp_IDs[5];
	else if (step % 4 != 0)
		pos_set_ID = pos_set_temp_IDs[step % 4];
	else
		pos_set_ID = pos_set_temp_IDs[4];
	pos_out_ID = SelectTarget(pos_now_ID, pos_set_ID);
	if (CheckLocation(pos_out_ID))
	{
		if (step < 16)step++;
		pos_now_ID = pos_out_ID;
		if (pos_out_ID.x == pos_set_temp_IDs[0].x&&pos_out_ID.y == pos_set_temp_IDs[0].y)
		{
			time_cnt_en = 1;
		}
		if (pos_out_ID.x == 4 && pos_out_ID.y == 4 && step >= 16)
		{
			time_cnt_en = 0;
		}
	}
	// 	if (step >= 16)
	// 	{
	// 		time_cnt_en = 0;   //ֹͣ��ʱ
	// 	}
}

//����
void Task_Advance()
{
	location pos_set_temp_IDs[5] = {
		{ 0,2 },{ 4,2 },{ 0,4 },{ 2,0 },{ 4,4 }
	};
	if (step == 0)
	{
		pos_set_ID = pos_set_temp_IDs[0];
		pos_now_ID = pos_set_temp_IDs[0];
	}
	else
		pos_set_ID = pos_set_temp_IDs[step%5];
	pos_out_ID = pos_set_ID;
	if (CheckLocation(pos_out_ID))
	{
		step++;
		pos_now_ID = pos_out_ID;
		if (pos_out_ID.x == pos_set_temp_IDs[0].x&&pos_out_ID.y == pos_set_temp_IDs[0].y)
		{
			time_cnt_en = 1;
		}
		if (pos_out_ID.x == 4 && pos_out_ID.y == 4 && step >= 16)
		{
			time_cnt_en = 0;
		}
	}
}
//taskMode:
//			1:����1
//			2:����2
//			3:����3
//			4:����4
//			11:����1
//			12:����2
//			13:����3
//			14:����4(����)
//�趨��x=pos_set[pos_out_ID.x][pos_out_ID.y].x
//�趨��y=pos_set[pos_out_ID.x][pos_out_ID.y].y
void Task_program(int taskMode)
{
	if (taskMode != taskMode_last)
	{
		step = 0;
		//time_cnt_en = 1;
		time_cnt_s = 0;
		time_cnt_5ms = 0;     //ģʽ�ı��ʼ����ʱ  
	}
	taskMode_last = taskMode;
	if (taskMode == 6)
		Task_Changed_Target();
	else if (taskMode == 7)
		Task_Move_Around();
	else if (taskMode == 8)
		Task_Advance();
	else
		Task_Confirmed_Target(taskMode);

}

