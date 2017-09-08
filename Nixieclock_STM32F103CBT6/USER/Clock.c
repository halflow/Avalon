#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "BPC_Decode.h"
#include "Clock.h"
#include "Display.h"
#include "rtc.h"

//////////////////////////////////////////////////////

//时钟计时   显示放在中断里面，中断时间尽量长100ms以上 用timer4

//////////////////////////////////////////////////////

u8 doki_doki=0;
u8 clock_or_number=0;
u8 nnn[6];

void TIM4_Int_Init(u16 arr,u16 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM4时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化TIM4
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许定时器4更新中断

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1          //比BPC解码的计时中断低,与按键相同
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
  TIM_Cmd(TIM4,ENABLE); //使能定时器4
}

void DokiDoki_BPC(void){           //同步时间成功后，计时
	doki_doki=0;
  TIM4_Int_Init(10000-1,720-1);    //定时器4定时100ms
	Display_C();	                   //串行输出
	RTC_Init();                      //刷新RTC的时间		                     
}

void DokiDoki_RTC(void){           //开始RTC计时
	doki_doki=0;
	RTC_Get();              //读取时间
	HHH=calendar.hour;
	MMM=calendar.min;
	SSS=calendar.sec;
	if(HHH < 12){           //上午		
		PP3=0;
	}
	else{                   //下午
		PP3=2;
	}	
  TIM4_Int_Init(10000-1,720-1);           //定时器4定时100ms
	doki_doki=0;
	DOT1=1;
	Display_C();	                          //串行输出
}

void TIM4_IRQHandler(void){          //timer4定时100ms
	u8 iii;
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET){ //溢出中断  
	  doki_doki++;
	  if(doki_doki==10){
		  doki_doki=0;
		  SSS++;
	    if(SSS==60){
		    SSS=0;
		    MMM++;
			  if(((MMM%5)==0) && (SSS==0)){    //5分钟防止阴极中毒
		      clock_or_number=1;
		    }			
	      if(MMM==60){
		      MMM=0;
		      if((HHH==23)&&(PP3!=0)){HHH=0;PP3=0;}    //之前是午后11点59分，现在是午前0点
		      else if((HHH==11)&&(PP3==0)){HHH++;PP3=2;}   //之前是午前11点59分，现在是午后12点
		      else if((HHH==2)&&(PP3==0)){AUTO_BPC=0;HHH++;}   //午前2点校时成功，现在午前3点，将成功标志位清零
          else {HHH++;}
	      }
			}	  
		}
					
	  if(doki_doki==0){
      if(!clock_or_number){	 //正常显示时钟		
			  Display_C();
			}
			else{   //防止阴极中毒			  
				for(iii=0;iii<6;iii++){
				  nnn[iii]=(clock_or_number-1);
				}
				show6num(nnn);
				if((++clock_or_number) > 10){
				  clock_or_number=0;
				}				
			}
			DOT1=1;
	  }
	  if(doki_doki==4){	
			DOT1=0;
	  }
	}	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);       //更新中断标志位清零	 	
}


