/*****************************************************************
                             KEY.C  file

主要功能：负责对开关模块开关动作的响应，区分短按、长按及停止动作操
          作，置位相应标志位

创建时间：2013.7.30
*****************************************************************/

/*--------------------------------------------------------------*/
/*---------------------------头文件-----------------------------*/
/*--------------------------------------------------------------*/
#include <STC89C5xRC.h>
#include "KEY.h"
#include "SCR.h"
#include "Sensor_T_H.h"
#include "LUX_LIGHT.h"
#include "CONFIG.h"
/*--------------------------------------------------------------*/



/*--------------------------------------------------------------*/
/*-------------------------全局变量定义-------------------------*/
/*--------------------------------------------------------------*/
bit  Key_Cheak;                                      //标志计数器2的用途，按键检测
unsigned int Key_Counter;                            //开关按下的时间计数器
bit  Long_Cheak;                                     //用于长按时检测合适停止控制
bit  Key_Shake;
unsigned char Key_Value;
bit  Key_Flag;


/*--------------------------------------------------------------*/
/*--------------------------开关初始化--------------------------*/
/*--------------------------------------------------------------*/
void init_key()
{
	A0=1;                                              //A0、A1、A2为74hc148输入端口，需要写入1
	A1=1;
	A2=1;
	Key_Cheak=0;                                       //按键检测初始化为零
	Key_Counter=0x0000;                                //计数器初始化成零
	Long_Cheak=0;
	Key_Shake=0;
	Key_Value=0;
	Key_Flag=0;

	
	T2CON=0x00;T2MOD=0x00;                             //设置定时器2为16位自动重装载模式
	RCAP2H=0xFF;RCAP2L=0x0A;                           //设置定时器的定时时间为100us
	TH2=0xFF;TL2=0x0A;
	TR2=1;                                             //启动定时器T2
	ET2=1;                                             //允许定时器T2中断
//外中断1初始化
	IE0=0;                                             //开中断之前先清空一下中断标志位
	EX0=1;                                             //允许外部中断0产生中断
	IT0=1;                                             //外部中断0为跳沿触发方式

}
/*--------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/*-------------------------外部中断0程序------------------------*/
/*--------------------------------------------------------------*/
void interrupt_INT0() interrupt 0                    //外部中断0的中断号为0
{
	Key_Cheak=1;                       //开启按键检测，具体检测和处理过程在定时器2中断程序中完成
	Key_Counter=0x0000;
	Key_Value=0;
	EX0=0;                                     //关闭外部中断0，防止按键抖动造成重复产生中断
	IE0=0;	                     //                //为保险起见，再次清空外部中断请求位
}
/*--------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/*------------------------定时器2中断程序-----------------------*/
/*--------------------------------------------------------------*/
void interrupt_timer2() interrupt 5                    //定时时间为100us
{
	TF2=0;                         //清空定时器2的中断请求标志位，T2的中断请求标志位需要软件清零

/*-----------------------可控硅触发控制输出程序-------------------------*/	
	SCR_counter++;
	if(SCR_counter==SCR1_value)				//可控硅的延时导通值
	{
		SCR1=0;								//触发SCR1控制灯泡亮
		SCR1_timer=SCR1_value;				//
	}
	if(SCR_counter==SCR2_value)				//可控硅的延时导通值
	{
		SCR2=0;								//触发SCR2控制灯泡亮
		SCR2_timer=SCR2_value;
	}
	if(SCR_counter==(SCR1_timer+SCR_TIGGER_TIME))          //使导通高脉冲维持1ms
		SCR1=1;							
	if(SCR_counter==(SCR2_timer+SCR_TIGGER_TIME))
		SCR2=1;
/*---------------------------------------------------------------*/
	
/*-----------------------可控硅调光控制输出程序-------------------------*/	
	if(SCR1_control_flag||SCR2_control_flag)               //按键长按时可控硅的控制
	{
		SCR_control_counter++;                             //可控硅控制计数器加1
		if(SCR_control_counter==SCR_CHANGE_RATE)//0.1s可控硅延时导通值增加或者减少1次，长按时从最亮到最暗需要7s钟
		{
			if(SCR1_control_flag)                            //硅控口1
			{
				if(SCR1_value>SCR_TOP_THRESHOLD)             //循环控制，亮度最暗时再逐渐增亮
				{
					SCR1_status=0;
					SCR1_value=SCR_TOP_THRESHOLD;
				}
				else if(SCR1_value<SCR_BOTTOM_THRESHOLD)       //循环控制，亮度最亮时再逐渐变暗
				{
					SCR1_status=1;
					SCR1_value=SCR_BOTTOM_THRESHOLD;
				}
				if(SCR1_status)
					SCR1_value++;
				else
					SCR1_value--;
				if(SCR1_value%10==0)                           //调光过程中实时反馈亮度值
					SCR1_Flag=1;
			}
			
			if(SCR2_control_flag)                            //硅控口2
			{
				if(SCR2_value>SCR_TOP_THRESHOLD)
				{
					SCR2_status=0;
					SCR2_value=SCR_TOP_THRESHOLD;
				}
				else if(SCR2_value<SCR_BOTTOM_THRESHOLD)       //循环控制，亮度最亮时再逐渐变暗
				{
					SCR2_status=1;
					SCR2_value=SCR_BOTTOM_THRESHOLD;
				}
				
				if(SCR2_status)
					SCR2_value++;
				else
					SCR2_value--;
				if(SCR2_value%10==0)
					SCR2_Flag=1;
			}
			
			SCR_control_counter=0;

		}
	}
/*-----------------------------------------------------------*/

/*--------按键的读取，短按、长按、停止控制的判断，消抖-------*/	
	if(Key_Cheak)				//开关使83编码器产生脉冲触发中断1--keycheak=1;
	{
		Key_Counter++;
//1		
		if(Key_Counter==KEY_SHAKE_TIME) 		  //开关消抖时间KEY_SHAKE_TIME =500,相当于50ms	
								//延迟50ms后读取按键的值，防止按键抖动造成取值错误，按键前消抖
		{	
			
			if(!INT0)		//若按键还在按下		//判断外部中断0的P32口按键是否还在按下
			{
				//3个if连续判断
				if(A0)			//sbit A0=P2^4;8-3编码器最低位
					Key_Value=Key_Value|0x01;			
				if(A1)			//sbit A1=P2^5; 8-3编码器第二位
					Key_Value=Key_Value|0x02;		
				if(A2)			//sbit A2=P2^6; 8-3编码器最高位
					Key_Value=Key_Value|0x04;	
					
				Key_Value=~Key_Value;          //74148的编码为反码
				Key_Value=Key_Value&0x07;	   //得到了按键按下74148的端口数
			}
			else		//若已松开，则认为误操作
			{
				Key_Cheak=0;
				Key_Counter=0;
				Key_Shake=1;
			}
		}
//2		
		if(Key_Counter>KEY_SHAKE_TIME)               //按键前消抖之后开始判断按键是长按还是短按
		{
			if(INT0)             //按键松开          //短按//
			{
				Key_Value=Key_Value|0x80;
				Key_Shake=1;
				Key_Cheak=0;
				Key_Counter=0;
				Key_Flag=1;
			}
			else if(Key_Counter==KEY_BOUNDARY_TIME) //按键未松开  //长按
			{										//KEY_BOUNDARY_TIME相当于0.5S长短按界限值
				Key_Value=Key_Value|0x40;
				Long_Cheak=1;
				Key_Cheak=0;
				Key_Counter=0;
				Key_Flag=1;
			}
		}
	}
	
	if(Long_Cheak)                                       //停止控制检测
	{
		if(INT0)								//按键松开
		{
			Key_Value=Key_Value|0x20;
			Key_Shake=1;
			Long_Cheak=0;
			Key_Flag=1;
		}
	}
	
	if(Key_Shake)                         //按键后消抖，防止松开按键的抖动造成错误的按键输入响应
	{
		Key_Counter++;
		if(Key_Counter==KEY_SHAKE_TIME)			//开关消抖时间，500*100us=50ms
		{
			Key_Shake=0;
			Key_Counter=0;
			IE0=0;            //外中断0的请求标志位          //开外部中断之前，先清空一下中断标志位
			EX0=1;                                           //开外部中断
		}
	}
/*-------------------------------------------------------------*/

/*--------------------开关类传感器延迟控制---------------------*/	
	if(Sound_Sensor_Flag2)
	{
		Sound_Sensor_Counter++;
		if(Sound_Sensor_Counter==10000)
		{
			Sound_Sensor_Counter=0;
			Sound_Sensor_Counter1++;
			if(Sound_Sensor_Counter1==Switch_Sensor_Config[2])
			{
				Sound_Sensor_Counter=0;
				Sound_Sensor_Counter1=0;
				Sound_Check_Flag=1;
				Sound_Pack_Flag=1;
				Sound_Sensor_Flag2=0;
			}
		}
	}

/*-------------------温湿度测试间隔时间控制--------------------*/
	Sensor_T_H_Counter++;
	if(Sensor_T_H_Counter==20000)                            //每隔2s测量一次温度和湿度
	{
		Sensor_T_H_Flag=1;
		Sensor_T_H_Counter=0;
		Lux_Start_Flag=1;                                      //每隔2s测量一次光照度值
	}
/*-------------------光照测试间隔时间控制--------------------*/		
	if(Lux_Flag)                                   //启动光照度传感器后，200ms后开始读取光照度值
	{
		Lux_Counter++;
		if(Lux_Counter==2000)
		{
			Lux_Read_Flag=1;
			Lux_Counter=0;
			Lux_Flag=0;
		}
	}
}