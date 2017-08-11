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
//输入：目标点位置ID，现在位置ID
//输出：规划点位置ID
//以单个辅助点及周围4个目标点为一个小区域
//先移动到制定小区域后再移动到目标点
//x轴优先运动
location SelectTarget(location pos_begin, location pos_next)
{
	location posvalue = pos_next;
	//如在小区域内，直接到目标点
	if (ABS(pos_begin.x - pos_next.x) <= 2)
		if (ABS(pos_begin.y - pos_next.y) <= 2)
			return pos_next;
	//右小区域->左小区域
	if (pos_begin.x - pos_next.x > 2)
	{
		//辅助点x到位
		posvalue.x = pos_begin.x == 3 ? 1 : 3;
		//辅助点y到位
		posvalue.y = pos_begin.y == 2 ? pos_next.y < 2 ? 1 : 3 : pos_begin.y < 2 ? 1 : 3;
		return posvalue;
	}
	//左小区域->右小区域
	else if (pos_begin.x - pos_next.x < -2)
	{
		//辅助点x到位
		posvalue.x = pos_begin.x == 1 ? 3 : 1;
		//辅助点y到位
		posvalue.y = pos_begin.y == 2 ? pos_next.y < 2 ? 1 : 3 : pos_begin.y < 2 ? 1 : 3;
		return posvalue;
	}
	//下小区域->上小区域
	if (pos_begin.y - pos_next.y > 2)
	{
		//辅助点y到位
		posvalue.y = pos_begin.y == 3 ? 1 : 3;
		//辅助点x到位
		posvalue.x = pos_begin.x == 2 ? pos_next.x < 2 ? 1 : 3 : pos_begin.x < 2 ? 1 : 3;
	}
	//上小区域->下小区域
	else if (pos_begin.y - pos_next.y < -2)
	{
		//辅助点y到位
		posvalue.y = pos_begin.y == 1 ? 3 : 1;
		//辅助点x到位
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

