#include "exti.h"
#include "stm32f10x.h"
#include "delay.h"
#include "BPC_Decode.h"
#include "Clock.h"
#include "Display.h"
#include "key.h"
#include "rng.h"
#include "rtc.h"

//�ⲿ�ж�0�����������
void EXTI0_IRQHandler(void){
	delay_ms(100);	//����
	if(CAL==1){	 
		   BPC_STOP();        //BPCģ�����		  	 
		   Clock_STOP();      //ֹͣʱ��	  
		   Show_STOP();       //ֹͣ��ҫ		
//	     show_CAL=1;
		   Display_CAL();	   //��ʾ���� 
//			 show_CAL=0;
			 DokiDoki_RTC();    //��ʾRTCʱ��
	}
	delay_ms(5);	//����
	EXTI_ClearITPendingBit(EXTI_Line0);  //���LINE0�ϵ��жϱ�־λ  
}
//�ⲿ�ж�3������������
void EXTI3_IRQHandler(void){
	delay_ms(100);	//����
	if(SHOW==0){
    BPC_STOP();        //BPCģ�����			
    Clock_STOP();     //ֹͣʱ��
		show_flag=1;
	}		 
//	delay_ms(5);	//����
	EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}
//�ⲿ�ж�4�������RTC
void EXTI4_IRQHandler(void){
	delay_ms(100);	//����
	if(RTC_SWITCH==0){	   
    BPC_STOP();       //BPCģ�����			
		Show_STOP();      //ֹͣ��ҫ
    Clock_STOP();     //ֹͣʱ��
		DokiDoki_RTC();   //��ʾRTCʱ��
	}		 
  delay_ms(50);	//����
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
}
	   
//�ⲿ�жϳ�ʼ������
//��ʼ��PE2~4,PA0Ϊ�ж�����.
void EXTIX_Init(void){ 
	
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

   //GPIOA.3	  �ж����Լ��жϳ�ʼ������ �½��ش��� //SHOW
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

   //GPIOA.4	  �ж����Լ��жϳ�ʼ������  �½��ش���	//RTC
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


   //GPIOA.0	  �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
 	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 

  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�1�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ22 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ���KEY0���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	 
}












