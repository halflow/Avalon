#include "BPC_Decode.h"
#include "stm32f10x_tim.h"
#include "sys.h"
#include "key.h"
#include "timer.h"
#include "stm32f10x_gpio.h"
#include "Clock.h"
#include "Display.h"
#include "delay.h"

u16 Decode_count=0,Decode_pre_count=0; 
unsigned char Decode_start=0;                         //0??????P1;1????????
unsigned char Decode_RE=0;
unsigned char Decode_OK=0;                            //0????????;1??????
unsigned char Decode_RE_Find=0;               //ÿ������У׼
unsigned char Check_table[4]={0,1,1,2};//1?????
unsigned char OE_CheckT=0,OE_CheckB=0,OE_Check=0;                   //????,1???
unsigned char PP1=0,PP2=0,PP3=0,PP4=0;
unsigned char Hour[2]={0,0},Min[3]={0,0,0},Week[2]={0,0},Day[3]={0,0,0},Month[2]={0,0},Year[4]={0,0,0,0};    //??????,????????????
unsigned char MON=5,DDD=15,HHH=1,MMM=1,SSS=1,WWW=1;                  //???????,??,?
u16 YYY=2017;
unsigned int x=0,y=0,z=0,i=0,j=0,k=0;
unsigned char AUTO_BPC=0;                 //��ҹ�Զ�Уʱ�ɹ���־	
	
void TIM3_Int_Init(u16 arr,u16 psc){

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
  TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
}

void IO_Init(void){
  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);       //����PBʱ�ӣ���ΪJTAGռ����PB���ţ���ʵ��䲻��Ҫ
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);        //��������ʱ�Ӳ��ܽ�ֹJTAG
  delay_ms(2);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //��ֹJTAG

	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;          //BCP_IN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;          
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;            //LED��ʹ�õ���JTAG�����ţ�ǰ���Ѿ���ֹ��JTAG����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;               
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          //BPC_EN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;               
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;          //BPC_EN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;               
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);     //�ر�����ʱ�� GPIOB�͸���ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 

}

void BPC_Decode(void){
//	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//	TIM_Cmd(TIM3,DISABLE); //���ܶ�ʱ��3
//  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //��ֹ��ʱ��3�����ж�
	Show_STOP();
	BPC_EN=1;            //BPCģ��ʹ��	
	LED=1;	
	Decode_count=0;
	Decode_pre_count=0;
	Decode_RE_Find=0;     //Ѱ�ұ�����ʼ��˳��Ѱ��֡ͷ
	Decode_RE=0;        //��Ҫ��������12.5ms
	Decode_start=0;              //0?????P1
	Decode_OK=0;                 //0??????????
  OE_CheckT=0;
//	IO_Init();
	Close();                       //�ص�
 //	CLK=1;DIN1=1;DIN2=1;DIN3=1;DIN4=1;
	delay_ms(1000);           //�ȴ�BPCģ������

	TIM3_Int_Init(100-1,72-1);  //��ʱ100us,��Ƶ72��1MHz
}

void BPC_Decode_RE(void){   //��У�������ʧ�����¿�ʼ��ʹ�ô˺����������ظ��صƣ����Ч�ʣ�������һ֡
	LED=1;	
	Decode_count=0;
	Decode_pre_count=0;
	Decode_RE_Find=0;     //Ѱ�ұ�����ʼ��˳��Ѱ��֡ͷ
	Decode_RE=0;        //��Ҫ��������12.5ms
	Decode_start=0;              //0?????P1
	Decode_OK=0;                 //0??????????
  OE_CheckT=0;
  TIM3_Int_Init(100-1,72-1);  //��ʱ100us,��Ƶ72��1MHz
}

void Find_Head_100us(void){
//  	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	  TIM_Cmd(TIM3,DISABLE); //���ܶ�ʱ��3
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //��ֹ��ʱ��3�����ж�
		TIM3_Int_Init(100-1,72-1);  //��ʱ100us,��Ƶ72��1MHz
}

void Head_6_25ms(void){
//	  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	  TIM_Cmd(TIM3,DISABLE); //���ܶ�ʱ��3
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //��ֹ��ʱ��3�����ж�
		TIM3_Int_Init(625-1,720-1);  //��ʱ6.25ms,0.1s�ڲ���8�Σ���ƽ�⣡5��������1����þ�Ϊ1	
}

void Calibration_12_5ms(void){
	  Decode_RE=0;
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_Cmd(TIM3,DISABLE); //���ܶ�ʱ��3
  	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //��ֹ��ʱ��3�����ж�
		TIM3_Int_Init(1250-1,720-1);  //12.5ms
}

void TIM3_IRQHandler(void)               //100us�ж�
{
 if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //����ж�
		if(Decode_start==0){                   //Decode_start==0   
		  if(BPC_IN==0){                    //Ѱ��֡ͷ������2.1s�����ж�Ѱ��ʧ�ܣ�����Ѱ��      
          Decode_pre_count++;
					if(Decode_pre_count > 21000){Decode_pre_count=0;}					//ԭ����5s
      }    
			else{                              //֡ͷP1Ӧ������һ�γ��ָߵ�ƽ��1.6~1.9s֮�����      
					if(Decode_pre_count>11000){        //������֮֡��ļ��Ϊ0.6~0.9s����1.1sΪ��ȫ��ֵ   //ԭ����1.25s 
							Decode_pre_count=0;           
							Decode_count=0;
							Decode_start=1;           //��ʼ����,Decode_start==1					
							Decode_RE_Find=0;         //����Ѱ��֡ͷ
							i=0;k=0;
              Head_6_25ms();						//��һ�β�����6.25ms��֮��������Ϊ12.5ms
					}					
					else{
					  Decode_pre_count=0;
					}        //?0??????
      }             
	  }	                             
	 else                       //Decode_start==1������У׼
	   {
//			if(Decode_RE==1){  //����У׼��ʱ��12.5ms       
//        Calibration_12_5ms();		   
//			}
			
      if(Decode_RE_Find){     //����Ѱ��֡ͷ,100us
			  if(BPC_IN==0){                    //���źţ�����    
          Decode_pre_count++;
					if(Decode_pre_count > 10000){     //1s��ʧ�����¿�ʼ�����뻯���0.9s�ڶ��ǵͣ�Ȼ����һ֡����						
             BPC_Decode_RE();
					}					
        }    
			  else{                              //BPC_IN==1 
					if(Decode_pre_count>5000){         //���ڰ�ȫ��ֵ//���뻯�����0.6s�ڶ��ǵ�	  
							Decode_pre_count=0;           //?1??????
							Decode_start=1;           //��ʼУ׼             							
						  Decode_RE_Find=0;	       //����Ѱ��֡ͷ
						  i=0;k=0;
              Head_6_25ms();										
					}					
					else if((Decode_pre_count > 1000) && (Decode_pre_count < 5000)){ //�ڰ�ȫ��ֵ�ڳ��ָߵ�ƽ��˵�����ȶ�,���¿�ʼ
              BPC_Decode_RE();
					}        //��һ֡�ս���ʱDecode_pre_count��С���Թ����ȴ��͵�ƽ
        }             
			}
 			else{         //����У׼ 
        x=Decode_count/(u16)32;       //�ж�����һ����         0~18
		    y=Decode_count%(u16)32;   //һ�����ڼ�����         0~31
		    z=y/(u16)8;                   //һ�����ڶ�λ�ĸ�0.1s   0~3
		    j=(y%(u16)8);                   //0.1s�ڵ��ĸ�λ     0~7
		 		
        Decode_count++;
				
				if(y==0){       //����ÿ֡��֡ͷ
				  Calibration_12_5ms();     //��Ҫ��������12.5ms
				}
				
				if(BPC_IN){i++;}          //����                   		    
		  	if(j==7){                          //0.1s�����һ�������㣬j==7		      
				  	if(i>=5){k++;}            //�þ� ,���ڵ���5��1�ж�Ϊ1
				    if(z==3){                  //1s��ǰ0.4sȫ���ɼ���						  
							  switch(x){							  
									  case 0:PP1=(k-1);
                           if(PP1==3){      //ʧ��
													   PP1=0;					  
                             BPC_Decode_RE();
														 i=0;k=0;
													 }
													 else{
														 LED=0;
													   Find_Head_100us();
														 Decode_RE_Find=1;      //����Ѱ��֡ͷ
													 }
													 break;
									  case 1:PP2=(k-1);LED=1;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
													 break;
                    case 2:Hour[0]=(k-1);LED=0;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
													 break;
										case 3:Hour[1]=(k-1);LED=1;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
													 break;
										case 4:Min[0]=(k-1);LED=0;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
													 break;
										case 5:Min[1]=(k-1);LED=1;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
													 break;
										case 6:Min[2]=(k-1);LED=0;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
										       break;
										case 7:Week[0]=(k-1);LED=1;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
										       break;
										case 8:Week[1]=(k-1);LED=0;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
										       break;
										case 9:PP3=(k-1);LED=1;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //����Ѱ��֡ͷ
										       break;
										case 10:Day[0]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 11:Day[1]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 12:Day[2]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 13:Month[0]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 14:Month[1]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 15:Year[0]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 16:Year[1]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 17:Year[2]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //����Ѱ��֡ͷ
										        break;
										case 18:Year[3]=(k-1);LED=0;
										        break;
									  default:break;						
									}
								k=0;                         //׼��������һ����Ľ���λ
							}
					  i=0;
					}
				else if(j>7){
					i=0;k=0;
				  BPC_Decode_RE();  //���ִ���,���¿�ʼ
				}			  
			}
			    			
		 if(Decode_count>607)            //��⵽P4��У��ɹ����˳���������ͬ����ȡ607��P4�źŵĽ�����	
       {                              //��Decode_countΪ608ʱ����ʵ��P4�Ľ���,����0.4s����6.25ms
			    OE_CheckT=Check_table[PP1]+Check_table[PP2];				                //?????
				  OE_CheckT+=Check_table[Hour[0]];
				  OE_CheckT+=Check_table[Hour[1]];
					OE_CheckT+=Check_table[Min[0]];
					OE_CheckT+=Check_table[Min[1]];
					OE_CheckT+=Check_table[Min[2]];
					OE_CheckT+=Check_table[Week[0]];
					OE_CheckT+=Check_table[Week[1]];
				  if((PP3%2)==(OE_CheckT%2)){OE_CheckT=1;}   //ǰ���У��ɹ�
				  else{OE_CheckT=0;}
					
					OE_CheckB+=Check_table[Day[0]];
				  OE_CheckB+=Check_table[Day[1]];
				  OE_CheckB+=Check_table[Day[2]];
				  OE_CheckB+=Check_table[Month[0]];
				  OE_CheckB+=Check_table[Month[1]];
				  OE_CheckB+=Check_table[Year[0]];
				  OE_CheckB+=Check_table[Year[1]];
				  OE_CheckB+=Check_table[Year[2]];
					if((Year[3]%2)==(OE_CheckB%2)){OE_CheckB=1;}  //����У��ɹ�
					else{OE_CheckB=0;}
					
					if(OE_CheckT && OE_CheckB)              //У��ɹ�
						{
							LED=0;
							BPC_EN=0;         //BPCģ�����
						  HHH=((Hour[0]*4)+Hour[1]);													
			        MMM=((Min[0]*16)+(Min[1]*4)+Min[2]);
							SSS=((PP1*20)+20);      //ȡ���������뷢����ʱ��+20s��·��300km����ʱԼ1ms
							        //��Decode_countΪ608ʱ����ʵ��P4�źŽ���ǰ6.25ms��������1ms
							        //���������ʱ604.75ms(��ʵ��Ѱ��֡ͷ��ʱ�������������100us)
							WWW=((Week[0]*4)+Week[1]);
							DDD=((Day[0]*16)+(Day[1]*4)+Day[2]);
							MON=((Month[0]*4)+Month[1]);
              if(Year[3]<2){Year[3]=0;}    //ת��P4
							else{Year[3]=1;}
							YYY=((Year[0]*16)+(Year[1]*4)+Year[2]+Year[3]*64)+2000;
              if(PP3==1){PP3=0;}           //PP3==0��ʾ��ǰ��PP3!=0��ʾ���
							if(PP3!=0){HHH+=12;} //������ʱ̨������12�㶨Ϊ���0�㣬��Ҫת��
						  if(SSS==60){
							  SSS=0;
							  MMM++;
				        if(MMM==60){
								  MMM=0;
									HHH++;
									if(HHH==24){HHH=0;}
									if((HHH >= 0) && (HHH < 12)){    //������ָ�����޷�������һ����>=0,(HHH >= 0)��������
									PP3=0;
									}
								}
							}
							Decode_start=0;
							Decode_count=0;
							OE_CheckT=0;
							Decode_OK=1;
							if(AUTO_BPC==0){AUTO_BPC=1;}            //��ǰ����Уʱ�ɹ������
							TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
					    TIM_Cmd(TIM3,DISABLE); //���ܶ�ʱ��3
					    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //��ֹ��ʱ��3�����ж�
			        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,DISABLE);  ///����TIM3ʱ��
						}
          else{                                  //У��ʧ�� 						
            BPC_Decode_RE();
 					}					
				}							 
   }
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);       //�����жϱ�־λ����	 
 }
}

