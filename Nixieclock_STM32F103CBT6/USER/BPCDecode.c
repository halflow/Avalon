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
unsigned char Decode_RE_Find=0;               //每秒重新校准
unsigned char Check_table[4]={0,1,1,2};//1?????
unsigned char OE_CheckT=0,OE_CheckB=0,OE_Check=0;                   //????,1???
unsigned char PP1=0,PP2=0,PP3=0,PP4=0;
unsigned char Hour[2]={0,0},Min[3]={0,0,0},Week[2]={0,0},Day[3]={0,0,0},Month[2]={0,0},Year[4]={0,0,0,0};    //??????,????????????
unsigned char MON=5,DDD=15,HHH=1,MMM=1,SSS=1,WWW=1;                  //???????,??,?
u16 YYY=2017;
unsigned int x=0,y=0,z=0,i=0,j=0,k=0;
unsigned char AUTO_BPC=0;                 //午夜自动校时成功标志	
	
void TIM3_Int_Init(u16 arr,u16 psc){

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
  TIM_Cmd(TIM3,ENABLE); //使能定时器3
}

void IO_Init(void){
  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);       //开启PB时钟，因为JTAG占用了PB引脚，其实这句不需要
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);        //开启复用时钟才能禁止JTAG
  delay_ms(2);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //禁止JTAG

	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;          //BCP_IN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;          
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;            //LED，使用的是JTAG的引脚，前面已经禁止了JTAG复用
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;               
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;          //BPC_EN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;               
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;          //BPC_EN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;               
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);     //关闭两个时钟 GPIOB和复用时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 

}

void BPC_Decode(void){
//	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//	TIM_Cmd(TIM3,DISABLE); //禁能定时器3
//  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //禁止定时器3更新中断
	Show_STOP();
	BPC_EN=1;            //BPC模块使能	
	LED=1;	
	Decode_count=0;
	Decode_pre_count=0;
	Decode_RE_Find=0;     //寻找报文起始，顺便寻找帧头
	Decode_RE=0;        //需要重新设置12.5ms
	Decode_start=0;              //0?????P1
	Decode_OK=0;                 //0??????????
  OE_CheckT=0;
//	IO_Init();
	Close();                       //关灯
 //	CLK=1;DIN1=1;DIN2=1;DIN3=1;DIN4=1;
	delay_ms(1000);           //等待BPC模块启动

	TIM3_Int_Init(100-1,72-1);  //定时100us,分频72，1MHz
}

void BPC_Decode_RE(void){   //在校验过程中失败重新开始，使用此函数，避免重复关灯，提高效率，紧接下一帧
	LED=1;	
	Decode_count=0;
	Decode_pre_count=0;
	Decode_RE_Find=0;     //寻找报文起始，顺便寻找帧头
	Decode_RE=0;        //需要重新设置12.5ms
	Decode_start=0;              //0?????P1
	Decode_OK=0;                 //0??????????
  OE_CheckT=0;
  TIM3_Int_Init(100-1,72-1);  //定时100us,分频72，1MHz
}

void Find_Head_100us(void){
//  	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	  TIM_Cmd(TIM3,DISABLE); //禁能定时器3
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //禁止定时器3更新中断
		TIM3_Int_Init(100-1,72-1);  //定时100us,分频72，1MHz
}

void Head_6_25ms(void){
//	  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	  TIM_Cmd(TIM3,DISABLE); //禁能定时器3
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //禁止定时器3更新中断
		TIM3_Int_Init(625-1,720-1);  //定时6.25ms,0.1s内采样8次，很平衡！5次以上是1，则裁决为1	
}

void Calibration_12_5ms(void){
	  Decode_RE=0;
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_Cmd(TIM3,DISABLE); //禁能定时器3
  	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //禁止定时器3更新中断
		TIM3_Int_Init(1250-1,720-1);  //12.5ms
}

void TIM3_IRQHandler(void)               //100us中断
{
 if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //溢出中断
		if(Decode_start==0){                   //Decode_start==0   
		  if(BPC_IN==0){                    //寻找帧头，大于2.1s，则判定寻找失败，重新寻找      
          Decode_pre_count++;
					if(Decode_pre_count > 21000){Decode_pre_count=0;}					//原来是5s
      }    
			else{                              //帧头P1应该在上一次出现高电平后1.6~1.9s之间出现      
					if(Decode_pre_count>11000){        //正常两帧之间的间隔为0.6~0.9s，设1.1s为安全阈值   //原来是1.25s 
							Decode_pre_count=0;           
							Decode_count=0;
							Decode_start=1;           //开始采样,Decode_start==1					
							Decode_RE_Find=0;         //不再寻找帧头
							i=0;k=0;
              Head_6_25ms();						//第一次采样在6.25ms，之后采样间隔为12.5ms
					}					
					else{
					  Decode_pre_count=0;
					}        //?0??????
      }             
	  }	                             
	 else                       //Decode_start==1，正在校准
	   {
//			if(Decode_RE==1){  //重新校准定时器12.5ms       
//        Calibration_12_5ms();		   
//			}
			
      if(Decode_RE_Find){     //重新寻找帧头,100us
			  if(BPC_IN==0){                    //无信号，计数    
          Decode_pre_count++;
					if(Decode_pre_count > 10000){     //1s丢失，重新开始，理想化在最长0.9s内都是低，然后下一帧来临						
             BPC_Decode_RE();
					}					
        }    
			  else{                              //BPC_IN==1 
					if(Decode_pre_count>5000){         //大于安全阈值//理想化在最短0.6s内都是低	  
							Decode_pre_count=0;           //?1??????
							Decode_start=1;           //开始校准             							
						  Decode_RE_Find=0;	       //不再寻找帧头
						  i=0;k=0;
              Head_6_25ms();										
					}					
					else if((Decode_pre_count > 1000) && (Decode_pre_count < 5000)){ //在安全阈值内出现高电平，说明不稳定,重新开始
              BPC_Decode_RE();
					}        //上一帧刚结束时Decode_pre_count很小，略过，等待低电平
        }             
			}
 			else{         //正常校准 
        x=Decode_count/(u16)32;       //判断是哪一秒钟         0~18
		    y=Decode_count%(u16)32;   //一秒钟内计总数         0~31
		    z=y/(u16)8;                   //一秒钟内定位哪个0.1s   0~3
		    j=(y%(u16)8);                   //0.1s内的哪个位     0~7
		 		
        Decode_count++;
				
				if(y==0){       //这是每帧的帧头
				  Calibration_12_5ms();     //需要重新设置12.5ms
				}
				
				if(BPC_IN){i++;}          //采样                   		    
		  	if(j==7){                          //0.1s内最后一个采样点，j==7		      
				  	if(i>=5){k++;}            //裁决 ,大于等于5个1判断为1
				    if(z==3){                  //1s内前0.4s全部采集完						  
							  switch(x){							  
									  case 0:PP1=(k-1);
                           if(PP1==3){      //失败
													   PP1=0;					  
                             BPC_Decode_RE();
														 i=0;k=0;
													 }
													 else{
														 LED=0;
													   Find_Head_100us();
														 Decode_RE_Find=1;      //重新寻找帧头
													 }
													 break;
									  case 1:PP2=(k-1);LED=1;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
													 break;
                    case 2:Hour[0]=(k-1);LED=0;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
													 break;
										case 3:Hour[1]=(k-1);LED=1;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
													 break;
										case 4:Min[0]=(k-1);LED=0;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
													 break;
										case 5:Min[1]=(k-1);LED=1;
													 Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
													 break;
										case 6:Min[2]=(k-1);LED=0;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
										       break;
										case 7:Week[0]=(k-1);LED=1;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
										       break;
										case 8:Week[1]=(k-1);LED=0;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
										       break;
										case 9:PP3=(k-1);LED=1;
										       Find_Head_100us();
													 Decode_RE_Find=1;       //重新寻找帧头
										       break;
										case 10:Day[0]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 11:Day[1]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 12:Day[2]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 13:Month[0]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 14:Month[1]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 15:Year[0]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 16:Year[1]=(k-1);LED=0;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 17:Year[2]=(k-1);LED=1;
										        Find_Head_100us();
													  Decode_RE_Find=1;       //重新寻找帧头
										        break;
										case 18:Year[3]=(k-1);LED=0;
										        break;
									  default:break;						
									}
								k=0;                         //准备接收下一秒的四进制位
							}
					  i=0;
					}
				else if(j>7){
					i=0;k=0;
				  BPC_Decode_RE();  //出现错误,重新开始
				}			  
			}
			    			
		 if(Decode_count>607)            //检测到P4，校验成功就退出，否则重同步，取607是P4信号的结束，	
       {                              //当Decode_count为608时，其实是P4的结束,距离0.4s还有6.25ms
			    OE_CheckT=Check_table[PP1]+Check_table[PP2];				                //?????
				  OE_CheckT+=Check_table[Hour[0]];
				  OE_CheckT+=Check_table[Hour[1]];
					OE_CheckT+=Check_table[Min[0]];
					OE_CheckT+=Check_table[Min[1]];
					OE_CheckT+=Check_table[Min[2]];
					OE_CheckT+=Check_table[Week[0]];
					OE_CheckT+=Check_table[Week[1]];
				  if((PP3%2)==(OE_CheckT%2)){OE_CheckT=1;}   //前半段校验成功
				  else{OE_CheckT=0;}
					
					OE_CheckB+=Check_table[Day[0]];
				  OE_CheckB+=Check_table[Day[1]];
				  OE_CheckB+=Check_table[Day[2]];
				  OE_CheckB+=Check_table[Month[0]];
				  OE_CheckB+=Check_table[Month[1]];
				  OE_CheckB+=Check_table[Year[0]];
				  OE_CheckB+=Check_table[Year[1]];
				  OE_CheckB+=Check_table[Year[2]];
					if((Year[3]%2)==(OE_CheckB%2)){OE_CheckB=1;}  //后半段校验成功
					else{OE_CheckB=0;}
					
					if(OE_CheckT && OE_CheckB)              //校验成功
						{
							LED=0;
							BPC_EN=0;         //BPC模块禁能
						  HHH=((Hour[0]*4)+Hour[1]);													
			        MMM=((Min[0]*16)+(Min[1]*4)+Min[2]);
							SSS=((PP1*20)+20);      //取从商丘整秒发出的时间+20s，路径300km，耗时约1ms
							        //当Decode_count为608时，其实是P4信号结束前6.25ms，程序处理1ms
							        //后面会再延时604.75ms(其实在寻找帧头的时候又引入了最多100us)
							WWW=((Week[0]*4)+Week[1]);
							DDD=((Day[0]*16)+(Day[1]*4)+Day[2]);
							MON=((Month[0]*4)+Month[1]);
              if(Year[3]<2){Year[3]=0;}    //转换P4
							else{Year[3]=1;}
							YYY=((Year[0]*16)+(Year[1]*4)+Year[2]+Year[3]*64)+2000;
              if(PP3==1){PP3=0;}           //PP3==0表示午前，PP3!=0表示午后
							if(PP3!=0){HHH+=12;} //商丘授时台将中午12点定为午后0点，需要转换
						  if(SSS==60){
							  SSS=0;
							  MMM++;
				        if(MMM==60){
								  MMM=0;
									HHH++;
									if(HHH==24){HHH=0;}
									if((HHH >= 0) && (HHH < 12)){    //编译器指出：无符号整型一定是>=0,(HHH >= 0)毫无意义
									PP3=0;
									}
								}
							}
							Decode_start=0;
							Decode_count=0;
							OE_CheckT=0;
							Decode_OK=1;
							if(AUTO_BPC==0){AUTO_BPC=1;}            //午前两点校时成功，标记
							TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
					    TIM_Cmd(TIM3,DISABLE); //禁能定时器3
					    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //禁止定时器3更新中断
			        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,DISABLE);  ///禁能TIM3时钟
						}
          else{                                  //校验失败 						
            BPC_Decode_RE();
 					}					
				}							 
   }
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);       //更新中断标志位清零	 
 }
}

