#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include  "GUI_ADC_CollectVoltage_Dialog.h"
#include "bsp_adc.h"


/* ���� ID */
#define ID_ADV_WIN         0x01    // �м���ʾ����ID
#define SCROLLBAR_Brigh_ID 0x02    // ��������ID
#define ID_TEXTBOX_Title   0x03    // ������
#define ID_TEXTBOX_Brigh   0x04    // ���Ȱٷֱ�

#define CircleCenter_1    (46)     // ��������ת�뾶
#define CircleCenter_2    (64)    // Բ���������뾶��С��
#define CircleCenter_3    (CircleCenter_2 + 6)    //  ������ CircleSize / 2

/* �ƶ������־ */
#define LeftToRight    0
#define RightToLeft    1
#define MOVE_WIN       1

#define CircleSize    140    // Բ����ʾ����Ĵ�С
#define Circle_X      169    // Բ����ʾ�����λ��
#define Circle_Y      (54)   // Բ����ʾ�����λ��

#define GUI_ADC_BACKGROUNG_PIC      "adc_desktop.jpg"

#define TitleHeight    28    // �������ĸ߶�

#define TriangleLen    10    // �����εı߳�

uint8_t AovingDirection = 0;
//double count = 0.0;
HWND MAIN_Handle;
HWND Title_Handle;
HWND Brigh_Handle;
HWND ADC_Handle;

HDC bk_hdc, TrianglePointer_DC;

static BITMAP bm_Triangle;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
double ADC_Vol; 

static void	X_MeterPointer(HDC hdc, int cx, int cy, int r, u32 color, double dat_val)
{
  double angle;
  int midpoint_x,midpoint_y;
  POINT pt[4];

  angle = (dat_val * 1.427 );    // ����Ƕ� + 0.785 = 45��
  
  /* ����������ƽ�е�һ�ߵ��е����� */
  midpoint_x =cx - sin(angle) * (r - TriangleLen * 0.866);    // 0.866 = sqrt(3) / 2
  midpoint_y =cy + cos(angle) * (r - TriangleLen * 0.866);

  /* ��Զ��һ���� */
  pt[0].x = cx - r * sin(angle);
  pt[0].y = cy + r * cos(angle);

  /* ��Զһ������һ�� */
  pt[1].x = midpoint_x - (TriangleLen / 2) * sin(angle - 1.57);    // 1.57 = 3.14/2 = ��/2 = 90��
  pt[1].y = midpoint_y + (TriangleLen / 2) * cos(angle - 1.57);

  /* ��Զһ����ұ�һ�� */
  pt[2].x = midpoint_x - (TriangleLen / 2) * sin(angle + 1.57);
  pt[2].y = midpoint_y + (TriangleLen / 2) * cos(angle + 1.57);

  pt[3].x = pt[0].x;
  pt[3].y = pt[0].y;


  /* �������� */
  SetBrushColor(hdc,color);
  EnableAntiAlias(hdc, TRUE);
  FillPolygon(hdc,0,0,pt,4);
  EnableAntiAlias(hdc, FALSE);
}

//�˳���ť�ػ���
static void CollectVoltage_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));      //���û���ɫ
	}
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }
}

/*
 * @brief  �ػ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button�Ĵ��ھ��.
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  GetClientRect(hwnd, &rc);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));

  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}

/* �ػ�Բ����ʾ���� */
void Circle_Paint(HWND hwnd, HDC hdc)
{
  char  cbuf[128];
  WCHAR wbuf[128];
  int vertex_x,vertex_y;
  RECT rc = {0, 0, CircleSize, CircleSize};
  int angle = ((225 - (-45))) * ADC_Vol / 3.3;

  EnableAntiAlias(hdc, TRUE);

  RotateBitmap(hdc, CircleSize/2, CircleSize/2, &bm_Triangle, angle + 45);
  
  if (ADC_Vol > 0.03)    // ��ѹ̫С������ɫСԲȦ
  {
    SetBrushColor(hdc, MapARGB(hdc, 255, 200, 200, 200));
    FillArc(hdc, CircleSize/2, CircleSize/2 + 2, CircleCenter_2+1, CircleCenter_3-1, -45, angle - 45);
    /* �������СԲȦ������ */
    vertex_x =  CircleSize / 2 - ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * sin(3.14/4); 
    vertex_y =  CircleSize / 2 + ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * cos(3.14/4) + 2; 
    FillCircle(hdc, vertex_x, vertex_y, (CircleCenter_3 - CircleCenter_2) / 2 - 1);     // ���СԲȦ
  }

  EnableAntiAlias(hdc, FALSE);

  /* ʹ��Ĭ������ */
	SetFont(hdc, defaultFont);

  rc.w = 24*3;
  rc.h = 48;
  rc.x = CircleSize/2 - rc.w/2 - 5;
  rc.y = CircleSize/2 - rc.h/2;

  /* ��ʾ��ѹ�ٷֱ� */
  SetTextColor(hdc, MapARGB(hdc, 255, 0, 0, 0));
  SetFont(hdc, controlFont_32);
  x_sprintf(cbuf, "%d", (int)(ADC_Vol/3.3*100));    // H -> % x_wsprintf(Backlightwbuf, L"%d", i);
  x_mbstowcs(wbuf, cbuf, 128);
  rc.w -= 20;
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);    // ��������(���ж��뷽ʽ)DT_CENTER
  rc.w += 20;
  rc.y += 4;
  SetFont(hdc, controlFont_24);
  DrawText(hdc, L"H", -1, &rc, DT_VCENTER|DT_RIGHT);    // ��������(���ж��뷽ʽ)DT_CENTER

  /* ��ʾ�ɼ����ĵ�ѹֵ */
  SetFont(hdc, defaultFont);
  rc.w = 24*4 - 10;
  rc.h = 30;
  rc.y = CircleSize/2 - rc.h/2 + CircleCenter_3 - 8;
  x_sprintf(cbuf, "%.2fV", ADC_Vol);
  x_mbstowcs(wbuf, cbuf, 128);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // ��������(���ж��뷽ʽ)
}
 
static LRESULT	CollectVoltage_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      ADCx_Init();    // ��ʼ�� ADC
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  286, 10, 25, 25, hwnd, eID_ADC_EXIT, NULL, NULL); 

      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0;

      Title_Handle = CreateWindow(TEXTBOX, L"ADCһ��λ����ѹ��ʾ", WS_VISIBLE | WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Title, NULL, NULL);//
      SendMessage(Title_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER | DT_BKGND);   

       /* ��������ָ�� */
      TrianglePointer_DC = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, TriangleLen, CircleCenter_1 * 2);    // ����������ָ���ڴ� DC
      ClrDisplay(TrianglePointer_DC, NULL, 0);
      X_MeterPointer(TrianglePointer_DC, TriangleLen/2, CircleCenter_1, CircleCenter_1-2, MapARGB(TrianglePointer_DC, 255, 250, 20, 20), 0);
      /* ת����bitmap */
      DCtoBitmap(TrianglePointer_DC,&bm_Triangle);

      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;

      if (strstr(GUI_ADC_BACKGROUNG_PIC, "0:/") == NULL)
        res = RES_Load_Content(GUI_ADC_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      else
        res = FS_Load_Content(GUI_ADC_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);

      bk_hdc = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      if(res)
      {
        /* ����ͼƬ���ݴ���JPG_DEC��� */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* �������ڴ���� */
        JPG_Draw(bk_hdc, 0, 0, dec);

        /* �ر�JPG_DEC��� */
        JPG_Close(dec);
      }
      /* �ͷ�ͼƬ���ݿռ� */
      RES_Release_Content((char **)&jpeg_buf);

      SetTimer(hwnd, 2, 20, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
      int tmr_id = wParam;

      if (tmr_id == 2)
      {
        RECT rc;
        static double ADC_buff = 0.0;
        double vol_buff = 0.0;
        static uint8_t xC = 0;

        vol_buff =(double) ADC_ConvertedValue/4096*(double)3.3; // ��ȡת����ADֵ
//        GUI_DEBUG("��ѹֵǰΪ��%f", ADC_Vol);
        #if 1

          if (xC++ < 10)
          {
            ADC_buff += vol_buff;
            break;
          }
          else
          {
            ADC_Vol = ADC_buff / ((double)(xC-1));
            ADC_buff = 0;
            xC = 0;
          }

        #else
          
        ADC_Vol = (double)(((int)(vol_buff * 10)) / 10.0);

        #endif
//        GUI_DEBUG("��ѹֵ��Ϊ��%f", ADC_Vol);
        
        rc.x = Circle_X;
        rc.y = Circle_Y;
        rc.w = CircleSize * 2;
        rc.h = CircleSize * 2;

        InvalidateRect(hwnd, &rc, FALSE);
      }
      break;
    }
    case WM_ERASEBKGND:
    {
      
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc.x, rc.y, SRCCOPY);

      return TRUE;
    }

    case WM_PAINT:
    {
      HDC hdc , hdc_mem;
      PAINTSTRUCT ps;
      //  RECT rc = {0,0,800,70};
      //  hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);

      hdc_mem = CreateMemoryDC(SURF_SCREEN, CircleSize, CircleSize);

      hdc = BeginPaint(hwnd, &ps);

      BitBlt(hdc_mem, 0, 0, CircleSize, CircleSize, bk_hdc, Circle_X, Circle_Y, SRCCOPY);

      Circle_Paint(hwnd, hdc_mem);    // ���Ƶ�һ�������е�Բ����ʾ����
      
      BitBlt(hdc, Circle_X, Circle_Y, CircleSize, CircleSize, hdc_mem, 0, 0, SRCCOPY);
      DeleteDC(hdc_mem);

      EndPaint(hwnd, &ps);

      break;
    } 
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_ADC_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }    

          case ID_TEXTBOX_Title:
          {
            Textbox_OwnerDraw(ds);
            return TRUE;             
          } 
       }

       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//��ȡ��Ϣ��ID��
      code=HIWORD(wParam);//��ȡ��Ϣ������    
      if(code == BN_CLICKED && id == eID_ADC_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    case WM_LBUTTONUP:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_MOUSEMOVE:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);
    }

    case WM_LBUTTONDOWN:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_DESTROY:
    {
      stopADC();    // ֹͣADC�Ĳɼ�
      DeleteDC(bk_hdc);
      DeleteDC(TrianglePointer_DC);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_ADC_CollectVoltage_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CollectVoltage_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
    
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_ADC_CollectVoltage_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //��ʾ������
	ShowWindow(MAIN_Handle, SW_SHOW);
	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


