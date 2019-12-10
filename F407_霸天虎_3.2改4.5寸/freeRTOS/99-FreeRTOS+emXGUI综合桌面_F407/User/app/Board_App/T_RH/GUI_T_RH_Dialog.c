#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./dht11/bsp_dht11.h"

/* ͼƬ��Դ */
#define Resource_From_Where 0//ͼƬ��Դ�Ӻδ�����,1ΪSD��,0ΪFlash

#if Resource_From_Where
#define GUI_HUMITURE_BACKGROUNG_PIC    "0:/humiture_desktop.jpg"
#else
#define GUI_HUMITURE_BACKGROUNG_PIC    "humiture_desktop.jpg"
#endif

/* ���� ID */
#define ID_TEXTBOX_Title    0x00     // ������
#define ID_TEXTBOX_T        0x01     // �¶���ʾ
#define ID_TEXTBOX_RH       0x02     // ʪ����ʾ


/* ��ť ID */
#define eID_T_RH_EXIT    0

#define TitleHeight    70    // �������ĸ߶�

DHT11_Data_TypeDef DHT11_Data;

HWND T_Handle;
HWND RH_Handle;

static HDC bk_hdc;
uint8_t Pointerstyle = 0;

//�˳���ť�ػ���
static void T_RH_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //���û���ɫ
	}

  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 5;
  }

}

/*
 * @brief  �ػ���ʾ���ȵ�͸���ı�
 * @param  ds:	�Զ�����ƽṹ��
 * @retval NONE
*/
static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //����һ����ť���
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
  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
  rc.w -= 45;
  GetWindowText(hwnd, wbuf, 128); //��ð�ť�ؼ�������
  SetFont(hdc, controlFont_32);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}

/*
 * @brief  ������������Ϣ����Ļص�����
 * @param  ds:	
 * @retval NONE
*/
static LRESULT	Collect_DTH11_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 

      /* DHT11��ʼ�� */
	    DHT11_GPIO_Config();
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  286, 10, 23, 23, hwnd, eID_T_RH_EXIT, NULL, NULL); 

      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight-2;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0; 

      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
			/* ������ʪ��ͼƬ */
      
      if (strstr(GUI_HUMITURE_BACKGROUNG_PIC, "0:/") != NULL)
      {
        res = FS_Load_Content(GUI_HUMITURE_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      }
      else
      {
        res = RES_Load_Content(GUI_HUMITURE_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      }
      
      bk_hdc = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      ClrDisplay(bk_hdc, NULL, 0);
      if(res)
      {
        /* ����ͼƬ���ݴ���JPG_DEC��� */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* �������ڴ���� */
        JPG_Draw(bk_hdc, 0, 0, dec);

        /* �ر�JPG_DEC��� */
        JPG_Close(dec);
      }
//      else
//			{
//				GUI_ERROR("Error to Load picture!Cheak Resource\r\n");
//				PostCloseMessage(hwnd);
//			}
      /* �ͷ�ͼƬ���ݿռ� */
      RES_Release_Content((char **)&jpeg_buf);

      SetTimer(hwnd, 0, 2000, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {

      if(Read_DHT11(&DHT11_Data) == SUCCESS)
      {
//				GUI_DEBUG("SUCCESS\r\n");
        InvalidateRect(hwnd, NULL, FALSE);    // �ػ洰��
      }
      else
      {
				GUI_ERROR("ERROR\r\n");
				printf("��ȡ���ݳ���");
        /* ������ */
      } 

      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      WCHAR wbuf[128];
      RECT rc;

      hdc = BeginPaint(hwnd, &ps);
      
      BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, bk_hdc, 0, 0, SRCCOPY);

      
      /* �¶���ֵ��ʾ */
      rc.w = 64;
      rc.h = 34;
      rc.x = 206;
      rc.y = 73;
      x_wsprintf(wbuf, L"%d.%d", DHT11_Data.temp_int,DHT11_Data.temp_deci);
      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
			SetFont(hdc, controlFont_32);
			
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ)

      /* ��ʾʪ����ֵ *///defaultFont
      rc.y = 133;
      x_wsprintf(wbuf, L"%d", DHT11_Data.humi_int);//.%d//,DHT11_Data.humi_deci
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//��������(���ж��뷽ʽ)

      
      EndPaint(hwnd, &ps);
      break;
    }
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_T_RH_EXIT:
          {
            T_RH_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case ID_TEXTBOX_T:
          case ID_TEXTBOX_RH:
          {
            Brigh_Textbox_OwnerDraw(ds);
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
      if(code == BN_CLICKED && id == eID_T_RH_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    case WM_DESTROY:
    {
      memset(&DHT11_Data, 0, sizeof(DHT11_Data));
      DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_T_RH_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Collect_DTH11_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
  
	//����������
	MAIN_Handle = CreateWindowEx(WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_T_RH_Dialog",
                              WS_CLIPCHILDREN,
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


