#include "stm32f10x.h"
#include "delay.h"
#include "BPC_Decode.h"
#include "Clock.h"
#include "Display.h"
#include "key.h"
#include "rng.h"
#include "rtc.h"

u32 D[13]={0x00000200,0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,0x00000100,0x00000000,0x00000001,0x00000002};   //�������������ʾʱ��ʮλ��0����ʾ,��ʱ����
//u32 D[13]={0xFFFFFE00,0xFFFFFC01,0xFFFFFC02,0xFFFFFC04,0xFFFFFC08,0xFFFFFC10,0xFFFFFC20,0xFFFFFC40,0xFFFFFC80,0xFFFFFD00,0xFFFFFC00,0xFFFFFC01,0xFFFFFC02};
u8 iii=0;
u8 HHH_Change=0;
u8 N[6]={0,0,0,0,0,0};         //�ݴ���Ҫ��ʾ��6������

unsigned char show_flag=0;
unsigned char show_CAL=0;

void Display_Init(void){
  IO_Init();
	DIN1=0;
	DIN2=0;
	DIN3=0;
	DIN4=0;
	LE=1;
	CLK=0;
	DOT1=0;
	BL=1;
}

void show6num(u8 n[6]){           //��ʾ����
	 u32 buf[2]={0x00000000,0x00000000};
	 u8 i=0,j=0;
   buf[0]=((D[n[2]]<<20)|(D[n[1]]<<10)|D[n[0]]);
   buf[1]=((D[n[5]]<<20)|(D[n[4]]<<10)|D[n[3]]);
	 CLK = 0; 
	 LE=1; 
   BL=0;	 
	 for(j=0;j<2;j++){
	   for(i=0;i<8;i++){
			 if(i==7){
         DIN4 = buf[j] & 0x00000001;
         buf[j] = buf[j] >> 1;			 
         DIN3 = buf[j] & 0x00000001;
         buf[j] = buf[j] >> 1;
         DIN2 = buf[1] & 0x00000001;
         buf[1] = buf[1] >> 1;
         DIN1 = buf[1] & 0x00000001;
         buf[1] = buf[1] >> 1;		
			 }
       else{
         DIN4 = buf[j] & 0x00000001;
         buf[j] = buf[j] >> 1;			 
         DIN3 = buf[j] & 0x00000001;
         buf[j] = buf[j] >> 1;
         DIN2 = buf[j] & 0x00000001;
         buf[j] = buf[j] >> 1;
         DIN1 = buf[j] & 0x00000001;
         buf[j] = buf[j] >> 1;					   
			 }			 
			 delay_us(20);		   
			 CLK = 1;
			 delay_us(20);
			 CLK = 0;
		 } 
	 }                            //��������
	 LE=0;
	 delay_us(50);
	 LE=1;   	 
	 delay_us(20);
}

void Display_C(void){     //��ʾʱ��	
	u8 n[6];
	n[5]=(SSS%10);    //���λ
	n[4]=(SSS/10);    //��ʮλ
	n[3]=(MMM%10);    //�ָ�λ
	n[2]=(MMM/10);    //��ʮλ
	n[1]=(HHH%10);    //ʱ��λ
	n[0]=(HHH/10);    //(14,D[((HHH/10)+10)]);    //ʱʮλ
  show6num(n);
}

void Display_CAL(void){
	  u8 n[6],i;
		RTC_Get();//��ȡʱ��
		YYY=calendar.w_year;
		MON=calendar.w_month;
		DDD=calendar.w_date;
		WWW=calendar.week;
	  if(WWW==0){WWW=7;}
	  n[5]=(DDD%10);    //���λ
	  n[4]=(DDD/10);    //��ʮλ
	  n[3]=(MON%10);    //�¸�λ
	  n[2]=(MON/10);    //��ʮλ
	  n[1]=(u8)(YYY%10);    //���λ
	  n[0]=(u8)((YYY%100)/10);    //(14,D[((HHH/10)+10)]);    //��ʮλ
	  show6num(n);    
	  DOT1=1;
	  delay_ms(1500);
    delay_ms(1000);	
	
	  for(i=0;i<6;i++){   //����
		  n[i]=WWW;
		}
		show6num(n);
		DOT1=0;
		delay_ms(1500);
		delay_ms(1000);
}

void ShowNixie(void){               //0��9����
	u8 i=0,j=0;
	u8 n[6];
	j=RNG_Init();	 		//��ʼ��ADC1��8ͨ����PB0
	DOT1=0;
	if(j==0){     //ADC��ʼ���ɹ�	
	  while(show_flag==1){
		   for(i=0;i<6;i++){
					n[i]=(u8)RNG_Get_RandomRange(ADC_Channel_8,0,9);//��ȡ[0,9]����������
					delay_ms(1);
			 }
       show6num(n);			 
			 delay_ms(1000);				 			  
		}
	}
	
 	else{             //ADC��ʼ��ʧ�ܣ���ʾ����
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1	, DISABLE );	  //����ADC1ͨ��ʱ��
	 ADC_Cmd(ADC1, DISABLE);	//����ָ����ADC1
	 while(show_flag==1){
		 for(j=0;j<10;j++){
		   for(i=0;i<6;i++){
			    n[i]=i;
		   }
       show6num(n);	
			 delay_ms(1000);
		 }
	 }
	}	 
}


void Close(void){         //�رյ�Դ
	u8 n[6]={0,0,0,0,0,0};
	show6num(n);
	DOT1=0;
	BL=1;
}

void Open(void){
	DOT1=1; 
}

void Clock_STOP(void){      //�ر�ʱ��
	TIM_Cmd(TIM4,DISABLE); //���ܶ�ʱ��4
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE); //��ֹ��ʱ��4�����ж�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,DISABLE);  ///����TIM4ʱ��  
	DOT1=0;
	BL=1;
}

void BPC_STOP(void){       //ֹͣBPCУ��
  LED=0;
	BPC_EN=0; 
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //��ֹ��ʱ��3�����ж�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,DISABLE);  ///����TIM3ʱ�� 
	TIM_Cmd(TIM3,DISABLE); //���ܶ�ʱ��3
}

void Show_STOP(void){      //��ֹ��ҫ
  show_flag=0;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1,DISABLE);	  //����ADC1ͨ��ʱ��
  ADC_Cmd(ADC1, DISABLE);	//����ָ����ADC1
}
