/**
  *********************************************************************
  * @file    gui_fs_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �ļ�ϵͳ�ӿڣ���Ҫʵ��FileSystem_Init����
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
#include "gui_fs_port.h"
#include "ff.h"



	/* FatFs�ļ�ϵͳ���� */
static FATFS fs ;										

void FileSystem_Test(void);



/**
  * @brief  �ļ�ϵͳ��ʼ��
  * @param  ��
  * @retval TRUE  FALSE
  */
BOOL FileSystem_Init(void)
{ 

#if defined(STM32F429_439xx) || defined(STM32H743xx) || defined(STM32F767xx) || defined(STM32F10X_HD) || defined(STM32F40_41xxx)
	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
  FRESULT res_sd; 
#if defined(STM32F767xx) || defined(STM32H750xx)
  FATFS_LinkDriver(&SD_Driver, SDPath);
#endif
	res_sd = f_mount(&fs,"0:",1);
	
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ...\r\n");
    /* ��ʽ�� */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_sd = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
//			LED_RED;
			printf("������ʽ��ʧ�ܡ�����\r\n");
//			while(1);
		}
    return FALSE;
	}
  else if(res_sd!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd);
    printf("��������ԭ��û�н���SD����\r\n");
    return FALSE;

  }
  else
  {
    printf("���ļ�ϵͳ���سɹ�\r\n");
    
#if 0
    /* �ļ�ϵͳ���� */
    FileSystem_Test();
#endif 
    
    /* ���Խ���unicode����ת����
      ��ʹ��extern_cc936ʱ���ɲ����Ƿ����cc936��Դ*/
    ff_convert('a',1);
    
    
  }
#elif defined(CPU_MIMXRT1052DVL6B)
  f_mount_test(&fs);
#endif
  
  return TRUE;
}

/********************************END OF FILE****************************/

