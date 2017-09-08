#include "stm32f10x.h"
#include "delay.h"
#include "BPC_Decode.h"
#include "Clock.h"
#include "Display.h"
#include "key.h"
#include "rng.h"
#include "rtc.h"
#include "exti.h"
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init();
	KEY_Init();    //����CAL��SHOW��RTC
	EXTIX_Init();
	Display_Init();
	delay_ms(10);
	BPC_Decode();	
  while(1){
		 if(Decode_OK==1){	
			   Decode_OK=0;
				 delay_ms(604);       //������ʱ����
		     DokiDoki_BPC();      //��ʼ��ʱ
		 }
     
     if((HHH==2)&&(PP3==0)&&(AUTO_BPC==0)){   //��ǰ2�㿪ʼ�Զ�Уʱ		  
				  Show_STOP();
          Clock_STOP();			    
          Close();            //�ص�				 
				  HHH=0xFF;
 				  BPC_Decode();
		 }
	    
		 if(show_flag==1){        //�����
			 Clock_STOP();     //ֹͣʱ��
			 ShowNixie();
		 }

//    if(show_CAL){
//			 show_CAL=0;
//		}		 
	 }
}



