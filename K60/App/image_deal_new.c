#include "image_deal_new.h"

/**************picture_deal_global_var******/
position car_position = normal;				//��ʼ�����ڻ���λ�ӱ�־Ϊnormal
ten_status car_ten_status = normal_ten;		//��ʼ������ʮ����λ�ӱ�־Ϊnormal_ten
status   car_status = running;	//����ͣ״̬��־Ϊrunning
int16 left_line[120] = { 0 };	//���������
int16 right_line[120] = { 0 };	//�ұ�������
int16 center_line[120] = { 0 };	//���������飨һ���������������������ʼ��һ�����ڲ�����ʱ��һ����
int16 start_line[120] = { 0 };	//������ʼ������
int16 compensation_line[10] = { 69,69,69,69,70,70,70,71,71,71 };//ȷ��������ʼ��Ľ���ʮ�У���û�б����ô˲���
int16 near_lost_count_left = 0;	//����ʮ����߶��߼���
int16 near_lost_count_righ = 0;	//����ʮ���ұ߶��߼���
int16 lost_to_have_flag = 0;		//�����һر�־
int16 turn_flag = 0;				//ת���־
int16 lost_count = 0;				//���߼���

int dir = 1;

int16 cp_line[120] = { 30,32,32,32,34,34,36,36,36,38,38,38,40,40,42,42,42,44,44,44,46,46,48,48,48,50,50,\
52,52,52,54,54,54,56,56,58,58,58,60,60,60,62,62,64,64,64,66,66,68,68,68,70,70,70,\
72,72,74,74,74,76,76,76,78,78,80,80,80,82,82,82,84,84,86,86,86,88,88,90,90,90,\
92,92,92,94,94,96,96,96,98,98, 92, 92, 92, 94, 94, 96, 96, 96, 98, 98, 92, 92, 92, 94, 94, 96, 96, 96, 98, 98, \
92, 92, 92, 94, 94, 96, 96, 96, 98, 98 };
int16 circle_offset = 15;

//�������ߣ����뻷��ֻ�ܿ���һ���ߣ���һ���߰���Զ������ƽ�Ƶõ���
unsigned char BlackEndL = 0;
unsigned char BlackEndM = 0;
unsigned char BlackEndR = 0;
unsigned char BlackEndMax = 0;
unsigned char BlackEndLMR = 0;
unsigned char g_Derict = 0;
unsigned char LEndFlag = 0;//�������־
unsigned char MEndFlag = 0;//�С�������־
unsigned char REndFlag = 0;//�ҽ�����־
int highM = 0, highL = 0, highR = 0;
//2017��4��16�ճ���Բ��
int lowX = 0, highY = 0, highX = 0;
float AddCircleLine[60];
int bCircle = 0, enCalcCircle = 1;
float direc_err_1, direc_err_2, direc_err_3;

char obstacle;	//�ϰ����־λ 
char obstacle_cnt;
int ob_left = 0, ob_right = 0;
int ob_cnter = 0;
int ob_err = 0;
int ob_sta[8] = { 0 };
int ttt_cnt = 0;
int start_fl = 0;
extern int stop_flag_cnt;
char circle_cnt = 0;

int16 point[2] = { 0 };
/**************function*********************/
/**
* @file		����ͼ��sline��eline�����õ�����ƫ��
*
* @date		2017-4
*/


/**************function*********************/
/**
* @file		������ʼ��
* @date		2017-4
*/
void param_reset(void)
{
	static int16 i;
	for (i = 0; i <= 119; i++) {
		LL(i) = 0;	//����߳�ʼ��
		RL(i) = 159;	//�ұ��߳�ʼ��

	}

	near_lost_count_left = 0;
	near_lost_count_righ = 0;

	lost_to_have_flag = 0;
	lost_count = 0;
}

//start>end  return left_column
/**************function*********************/
/**
* @file		8�㷶Χ�ڸ�Ч��Ѱ��
* @date		2017-4
*/
int16 gb_recursion(int16 start, int16 end, int16 line)
{
	if (abs(start - end) == 1) {
		return end;
	}
	if (PP(line, ((start + end) / 2)) == BLACK_P) {
		return gb_recursion(start, ((start + end) / 2), line);	//�ݹ�
	}
	if (PP(line, ((start + end) / 2)) == WHITE_P) {
		return gb_recursion(((start + end) / 2), end, line);		//�ݹ�
	}
	return 0;
}

/**************function*********************/
/**
* @file		start��line֮��Ѱ�ߡ�8����Ѱ�ߣ��ҳ����·�Χ�����ø�ЧѰ���Ҿ����
* @date		2017-4
*/
void find_line(int16 line, int16 start)
{
	static int16 i;
	//left
	for (i = start; i > 8; i -= 8) {
		if (PP(line, i) == BLACK_P) {
			LL(line) = gb_recursion((i + 7), i, line);
			break;
		}
	}
	if (i <= 8) {
		LL(line) = gb_recursion((i + 8), 0, line);
	}

	//right
	for (i = start; i < 151; i += 8) {
		if (PP(line, i) == BLACK_P) {
			RL(line) = gb_recursion((i - 7), i, line);
			break;
		}
	}
	if (i >= 151) {
		RL(line) = gb_recursion((i - 8), 159, line);
	}

}



/**************function*********************/
/**
* @file		�õ�����arry��i��j�е�ƽ��ƫ��
* @date		2017-4
*/
float get_average_diff(int16 i, int16 j, int16* arry)
{
	static int16 a;
	static int32 sum;
	sum = 0;


	quick_sort(arry, i, j);		//��������
	for (a = i + 1; a <= j - 1; a++)
	{
		sum += arry[a];
	}

	return (sum / (float)(j - i - 1));
}

/**************function*********************/
/**
* @file
* @date		2017-4
*/
int16 partition(int16 arr[], int16 low, int16 high)
{
	int16 key;
	key = arr[low];
	while (low < high) {
		while (low < high && arr[high] >= key)
			high--;
		if (low < high)
			arr[low++] = arr[high];
		while (low < high && arr[low] <= key)
			low++;
		if (low < high)
			arr[high--] = arr[low];
	}
	arr[low] = key;
	return low;
}

/**************function*********************/
/**
* @file		��������
* @date		2017-4
*/
void quick_sort(int16 arr[], int16 start, int16 end)
{
	int16 pos;
	if (start < end) {
		pos = partition(arr, start, end);//
		quick_sort(arr, start, pos - 1);	//�ݹ�
		quick_sort(arr, pos + 1, end);		//�ݹ�
	}
	return;
}

void find_black_point(int start_x, int end_x, int start_y, int end_y)
{
	 	 int16 i =0, j =0;
		 int16  asd = 0;
	 	 int16 dis = 0, dis_max = 0;
		 int temp_x = 0, temp_y = 0;
		 temp_x = point[0];
		 temp_y = point[1];
		 asd = 0;
	 	for (j = start_y; j < end_y; j++)    //ָ����ΧѰ����߽�  
	 	{
			asd++;
	 		for (i = start_x; i < end_x; i ++) 
			{
	 			if (PP(j, i) == BLACK_P) {
	 				LL(j) = i;
	 				break;
	 			}

	 		}
	 		if (LL(j) >= start_x + 1&&LL(j)<= end_x)      //����߽�  
	 		{
	 			point[0] = LL(j) + 2;
	 			point[1] = j+1;
	 			vcan_send_buff[0] = (float)point[0];
	 			vcan_send_buff[1] = (float)j;
	 			pos_ball.x = point[0];
	 			pos_ball.y = point[1];
	 			j = end_y;
	 		}
	 	}
// 	int i, j;
// 	for (j = start_y; j < end_y; j++)
// 	{
// 		for (i = start_x; i < end_x; i++)
// 		{
// 			if (PP(j, i) == BLACK_P)
// 			{
// 				point[0] = i;
// 				point[1] = j;
// 				
// 				j = end_y;
// 				break;
// 			}
// 
// 		}
// 	}


}