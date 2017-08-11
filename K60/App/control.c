#include "control.h"
#include "common.h"
#include "include.h"
location pos_ball;
location pos_set[5][5] = {
	{ { 13,25 },{ 0,0 },{ 82,22 },{ 0,0 },{ 151,22 } },
	{ { 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 } },
	{ { 14,93 },{ 0,0 },{ 83,91 },{ 0,0 },{ 151,89 } },
	{ { 0,0 },{ 0,0 },{ 0,0 },{ 0,0 },{ 0,0 } },
	{ { 17,159 },{ 0,0 },{ 84,158 },{ 0,0 },{ 151,157 } },

};
location pos_set_ID = { 4,2 }, pos_now_ID = { 0,0 };
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
void Correct_Mid(int nowMidX,int nowMidY)
{
	int i, j;
	int offsetx = nowMidX - pos_set[2][2].x, offsety = nowMidY - pos_set[2][2].y;
	for (i=0;i<5;i++)
	{
		for (j=0;j<5;j++)
		{
			pos_set[i][j].x += offsetx;
			pos_set[i][j].y += offsety;
		}
	}
}

