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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();
	KEY_Init();    //按键CAL、SHOW、RTC
	EXTIX_Init();
	Display_Init();
	delay_ms(10);
	BPC_Decode();	
  while(1){
		 if(Decode_OK==1){	
			   Decode_OK=0;
				 delay_ms(604);       //传输延时补偿
		     DokiDoki_BPC();      //开始计时
		 }
     
     if((HHH==2)&&(PP3==0)&&(AUTO_BPC==0)){   //午前2点开始自动校时		  
				  Show_STOP();
          Clock_STOP();			    
          Close();            //关灯				 
				  HHH=0xFF;
 				  BPC_Decode();
		 }
	    
		 if(show_flag==1){        //随机数
			 Clock_STOP();     //停止时钟
			 ShowNixie();
		 }

//    if(show_CAL){
//			 show_CAL=0;
//		}		 
	 }
}



