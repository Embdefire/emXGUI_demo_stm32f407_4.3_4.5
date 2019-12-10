#include "emXGUI.h"
#include "x_libc.h"
#include "./led/bsp_led.h"  

  /*
	������������������������������������
  �����̵�LED�Ƹ�ͨ��ʹ�õĲ���ͬһ����ʱ��
  ���Ӳ��ʱ����ʹ��ͬһ����ʱ����������յ�ƣ��򻯴���
	��ʹ�õ�Ӳ����ͬһ����ʱ������Ҫ����Դ�����ͷ�ļ�������ֱ���޸ĺ���ʵ��
	�ر��Ƕ�ʱ����ʼ�����֡�
	������������������������������������
*/
/********************��ʱ��ͨ��**************************/
#define COLOR_TIM_GPIO_CLK                 (RCC_AHB1Periph_GPIOF)

/************���***************/
#define COLOR_RED_TIM           						TIM10
#define COLOR_RED_TIM_CLK       						RCC_APB2Periph_TIM10
#define COLOR_RED_TIM_APBxClock_FUN         RCC_APB2PeriphClockCmd
/*����˵����c�ļ�*/
/*����ͨ�ö�ʱ����ʱ��ΪHCLK/4������ΪHCLK/2��ע��Ҫ������ͨ���Ķ�ʱ��Ƶ������Ϊһ��*/
#define COLOR_RED_TIM_PRESCALER					  	(((SystemCoreClock)/1000000)*30-1)

/************�̵�***************/
#define COLOR_GREEN_TIM           						TIM11
#define COLOR_GREEN_TIM_CLK       						RCC_APB2Periph_TIM11
#define COLOR_GREEN_TIM_APBxClock_FUN         RCC_APB2PeriphClockCmd
/*����ͨ�ö�ʱ����ʱ��ΪHCLK/4������ΪHCLK/2��ע��Ҫ������ͨ���Ķ�ʱ��Ƶ������Ϊһ��*/
#define COLOR_GREEN_TIM_PRESCALER					  	(((SystemCoreClock)/1000000)*30-1)

/************����***************/
#define COLOR_BLUE_TIM           							TIM13
#define COLOR_BLUE_TIM_CLK       					   	RCC_APB1Periph_TIM13
#define COLOR_BLUE_TIM_APBxClock_FUN          RCC_APB1PeriphClockCmd
/*����ͨ�ö�ʱ����ʱ��ΪHCLK/4������ΪHCLK/2��ע��Ҫ������ͨ���Ķ�ʱ��Ƶ������Ϊһ��*/
#define COLOR_BLUE_TIM_PRESCALER					   	(((SystemCoreClock/2)/1000000)*30-1)


#define PWM_LEDR_GPIO_PORT         GPIOF
#define PWM_LEDR_GPIO_PIN          GPIO_Pin_6
#define PWM_LEDR_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define PWM_LEDR_PINSOURCE         GPIO_PinSource6
#define PWM_LEDR_AF                GPIO_AF_TIM10
#define  COLOR_RED_TIM_OCxInit                TIM_OC1Init            //ͨ����ʼ������
#define  COLOR_RED_TIM_OCxPreloadConfig       TIM_OC1PreloadConfig //ͨ���������ú���


#define PWM_LEDG_GPIO_PORT         GPIOF
#define PWM_LEDG_GPIO_PIN          GPIO_Pin_7
#define PWM_LEDG_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define PWM_LEDG_PINSOURCE         GPIO_PinSource7
#define PWM_LEDG_AF                GPIO_AF_TIM11
#define  COLOR_GREEN_TIM_OCxInit                TIM_OC1Init            //ͨ����ʼ������
#define  COLOR_GREEN_TIM_OCxPreloadConfig       TIM_OC1PreloadConfig //ͨ���������ú���


#define PWM_LEDB_GPIO_PORT         GPIOF
#define PWM_LEDB_GPIO_PIN          GPIO_Pin_8
#define PWM_LEDB_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define PWM_LEDB_PINSOURCE         GPIO_PinSource8
#define PWM_LEDB_AF                GPIO_AF_TIM13
#define   COLOR_BLUE_TIM_OCxInit              TIM_OC1Init            //ͨ����ʼ������
#define   COLOR_BLUE_TIM_OCxPreloadConfig    TIM_OC1PreloadConfig  //ͨ���������ú���



 /**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
void TIM_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  /* GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(PWM_LEDR_GPIO_CLK|PWM_LEDG_GPIO_CLK|PWM_LEDB_GPIO_CLK, ENABLE); 
  
  GPIO_PinAFConfig(PWM_LEDR_GPIO_PORT,PWM_LEDR_PINSOURCE,PWM_LEDR_AF); 
  GPIO_PinAFConfig(PWM_LEDG_GPIO_PORT,PWM_LEDG_PINSOURCE,PWM_LEDG_AF); 
  GPIO_PinAFConfig(PWM_LEDB_GPIO_PORT,PWM_LEDB_PINSOURCE,PWM_LEDB_AF); 
  
  /* ���ú������õ���PB0���� */
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		    // �����������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(PWM_LEDR_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDG_GPIO_PIN;
  GPIO_Init(PWM_LEDG_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =  PWM_LEDB_GPIO_PIN;
  GPIO_Init(PWM_LEDB_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  ����TIMx�����PWM�źŵ�ģʽ�������ڡ�����
  * @param  ��
  * @retval ��
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> �ж� & TIMxCNT ���¼���
 *                    TIMx_CCR(��ƽ�����仯)
 *
 * �ź�����=(TIMx_ARR +1 ) * ʱ������
 * 
 */
/*    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 */
void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;																				
	
	COLOR_RED_TIM_APBxClock_FUN(COLOR_RED_TIM_CLK,ENABLE);
	COLOR_GREEN_TIM_APBxClock_FUN(COLOR_GREEN_TIM_CLK,ENABLE);
	COLOR_BLUE_TIM_APBxClock_FUN(COLOR_BLUE_TIM_CLK,ENABLE);
	
	
   /* ������ʱ������ */		 
   TIM_TimeBaseStructure.TIM_Period = 255;       							  //����ʱ����0������255����Ϊ266�Σ�Ϊһ����ʱ����
   TIM_TimeBaseStructure.TIM_Prescaler = 2499;	    							//����Ԥ��Ƶ��
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
 
   TIM_TimeBaseInit(COLOR_RED_TIM, &TIM_TimeBaseStructure);
   TIM_TimeBaseInit(COLOR_GREEN_TIM, &TIM_TimeBaseStructure);
	 TIM_TimeBaseInit(COLOR_BLUE_TIM, &TIM_TimeBaseStructure);
	
	
   /* PWMģʽ���� */
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				//����ΪPWMģʽ1
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//ʹ�����
   TIM_OCInitStructure.TIM_Pulse = 0;										  			//���ó�ʼPWM������Ϊ0	
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  //����ʱ������ֵС��CCR1_ValʱΪ�͵�ƽ
 
   TIM_OC1Init(COLOR_RED_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC1PreloadConfig(COLOR_RED_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	

   TIM_OC1Init(COLOR_GREEN_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC1PreloadConfig(COLOR_GREEN_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
   TIM_OC1Init(COLOR_BLUE_TIM, &TIM_OCInitStructure);	 									//ʹ��ͨ��3
   TIM_OC1PreloadConfig(COLOR_BLUE_TIM, TIM_OCPreload_Enable);						//ʹ��Ԥװ��	
  
   TIM_ARRPreloadConfig(COLOR_RED_TIM, ENABLE);			
   TIM_ARRPreloadConfig(COLOR_GREEN_TIM, ENABLE);			
   TIM_ARRPreloadConfig(COLOR_BLUE_TIM, ENABLE);			 										//ʹ��TIM5���ؼĴ���ARR
 
	// ʹ�ܼ�����
	TIM_Cmd(COLOR_RED_TIM, ENABLE);		
	TIM_Cmd(COLOR_GREEN_TIM, ENABLE);
	TIM_Cmd(COLOR_BLUE_TIM, ENABLE);              										//ʹ�ܶ�ʱ��5
	
}

//RGBLED��ʾ��ɫ
void SetRGBColor(uint32_t rgb)
{
	uint8_t r=0,g=0,b=0;
	r=(uint8_t)(rgb>>16);
	g=(uint8_t)(rgb>>8);
	b=(uint8_t)rgb;
	COLOR_RED_TIM->CCR1 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	COLOR_GREEN_TIM->CCR1 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	COLOR_BLUE_TIM->CCR1 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}

//RGBLED��ʾ��ɫ
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
	COLOR_RED_TIM->CCR1 = r;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
	COLOR_GREEN_TIM->CCR1 = g;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ        
	COLOR_BLUE_TIM->CCR1 = b;	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
}

//ֹͣpwm���
void TIM_RGBLED_Close(void)
{
	SetColorValue(0,0,0);
	TIM_ForcedOC1Config(COLOR_RED_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC2Config(COLOR_GREEN_TIM,TIM_ForcedAction_InActive);
	TIM_ForcedOC3Config(COLOR_BLUE_TIM,TIM_ForcedAction_InActive);
	
	TIM_ARRPreloadConfig(COLOR_RED_TIM, DISABLE);
	TIM_ARRPreloadConfig(COLOR_GREEN_TIM, DISABLE);
	TIM_ARRPreloadConfig(COLOR_BLUE_TIM, DISABLE);
	
	TIM_Cmd(COLOR_RED_TIM, DISABLE);                   //ʧ�ܶ�ʱ��
	TIM_Cmd(COLOR_GREEN_TIM, DISABLE);  
	TIM_Cmd(COLOR_BLUE_TIM, DISABLE);  
	
	RCC_APB1PeriphClockCmd(COLOR_RED_TIM_CLK, DISABLE); 	//ʧ�ܶ�ʱ��ʱ��
	RCC_APB1PeriphClockCmd(COLOR_GREEN_TIM_CLK, DISABLE); 
	RCC_APB1PeriphClockCmd(COLOR_BLUE_TIM_CLK, DISABLE); 
	
	LED_GPIO_Config();
}


