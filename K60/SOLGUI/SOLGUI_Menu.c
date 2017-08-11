#include"SOLGUI_Include.h"
#include<string.h>

char show_ok = 0;
extern uint8 imgbuff[CAMERA_SIZE];
extern float bet_value;					//��ص�ѹ
extern char vcan_send_flag;				//������λ����־λ
extern float g_AngleOfCar;				//С���Ƕ�
extern float g_BlanceControlOut;		//ƽ�����
extern float g_fDirectionControlOut;	//�������
extern float g_fExpectSpeed;			//�����ٶ�
extern  uint8  img[CAMERA_H][CAMERA_W]; //ͼ������
extern float g_dirControl_P ;			//����P	
extern float g_dirControl_D ;			//����
extern float g_dirControl_gyro_D;
extern float g_speedControl_P;			//�ٶ�P
extern char vcan_send_flag ;			 //ɽ����λ�����ͱ�־λ

extern int16 point[2];
extern int g_ball_x;
extern int g_ball_y;
extern int point_y_set;

extern int pwm_temp;
extern int pwm_temp1;
extern int servo1_pwm;
extern int servo2_pwm;
extern char mode;
extern int servo_mode;

int aa, ss, dd, ff, gg, hh, jj, kk;

#if MENU_FRAME_EN==1
//��������ͷͼ��  
void Draw_PIC(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char bmp[])
{
	int32 i, j, k;
	for (i = 0; i <= 117; i+=2)
	{
		for (j = 0; j<= 157; j+=2)
		{
			if(img[i][j]==BLACK_P||img[i+1][j]==BLACK_P||img[i][j+1]==BLACK_P||img[i+1][j+1]==BLACK_P)
			{
				SOLGUI_DrawPoint(  j/2+44, 63-i / 2 , BLACK_P);
			}
			else
			{
				SOLGUI_DrawPoint(  j/2+44, 63-i / 2 , WHITE_P);
			}
// 			if (i >= 19 && i <= 99 && j >= 49 && j <= 109) {
// 				SOLGUI_DrawPoint(i+22, j-50, img[i][j]);
// 			}
		}


	}
	for (i = 0; i <= 59; i+=2)
	{
		SOLGUI_DrawPoint(x_x/2 + 44-5, 63 - i, BLACK_P);    //������
	}
	for (i = 0; i <= 59; i+=2)
	{
		SOLGUI_DrawPoint(x_x / 2 + 44 +37, 63 - i, BLACK_P);    //������v
	}
	for (j = 0; j <= 79; j+=2)
	{
		SOLGUI_DrawPoint(j + 44, 63-x_y/2+5, BLACK_P);
	}
	for (j = 0; j <= 79; j+=2)
	{
		SOLGUI_DrawPoint(j + 44, 63 - x_y / 2 -37, BLACK_P);
	}
	for (i = 0; i <= 59; i ++)
	{
		SOLGUI_DrawPoint(point[0] / 2 + 44 , 63 - i, BLACK_P);    //������
	}
	for (j = 0; j <= 79; j ++)
	{
		SOLGUI_DrawPoint(j + 44, 63 - point[1] / 2 , BLACK_P);
	}
}
void Draw_PIC_correct(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char bmp[])
{
	int32 i, j, k;

	for (i = 0; i <= 159; i++)
	{
		img[x_y][i] = BLACK_P;
	}
	for (j = 0; j <= 119; j++)
	{
		img[j][x_x] = BLACK_P;
	}
	for (i = 0; i <= 117; i += 2)
	{
		for (j = 0; j <= 157; j += 2)
		{
			if (img[i][j] == BLACK_P || img[i + 1][j] == BLACK_P || img[i][j + 1] == BLACK_P || img[i + 1][j + 1] == BLACK_P)
			{
				SOLGUI_DrawPoint(j / 2 + 44, 63 - i / 2, BLACK_P);
			}
			else
			{
				SOLGUI_DrawPoint(j / 2 + 44, 63 - i / 2, WHITE_P);
			}
			// 			if (i >= 19 && i <= 99 && j >= 49 && j <= 109) {
			// 				SOLGUI_DrawPoint(i+22, j-50, img[i][j]);
			// 			}
		}
		// 		for (i = 0; i <= 59; i++)
		// 		{
		// 			SOLGUI_DrawPoint(40 + 44, i, BLACK_P);
		// 		}
		// 		for (j = 0; j <= 79; j++)
		// 		{
		// 			SOLGUI_DrawPoint(j + 44, 30, BLACK_P);
		// 		}

	}
}
void CarStart(void)
{
  DELAY_MS(1000);
}
void SaveParameter(void)
{
	my_cnf[0].f = angle_offset;
	my_cnf[1].f = g_upstandControl_P;
	my_cnf[2].f = g_upstandControl_D;
	my_cnf[3].f = g_speedControl_P;           
	my_cnf[4].f = g_fExpectSpeed;
	my_cnf[5].f = g_dirControl_P;
	my_cnf[6].f = g_dirControl_D;
    my_cnf[7].f = g_dirControl_gyro_D;

	my_cnf[8].f = (float)x_x;
	my_cnf[9].f = (float)x_y;
	my_cnf[10].f = M1PID.Proportion;
	my_cnf[11].f = M1PID.Integral;
	my_cnf[12].f = M1PID.Derivative;
	my_cnf[13].f = M2PID.Proportion;
	my_cnf[14].f = M2PID.Integral;
	my_cnf[15].f = M2PID.Derivative;
	my_cnf[16].f = servo_offset1;
	my_cnf[17].f = servo_offset2;
//	my_cnf[13].f = SPEED_CONTROL_D;
//	my_cnf[14].f = DIRECTION_CONTROL_P;
//	my_cnf[15].f = DIRECTION_CONTROL_D;
//	my_cnf[16].f = Begin_CarSpeed;
//	my_cnf[17].f = DIRECTION_CONTROL_DD;
//	my_cnf[18].f = DIRECTION_CONTROL_P_L;
//	my_cnf[19].f = DIRECTION_CONTROL_D_L;
	Write_config();
	
	SOLGUI_Widget_OptionText(3, "SAVE OK!");
	DELAY_MS(500);
	show_ok = 0; 

}
void run(void)
{
}

void set_abcd(void)
{
		pos_set_IDs[0].x = aa 	;
		pos_set_IDs[0].y =ss	;
		pos_set_IDs[1].x = dd;
		pos_set_IDs[1].y = ff;
		pos_set_IDs[2].x =gg	;
		pos_set_IDs[2].y =hh	;
		pos_set_IDs[3].x = jj;
		pos_set_IDs[3].y = kk;
		SOLGUI_Widget_OptionText(9, "SET OK!");
		DELAY_MS(500);

}
//##############################���Զ���ҳ�桿##############################
MENU_PAGE UI_List;MENU_PAGE UI_image_show;MENU_PAGE UI_Dataview;MENU_PAGE UI_Debug;    
MENU_PAGE UI_image_correct;
__M_PAGE(UI_List,"CAUC-Readme",PAGE_NULL,
{
  	SOLGUI_Cursor(6,0,16);
	SOLGUI_Widget_GotoPage(0,&UI_image_show);
    SOLGUI_Widget_GotoPage(1, &UI_Debug);
    SOLGUI_Widget_GotoPage(2,&UI_image_correct);
	SOLGUI_Widget_Button(3, "SavePara", SaveParameter);
	//SOLGUI_Widget_Spin(4, "PWM1_test", INT16, -1000, 1000, &pwm_temp);
	//SOLGUI_Widget_Spin(5, "PWM2_test", INT16, -1000, 1000, &pwm_temp1);
	//SOLGUI_Widget_Button(4, "Run", run);
	//SOLGUI_Widget_OptionText(5, " %d ", stop_flag_cnt);
	//SOLGUI_Widget_OptionText(6, "Voltage: %f v", bet_value);  //��ʾ��ѹ
	
	//SOLGUI_Widget_OptionText(5, "Voltage:  %d", img[80][20]);
	//SOLGUI_Widget_Spin(6, "Speed_Set", FLT16, -10000, 10000, &g_fExpectSpeed);
	//SOLGUI_Widget_OptionText(7, "Angle:   %f ", g_AngleOfCar);
	SOLGUI_Widget_OptionText(4, "x:   %d ", g_ball_x);
    SOLGUI_Widget_OptionText(5, "y:   %d ", g_ball_y);
	SOLGUI_Widget_Spin(6, "mode", INT16, 1, 8, &mode);
	SOLGUI_Widget_Spin(7, "ser_mode", INT16, 0, 2, &servo_mode);
	SOLGUI_Widget_Spin(8, "servo1", INT32, -10000, 10000, &servo_offset1);
	SOLGUI_Widget_Spin(9, "servo2", INT32, -10000, 10000, &servo_offset2);
	SOLGUI_Widget_OptionText(10, "x:   %f ", M1PID.SetPoint);
	SOLGUI_Widget_OptionText(11, "y:   %f ", M2PID.SetPoint);
	SOLGUI_Widget_OptionText(12, "T:   %d ", timeCount);
//	SOLGUI_Widget_Spin(4, "PWM1_test", INT16, -10000, 10000, &servo1_pwm);
//	SOLGUI_Widget_Spin(5, "PWM1_test", INT16, -10000, 10000, &servo2_pwm);
	//SOLGUI_Widget_Spin(8, "CarStop", INT8, 0, 1, &Flag_Stop);			
	//SOLGUI_Widget_Spin(9, "SpeedEN", INT8, 0, 1, &Flag_Speed);		
	//SOLGUI_Widget_Spin(10, "DirecEN", INT8, 0, 1, &Flag_Direction);		
	//SOLGUI_Widget_Spin(7, "motor", FLT16, -1000, 1000, &g_BlanceControlOut);
	SOLGUI_Widget_Spin(13, "vcan_send", INT8, 0, 1, &vcan_send_flag);

});

//-----------------------

__M_PAGE(UI_image_show,"Image",&UI_List,
{
// 	img[20][40] =  0;
// 	img[20][120] = 0;
// 	img[100][40] = 0;
// 	img[100][120] =0;
// 	img[21][40] =  0;
// 	img[21][120] = 0;
// 	img[101][40] = 0;
// 	img[101][120] =0;
// 	img[22][40] = 0;
// 	img[22][120] = 0;
// 	img[102][40] = 0;
// 	img[102][120] = 0;
	Draw_PIC(0,0,127,63,bmp_buff);
//SOLGUI_Widget_OptionText(1, "ERR %d ", ttt_cnt);
SOLGUI_Cursor(6, 0, 16);
SOLGUI_Widget_OptionText(2, "X%d", point[0]);
SOLGUI_Widget_OptionText(3, "Y%d", point[1]);
SOLGUI_Widget_OptionText(4, "X%d", servo1_pwm);
SOLGUI_Widget_OptionText(5, "Y%d", servo2_pwm);
// SOLGUI_Widget_Spin(4, " ", INT16, 0, 159, &x_x);
// SOLGUI_Widget_Spin(5, " ", INT16, 0, 119, &x_y);
//SOLGUI_DrawPoint(point[1], point[2], 0);
});

__M_PAGE(UI_image_correct, "SET_ABCD", &UI_List,
{
	SOLGUI_Cursor(6, 0, 16);
SOLGUI_Widget_Spin(0, "Ax", INT16, 0, 4, &aa);
SOLGUI_Widget_Spin(1, "AY", INT16, 0, 4, &ss);
SOLGUI_Widget_Spin(2, "Bx", INT16, 0, 4, &dd);
SOLGUI_Widget_Spin(3, "By", INT16, 0, 4, &ff);
SOLGUI_Widget_Spin(4, "Cx", INT16, 0, 4, &gg);
SOLGUI_Widget_Spin(5, "Cy", INT16, 0, 4, &hh);
SOLGUI_Widget_Spin(6, "Dx", INT16, 0, 4, &jj);
SOLGUI_Widget_Spin(7, "Dx", INT16, 0, 4, &kk);
SOLGUI_Widget_Button(8, "SetABCD", set_abcd);
	// 	img[20][40] =  0;
	// 	img[20][120] = 0;
	// 	img[100][40] = 0;
	// 	img[100][120] =0;
	// 	img[21][40] =  0;
	// 	img[21][120] = 0;
	// 	img[101][40] = 0;
	// 	img[101][120] =0;
	// 	img[22][40] = 0;
	// 	img[22][120] = 0;
	// 	img[102][40] = 0;
	// 	img[102][120] = 0;
	//Draw_PIC_correct(0,0,127,63,bmp_buff);
//SOLGUI_Widget_OptionText(1, "ERR %d ", ttt_cnt);

// SOLGUI_Widget_Button(4, "Save", SaveParameter);
// SOLGUI_Widget_Spin(5, "x", INT16, 0, 159, &x_x);
// SOLGUI_Widget_Spin(6, "y", INT16, 0, 119, &x_y);
// SOLGUI_Widget_OptionText(7, "mid_x:   %d ", g_ball_x);
// SOLGUI_Widget_OptionText(8, "mid_y:   %d ", g_ball_y);
//SOLGUI_DrawPoint(point[1], point[2], 0);
});
//-----------------------

__M_PAGE(UI_Dataview,"Dataview",&UI_List,
{
  	SOLGUI_Cursor(6,0,14); 	

});
//-----------------------
__M_PAGE(UI_Debug,"Debug",&UI_List,
{
		SOLGUI_Cursor(6,0,16);		
		SOLGUI_Widget_Spin(1, "1Set_P", FLT16, -2000, 2000, &M1PID.Proportion);
		SOLGUI_Widget_Spin(2, "1Set_I", FLT16, -2000, 2000, &M1PID.Integral);
		SOLGUI_Widget_Spin(3, "1Set_D", FLT16, -2000, 2000, &M1PID.Derivative);
		SOLGUI_Widget_Spin(4, "2Set_P", FLT16, -2000, 2000, &M2PID.Proportion);
		SOLGUI_Widget_Spin(5, "2Set_I", FLT16, -2000, 2000, &M2PID.Integral);
		SOLGUI_Widget_Spin(6, "2Set_D", FLT16, -2000, 2000, &M2PID.Derivative);
});

//##############################��ȫ�ֱ������塿##############################
MENU_PAGE *current_page;//��ǰҳ��
GUI_FIFO _key_cache;	//��ֵFIFO
u8 cur_key=0;			//ȫ�ּ�ֵ

u8 SOLGUI_CSR=0;		//ռ�ñ�־�Ĵ����������㼴��ʾռ�ã������˳���ǰҳ�棩
/*----------------��ռ�ñ�־�Ĵ�����------------------	
	SOLGUI_CSR[0]��	OK[���1]ռ��1������0
	SOLGUI_CSR[1]��	ȫ��ռ��1������0��Ҫռ��ȫ����
	SOLGUI_CSR[2]�� OK[���2]ռ��1���ر�0
-----------------------------------------------------*/

extern CURSOR *cursor;	//������

//##############################���ڲ�ʹ�á�##############################

void FIFO_Init(void)
{
//------------��FIFO�������㡿
	memset(_key_cache.FIFOBuffer,0,sizeof(_key_cache.FIFOBuffer));
//------------��FIFO��дָ�����㡿
	_key_cache.Read=0;
	_key_cache.Write=0;
}

void FIFO_EnQueue(u8 KeyCode)
{
	_key_cache.FIFOBuffer[_key_cache.Write]=KeyCode;
	if(++_key_cache.Write>=FIFOBUF_SIZE) _key_cache.Write=0;
}

u8 FIFO_DeQueue(void)
{
	u8 ret;
	if(_key_cache.Read==_key_cache.Write) return(FIFO_NONE); 	//FIFO��
	else
	{
		ret=_key_cache.FIFOBuffer[_key_cache.Read];
		if (++_key_cache.Read>=FIFOBUF_SIZE) _key_cache.Read=0;
		return(ret);
	}
} 

void SOLGUI_Menu_Title(MENU_PAGE *page)
{
	u8 left_len=0,title_len=0;

	title_len=6*(strlen((const char*)page->pageTitle)+4);					//����������ؿ�� 
	left_len=(SCREEN_X_WIDTH-title_len)>>1;									//�������ƫ������

//---------�����⡿
	SOLGUI_printf(left_len+2,56,F6X8,"[ %s ]",page->pageTitle);				//page.pageTitle  ҳ��������
	SOLGUI_GBasic_Line(6,57,left_len+2,57,ACTUAL);							//��������
	SOLGUI_GBasic_Line(left_len+title_len-2,57,SCREEN_X_WIDTH-7,57,ACTUAL);	//��������
//---------������ͼ�꡿
	if((page->parentPage!=PAGE_NULL)&&(SOLGUI_CSR==0)) SOLGUI_printf(0,56,F6X8,"%c",ICON_BACK);	//�и�ҳ�����ڷ�ռ��ģʽ�����������ͼ��

}

//##############################��API��##############################
void SOLGUI_Menu_SetHomePage(MENU_PAGE *home_page)	//��ҳ����
{
	current_page=home_page;
	FIFO_Init();
}

void SOLGUI_InputKey(u8 key_value) 					//������������ϵͳ�л�ȡ��ǰ������ֵ�������
{
	FIFO_EnQueue(key_value);
}

u8 SOLGUI_GetCurrentKey(void) 						//������������ȡGUI��ǰ��ȫ�ּ�ֵ
{
  	return(cur_key); 	
}

void SOLGUI_Menu_PageStage(void)					//����������SOLGUIǰ̨ҳ���л���
{
//------------------����ֵ��ȡ��
	cur_key=FIFO_DeQueue();							//�ӳ���ȡһ����ֵ��Ϊ��ǰGUI��ȫ�ּ�ֵ
//------------------��������ơ�
	SOLGUI_Menu_Title(current_page);				//����
//------------------��ִ��ҳ�溯����
	current_page->pageFunc();						//ִ��ҳ�溯��
	if(cur_key==SOLGUI_KEY_BACK&&SOLGUI_CSR==0){	//����Ƿ�Ϊ���ؼ�ֵ��CSR��ռ��
		if(current_page->parentPage!=PAGE_NULL)		//�и�ҳ��ſ�ʹ�÷��ؼ�
		{
			current_page=current_page->parentPage;	//�´�ִ�и�ҳ�溯��
			cursor->cursor_rel_offset=0;			//��չ��ƫ�Ƽ�����
			cursor->viewport_offset=0;
		}
	}
}

#endif
