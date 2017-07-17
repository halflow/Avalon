#include <STC89C5xRC.h>
#include "CONTROL.h"
#include "TELE_MANAGE.h"
#include "TELE_T_R.h"
#include "TELE_PARSE.h"
#include "TELE_PACK.h"
#include "RELAY.h"
#include "SCR.h"
#include "Sensor_T_H.h"
#include "LUX_LIGHT.h"
#include <intrins.h>

sbit sound=P1^5;
void main()
{
	unsigned char i;
	
	init_tele_t_r();				//报文发送与接收模块初始化
	init_config();					//初始化程序
	init_tele_manage();				//报文管理初始化
	init_scr();						//可控硅初始化
	init_sensor_t_h();				//温湿度传感器初始化
	init_lux();						//光照度传感器初始化
	init_key();						//开关初始化
	init_relay();					//继电器初始化
	SM2=1;			         //所有节点处于监听状态		
	
	sound=1;				//读端口前先置1

	for(i=0;i<255;i++)				//延时大约6us
		_nop_();
	for(i=0;i<255;i++)				//延时大约6us
		_nop_();
	for(i=0;i<255;i++)				//延时大约6us
		_nop_();
	for(i=0;i<255;i++)				//延时大约6us
		_nop_();
	RI=0;									//串口接收标志位清零
	TI=0;									//串口发送标志位清零
	EA=1;                                             //开总中断
	ES=1;                                             //开串口中断
	REN=1;                                            //允许接收数据
	WDT_CONTR=0x3F;									//看门狗寄存器设置
	
	while(1)
	{
		if(Tele_Cheak_T)	//发送缓冲队列检查标志位，检查队列中是否有报文，初试值为1，一直在检测
			tele_cheakt();					//报文缓冲队列检查函数
		if(Tele_Cheak_R)					//接收缓冲队列检查标志位，检查队列中是否有报文
			tele_cheakr();					//报文缓冲队列检查函数
			
		if(Tele_TF)							//报文发送标志位，在tele_cheakt()中置1
			telegram_t();					//报文发送程序
		if(Tele_Handle_R)					//接收缓冲区报文处理标志位,tele_cheakr()中置1
			tele_parse();					//报文解析函数
			
		if(Key_Flag)						//按键动作标志位
		{
			tele_pack_key();				//按键动作报文组装程序
		}
		if(Relay_Flag)						//继电器动作标志位
			tele_pack_relayval();			//继电器状态报文
		if(SCR1_Flag)						//可控硅口1状态改变标志位
		{
			tele_pack_scrval();				//可控硅状态反馈报文
			Scr1_Act_Flag=1;
		}	
		if(SCR2_Flag)						//可控硅口2状态改变标志位
		{
			tele_pack_scrval();				//可控硅状态反馈报文
			Scr2_Act_Flag=1;
		}
				
		if(Sound_Check_Flag)				//声音传感器标志位
		{
			if(sound==0)					//检测到有声音
			{
				Sound_Check_Flag=0;
				Sound_Pack_Flag=1;			//声音报文组装标志位
			}
		}	
		if(Sound_Pack_Flag)
			tele_pack_sound();				//声音传感器动作报文 
		
		if(Sensor_T_H_Flag)					//定时器2中每隔2s
			star_sensor_t_h();				//启动温湿度传感器
		if(Sensor_T_H_Flag1&&T_H_Data==0)	//温湿度传感器IIC通信数据信号
			read_sensor_t_h();				//读取温湿度传感器

		if(Lux_Start_Flag)				
			start_lux();					//启动光照度传感器
		if(Lux_Read_Flag)					//光照读取标志位，计时器1每200ms读取一次
			read_lux();						//读取光照度传感器					
			
		if(Lux_Check_Flag)														//光照强度检测标志位
		{
			if(Lux_Value>=Analog_Threshold[0])
			{
				Lux_Check_Flag=0;
				Lux_High_Flag=1;
				tele_pack_lux();
			}
			else if(Lux_Value<=Analog_Threshold[3])
			{
				Lux_Check_Flag=0;
				Lux_Low_Flag=1;
				tele_pack_lux();
			}
		}
		if((Lux_Value<Analog_Threshold[0])&&(Lux_Value>Analog_Threshold[3]))	//直到光照值合适才恢复检测
		{
			Lux_Check_Flag=1;
		}	
		
		if(Relay_Check_Flag)													//继电器状态检测标志位
		{
			if(Relay1_Alconfig_Flag&&Relay1_Act_Flag)								//已经配置过并且继电器有动作
			{
				Relay1_Act_Flag=0;												//清空本次动作标志位
				Relay_Check_Flag=0;
				Which_Relay=0;
				tele_pack_relay();	
			}
			else if(Relay2_Alconfig_Flag&&Relay2_Act_Flag)								//已经配置过并且继电器有动作
			{
				Relay2_Act_Flag=0;												//清空本次动作标志位
				Relay_Check_Flag=0;
				Which_Relay=1;
				tele_pack_relay();	
			}
			else if(Relay3_Alconfig_Flag&&Relay3_Act_Flag)								//已经配置过并且继电器有动作
			{
				Relay3_Act_Flag=0;												//清空本次动作标志位
				Relay_Check_Flag=0;
				Which_Relay=2;
				tele_pack_relay();	
			}
			else if(Relay4_Alconfig_Flag&&Relay4_Act_Flag)								//已经配置过并且继电器有动作
			{
				Relay4_Act_Flag=0;												//清空本次动作标志位
				Relay_Check_Flag=0;
				Which_Relay=3;
				tele_pack_relay();	
			}
		}
		if(Scr_Check_Flag)
		{
			if((Scr1_Alconfig_Flag&&Scr1_Act_Flag)||(Scr2_Alconfig_Flag&&Scr2_Act_Flag))							//已经配置过并且继电器有动作
			{
				Scr_Check_Flag=0;											//停止检测
				if(Scr1_Act_Flag)
				{
					Scr1_Act_Flag=0;												//清空本次动作标志位				
					tele_pack_scr1();					
				}
				if(Scr2_Act_Flag)
				{
					Scr2_Act_Flag=0;												//清空本次动作标志位				
					tele_pack_scr2();						
				}

			}
		}
//延时跳转指令时间到		
		if(Time_Up1)
		{					
			Delay_Flag1=0;
			jump_order(0);
			jump_tele[0]=0;
			Time_Up1=0;
		}
		if(Time_Up2)
		{					
			Delay_Flag2=0;
			jump_order(7);
			jump_tele[7]=0;
			Time_Up2=0;
		}
		if(Time_Up3)
		{					
			Delay_Flag3=0;
		 	jump_order(14);
			jump_tele[14]=0;
			Time_Up3=0;
		}	
		if(Time_Up4)
		{					
			Delay_Flag4=0;
			jump_order(21);
			jump_tele[21]=0;
			Time_Up4=0;
		}	
		WDT_CONTR=0x3F;
	}
}