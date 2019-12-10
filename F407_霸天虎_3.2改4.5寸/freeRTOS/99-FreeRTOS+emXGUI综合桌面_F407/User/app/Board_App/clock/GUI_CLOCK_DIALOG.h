#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"

#ifndef _GUI_CLOCK_DIALOG_H
#define _GUI_CLOCK_DIALOG_H

/* ͼƬ��Դ�� */
#define GUI_CLOCK_BACKGROUNG_PIC      "settingsdesktop.jpg"      // �����ý���ʹ��ͬһ�ű���      
#define GUI_CLOCK_BTN_PIC             "clock_but.png"              
#define GUI_CLOCK_BTN_PRESS_PIC       "clock_but_press.png"        
#define GUI_CLOCK_CALENDAR_PIC        "settings_calendar.png"       
//#define GUI_CLOCK_00BACK_PIC          "clock_00_background.png"     
//#define GUI_CLOCK_00H_PIC             "clock_00_hour_hand.png"   
//#define GUI_CLOCK_00M_PIC             "clock_00_minute_hand.png" 
//#define GUI_CLOCK_00S_PIC             "clock_00_second_hand.png"  
//#define GUI_CLOCK_01BACK_PIC          "clock_01_background.png"     
//#define GUI_CLOCK_01H_PIC             "clock_01_hour_hand.png"   
//#define GUI_CLOCK_01M_PIC             "clock_01_minute_hand.png"  
//#define GUI_CLOCK_01S_PIC             "clock_01_second_hand.png"  
#define GUI_CLOCK_02BACK_PIC          "clock_02_background.png"     
#define GUI_CLOCK_02H_PIC             "clock_02_hour_hand.png"    
#define GUI_CLOCK_02M_PIC             "clock_02_minute_hand.png"  
#define GUI_CLOCK_02S_PIC             "clock_02_second_hand.png" 
//#define GUI_CLOCK_CHCKED_PIC          "blue_glow.png"               


typedef enum 
{ 
  /****************** ��ť�ؼ� ID ֵ *******************/
  ID_CLOCK_BACK = 0x1000,      // ��һ��
  ID_CLOCK_NEXT,               // ��һ��
  ID_CLOCK_OK,                 // �������
  ID_CLOCK_WAIVE,              // ������������
  ID_CLOCK_EXIT,               // �˳���ť
	ID_CLOCK_SET,                // ����
  /***************** �ı��ؼ� ID ֵ *********************/
  ID_CLOCK_TITLE,              // ���ڱ�����
  ID_CLOCK_MONTH,              // ��
  ID_CLOCK_WEEK,               // ��
  ID_CLOCK_DAY,                // ��
	ID_CLOCK_SETTITLE,           // ���ô��ڱ�����
	ID_CLOCK_SETTIME,            // ������ʾʱ��
	ID_CLOCK_SETDATE,            // ������ʾ����
  ID_CLOCK_TIME,               // ʱ����ʾ���򣨴���Ϊ�ؼ������ػ棩
	/****************** ��ѡ��ť ***********************/
//	ID_CLOCK_Background00,
//	ID_CLOCK_Background01,
	ID_CLOCK_Background02,
	/********************* �б� **************************/
	ID_CLOCK_SetWin,
  ID_CLOCK_SetYear,
	ID_CLOCK_SetMonth,
  ID_CLOCK_SetDate,
  ID_CLOCK_SetHour,
  ID_CLOCK_SetMinute,
}clock_id_t;

typedef struct{
	WCHAR *icon_name;    // ͼ����
	RECT rc;             // λ����Ϣ
	clock_id_t id;      // ��ťID
}clock_icon_t;

typedef enum
{
	hdc_clock_btn = 0,
	hdc_clock_btn_press,
	hdc_clock_calendar,
//	hdc_clock_back_00,
//	hdc_clock_h_00,
//	hdc_clock_m_00,
//	hdc_clock_s_00,
//	hdc_clock_back_01,
//	hdc_clock_h_01,
//	hdc_clock_m_01,
//	hdc_clock_s_01,
	hdc_clock_back_02,
	hdc_clock_h_02,
	hdc_clock_m_02,
	hdc_clock_s_02,
//	hdc_clock_chcked,

	hdc_clock_end,              // �������������
}hdc_clock_png_t;

typedef struct{
	char *pic_name;     // ͼƬ��
	int w;               // ͼƬ��
	int h;               // ͼƬ��
	hdc_clock_png_t id;        // hdc ���
}clock_hdc_t;



#endif

