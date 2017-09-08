#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "BPC_Decode.h"
#include "Clock.h"
#include "Display.h"
#include "rtc.h"

//////////////////////////////////////////////////////

//ʱ�Ӽ�ʱ   ��ʾ�����ж����棬�ж�ʱ�価����100ms���� ��timer4

//////////////////////////////////////////////////////

u8 doki_doki=0;
u8 clock_or_number=0;
u8 nnn[6];

void TIM4_Int_Init(u16 arr,u16 psc){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///ʹ��TIM4ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//��ʼ��TIM4
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //����ʱ��4�����ж�

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1          //��BPC����ļ�ʱ�жϵ�,�밴����ͬ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
  TIM_Cmd(TIM4,ENABLE); //ʹ�ܶ�ʱ��4
}

void DokiDoki_BPC(void){           //ͬ��ʱ��ɹ��󣬼�ʱ
	doki_doki=0;
  TIM4_Int_Init(10000-1,720-1);    //��ʱ��4��ʱ100ms
	Display_C();	                   //�������
	RTC_Init();                      //ˢ��RTC��ʱ��		                     
}

void DokiDoki_RTC(void){           //��ʼRTC��ʱ
	doki_doki=0;
	RTC_Get();              //��ȡʱ��
	HHH=calendar.hour;
	MMM=calendar.min;
	SSS=calendar.sec;
	if(HHH < 12){           //����		
		PP3=0;
	}
	else{                   //����
		PP3=2;
	}	
  TIM4_Int_Init(10000-1,720-1);           //��ʱ��4��ʱ100ms
	doki_doki=0;
	DOT1=1;
	Display_C();	                          //�������
}

void TIM4_IRQHandler(void){          //timer4��ʱ100ms
	u8 iii;
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET){ //����ж�  
	  doki_doki++;
	  if(doki_doki==10){
		  doki_doki=0;
		  SSS++;
	    if(SSS==60){
		    SSS=0;
		    MMM++;
			  if(((MMM%5)==0) && (SSS==0)){    //5���ӷ�ֹ�����ж�
		      clock_or_number=1;
		    }			
	      if(MMM==60){
		      MMM=0;
		      if((HHH==23)&&(PP3!=0)){HHH=0;PP3=0;}    //֮ǰ�����11��59�֣���������ǰ0��
		      else if((HHH==11)&&(PP3==0)){HHH++;PP3=2;}   //֮ǰ����ǰ11��59�֣����������12��
		      else if((HHH==2)&&(PP3==0)){AUTO_BPC=0;HHH++;}   //��ǰ2��Уʱ�ɹ���������ǰ3�㣬���ɹ���־λ����
          else {HHH++;}
	      }
			}	  
		}
					
	  if(doki_doki==0){
      if(!clock_or_number){	 //������ʾʱ��		
			  Display_C();
			}
			else{   //��ֹ�����ж�			  
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
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);       //�����жϱ�־λ����	 	
}


