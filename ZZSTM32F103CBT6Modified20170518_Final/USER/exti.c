#include "exti.h"
#include "stm32f10x.h"
#include "delay.h"
#include "BPC_Decode.h"
#include "Clock.h"
#include "Display.h"
#include "key.h"
#include "rng.h"
#include "rtc.h"

//Íâ²¿ÖĞ¶Ï0·şÎñ³ÌĞò£¬ÈÕÀú
void EXTI0_IRQHandler(void){
	delay_ms(100);	//Ïû¶¶
	if(CAL==1){	 
		   BPC_STOP();        //BPCÄ£¿é½ûÄÜ		  	 
		   Clock_STOP();      //Í£Ö¹Ê±ÖÓ	  
		   Show_STOP();       //Í£Ö¹ìÅÒ«		
//	     show_CAL=1;
		   Display_CAL();	   //ÏÔÊ¾ÈÕÀú 
//			 show_CAL=0;
			 DokiDoki_RTC();    //ÏÔÊ¾RTCÊ±¼ä
	}
	delay_ms(5);	//Ïû¶¶
	EXTI_ClearITPendingBit(EXTI_Line0);  //Çå³ıLINE0ÉÏµÄÖĞ¶Ï±êÖ¾Î»  
}
//Íâ²¿ÖĞ¶Ï3·şÎñ³ÌĞò£¬Ëæ»úÊı
void EXTI3_IRQHandler(void){
	delay_ms(100);	//Ïû¶¶
	if(SHOW==0){
    BPC_STOP();        //BPCÄ£¿é½ûÄÜ			
    Clock_STOP();     //Í£Ö¹Ê±ÖÓ
		show_flag=1;
	}		 
//	delay_ms(5);	//Ïû¶¶
	EXTI_ClearITPendingBit(EXTI_Line3);  //Çå³ıLINE3ÉÏµÄÖĞ¶Ï±êÖ¾Î»  
}
//Íâ²¿ÖĞ¶Ï4·şÎñ³ÌĞò£¬RTC
void EXTI4_IRQHandler(void){
	delay_ms(100);	//Ïû¶¶
	if(RTC_SWITCH==0){	   
    BPC_STOP();       //BPCÄ£¿é½ûÄÜ			
		Show_STOP();      //Í£Ö¹ìÅÒ«
    Clock_STOP();     //Í£Ö¹Ê±ÖÓ
		DokiDoki_RTC();   //ÏÔÊ¾RTCÊ±¼ä
	}		 
  delay_ms(50);	//Ïû¶¶
	EXTI_ClearITPendingBit(EXTI_Line4);  //Çå³ıLINE4ÉÏµÄÖĞ¶Ï±êÖ¾Î»  
}
	   
//Íâ²¿ÖĞ¶Ï³õÊ¼»¯³ÌĞò
//³õÊ¼»¯PE2~4,PA0ÎªÖĞ¶ÏÊäÈë.
void EXTIX_Init(void){ 
	
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//Ê¹ÄÜ¸´ÓÃ¹¦ÄÜÊ±ÖÓ

   //GPIOA.3	  ÖĞ¶ÏÏßÒÔ¼°ÖĞ¶Ï³õÊ¼»¯ÅäÖÃ ÏÂ½µÑØ´¥·¢ //SHOW
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//¸ù¾İEXTI_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèEXTI¼Ä´æÆ÷

   //GPIOA.4	  ÖĞ¶ÏÏßÒÔ¼°ÖĞ¶Ï³õÊ¼»¯ÅäÖÃ  ÏÂ½µÑØ´¥·¢	//RTC
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);	  	//¸ù¾İEXTI_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèEXTI¼Ä´æÆ÷


   //GPIOA.0	  ÖĞ¶ÏÏßÒÔ¼°ÖĞ¶Ï³õÊ¼»¯ÅäÖÃ ÉÏÉıÑØ´¥·¢ PA0  WK_UP
 	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 

  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);		//¸ù¾İEXTI_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèEXTI¼Ä´æÆ÷


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//Ê¹ÄÜ°´¼üWK_UPËùÔÚµÄÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//ÇÀÕ¼ÓÅÏÈ¼¶1£¬ 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//×ÓÓÅÏÈ¼¶2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Ê¹ÄÜÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_Init(&NVIC_InitStructure); 

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//Ê¹ÄÜ°´¼üKEY1ËùÔÚµÄÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//ÇÀÕ¼ÓÅÏÈ¼¶1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//×ÓÓÅÏÈ¼22 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Ê¹ÄÜÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_Init(&NVIC_InitStructure);  	  //¸ù¾İNVIC_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//Ê¹ÄÜ°´¼üKEY0ËùÔÚµÄÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//ÇÀÕ¼ÓÅÏÈ¼¶1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//×ÓÓÅÏÈ¼¶2 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//Ê¹ÄÜÍâ²¿ÖĞ¶ÏÍ¨µÀ
  	NVIC_Init(&NVIC_InitStructure);  	  //¸ù¾İNVIC_InitStructÖĞÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ÍâÉèNVIC¼Ä´æÆ÷	 
}












