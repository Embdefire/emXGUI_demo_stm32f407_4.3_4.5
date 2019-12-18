#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "GUI_SimulateUDisk_Dialog.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "GUI_Font_XFT.h"
#include "emXGUI.h"

#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usb_bsp.h"

//#include "./pic_load/gui_pic_load.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;

static HDC hdc_bk;
static HDC hdc_btn_press;
static HDC hdc_btn;



//�˳���ť�ػ���
static void _ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc, rc_tmp;
  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
  hwnd = ds->hwnd;


  if (ds->State & BST_PUSHED)
	{ //��ť�ǰ���״̬
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //��ť�ǵ���״̬

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //���û���ɫ
	}
  SetPenSize(hdc, 2);
  InflateRect(&rc, 0, -1);
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }

}

static void btn_owner_draw(DRAWITEM_HDR *ds) //����һ����ť���
{
	HDC hdc;
	RECT rc, rc_tmp;
  WCHAR wbuf[128];
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button�Ļ�ͼ�����ľ��.
	rc = ds->rc;     //button�Ļ��ƾ�����.

  GetClientRect(hwnd, &rc_tmp);//�õ��ؼ���λ��
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//����ת��

  EnableAntiAlias(hdc, TRUE);
  
  SetBrushColor(hdc, MapRGB(hdc, 66, 254, 255));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));

  if (ds->State & BST_PUSHED)
  { //��ť�ǰ���״̬
    OffsetRect(&rc, 1, 1);
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  { //��ť�ǵ���״̬
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  InflateRect(&rc, -5, -5);
  SetBrushColor(hdc, MapRGB(hdc, 13, 148, 214));
  FillRoundRect(hdc, &rc, MIN(rc.h, rc.w));
  
  EnableAntiAlias(hdc, FALSE);
  
  GetWindowText(ds->hwnd, wbuf, 128); //��ð�ť�ؼ�������
  
  /* ��ʾ�ı� */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
}



static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc);
                      
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  740, 25, 36, 36, hwnd, eID_SUD_EXIT, NULL, NULL);

      CreateWindow(BUTTON, L"����", WS_TRANSPARENT| BS_NOTIFY | WS_VISIBLE | BS_3D|WS_OWNERDRAW,
                  317, 393, 166, 70, hwnd, eID_SUD_LINK, NULL, NULL);    // ʹ��ʱ�ӵİ�ť����
      
      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      res = RES_Load_Content(GUI_UDISK_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//      hdc_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
//      if(res)
//      {
//       /* ����ͼƬ���ݴ���JPG_DEC��� */
//       dec = JPG_Open(jpeg_buf, jpeg_size);

//       /* �������ڴ���� */
//       JPG_Draw(hdc_bk, 0, 0, dec);

//       /* �ر�JPG_DEC��� */
//       JPG_Close(dec);
//      }
      /* �ͷ�ͼƬ���ݿռ� */
      RES_Release_Content((char **)&jpeg_buf);
      
      u8 *pic_buf;
      u32 pic_size;
      PNG_DEC *png_dec;
      BITMAP png_bm;
      

      /* ���� HDC */
      hdc_btn = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 71, 30);
      ClrDisplay(hdc_btn, NULL, 0);
      res = RES_Load_Content(GUI_UDISK_BTN_PIC, (char**)&pic_buf, &pic_size);
      if(res)
      {
        png_dec = PNG_Open(pic_buf, pic_size);
        PNG_GetBitmap(png_dec, &png_bm);
        DrawBitmap(hdc_btn, 0, 0, &png_bm, NULL);
        PNG_Close(png_dec);
      }
      /* �ͷ�ͼƬ���ݿռ� */
      RES_Release_Content((char **)&pic_buf);

      break;
    } 
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // ��ʱ�� ID

      if (tmr_id == 10)    
      {
        
      }
      
      break;
    }

    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc_title = {0, 0, GUI_XSIZE, 80};
      RECT rc_title_grad = {0, 80, GUI_XSIZE, 5};
      RECT rc_lyric = {0, 80, GUI_XSIZE, 400};
//      RECT rc_control = {0, 396, GUI_XSIZE, 84};
      SetBrushColor(hdc, MapRGB(hdc, 1, 218, 254));
      FillRect(hdc, &rc_title);
//         GradientFillRect(hdc, &rc_title, MapRGB(hdc, 1, 218, 254), MapRGB(hdc, 1, 168, 255), FALSE);

      SetFont(hdc, defaultFont);
      SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
      DrawText(hdc, L"�ⲿFLASHģ��U��", -1, &rc_title, DT_VCENTER|DT_CENTER);

      SetBrushColor(hdc, MapRGB(hdc, 240, 240, 240));
      FillRect(hdc, &rc_lyric);
      GradientFillRect(hdc, &rc_title_grad, MapRGB(hdc, 150, 150, 150), MapRGB(hdc, 220, 220, 220), TRUE);

      return FALSE;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT rc  = {0, 150, GUI_XSIZE, 200};

      hdc = BeginPaint(hwnd, &ps);
      
      SetFont(hdc, defaultFont); 
      SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
      SetTextInterval(hdc, -1, 28);
      DrawText(hdc, L"��Ӧ��ʹ���ⲿFLASH�ĺ�512Kģ��U��\r\n���ڵ������ǰʹ��Micro USB\r\n���������ӿ������J24�����ԣ�", -1, &rc, DT_VCENTER|DT_CENTER);//��������(���ж��뷽ʽ)
   
      EndPaint(hwnd, &ps);

      break;
    }

    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_SUD_EXIT:
          {
            _ExitButton_OwnerDraw(ds);
            return TRUE;              
          }  

          case eID_SUD_LINK:
          {
            btn_owner_draw(ds);
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

      switch(id)
       {
        /* �˳���ť���� */
          case eID_SUD_EXIT:
          {
               switch(code)
                {
                    case BN_CLICKED:
                    {
                        PostCloseMessage(hwnd);    // ���͹رմ��ڵ���Ϣ
                    }  
                    break;
                }
          }
          break;

          /* ���Ӱ�ť���� */
          case eID_SUD_LINK:
          {
               switch(code)
                {
                    case BN_CLICKED:
                    {
                        USBD_Init(&USB_OTG_dev,
																	USB_OTG_FS_CORE_ID,
																	&USR_desc,
																	&USBD_MSC_cb, 
																	&USR_cb);
                      
                      SetWindowText(GetDlgItem(hwnd, eID_SUD_LINK), L"������");
                      EnableWindow(GetDlgItem(hwnd, eID_SUD_LINK), FALSE);
                    }  
                    break;
                }
          }
          break;
       }
      
      break;
    } 

    case WM_DESTROY:
    { 
//			DeleteDC(hdc_bk);
      DeleteDC(hdc_btn);
//      DeleteDC(hdc_btn_press);
      DCD_DevDisconnect(&USB_OTG_dev);
      USB_OTG_STOP();
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_SimulateUDisk_Dialog(void)
{
	HWND SUD_Main_Handle;
	WNDCLASS	wcex;
	MSG msg;
  
	wcex.Tag = WNDCLASS_TAG;
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//����������
	SUD_Main_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI Simulate U Disk Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);

   //��ʾ������
	ShowWindow(SUD_Main_Handle, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, SUD_Main_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


