/*****************************************************************
                        TELE_PACK.c  file

主要功能：根据按键操作或感应器输入，打包报文

创建时间：2015.10.31
*****************************************************************/
/*--------------------------------------------------------------*/
/*----------------------------头文件------------------ ---------*/
/*--------------------------------------------------------------*/
#include "TELE_PACK.h"
#include "TELE_MANAGE.h"
#include "CONFIG.h"
#include "CONTROL.h"
#include "RELAY.h"
#include "SCR.h"
#include "SENSOR_T_H.h"
#include "LUX_LIGHT.h"
#include "TELE_T_R.h"
#include "TELE_PARSE.h"
/*-------------------------------------------------------------*/
/*-------------------------全局变量声明------------------------*/
/*-------------------------------------------------------------*/


/*-------------------------------------------------------------*/
/*-----------------指令报文：按键动作报文组装程序--------------*/
/*--------------------Key_Flag=1时由主函数调用-----------------*/
/*-------------------------------------------------------------*/
void tele_pack_key()                                               
{	
	unsigned char Which_Key;
	unsigned int addr;
	Key_Flag=0;                                  //按键动作标志位置零，防止主函数循环调用此函数
	Which_Key=Key_Value&0x07;                    //获取按键编码值--对应的74148的端口数值
	addr=0x8200+(Which_Key<<9);

	if(Key_Value&0x80)							//按键短按			
	{
		Key_Value=0;
		order(addr);							//短指令行
	}	
	else if(Key_Value&0x40)						//按键长按
	{
		Key_Value=Key_Value&0x0F;				//检测到长按后把标志位清零，保留键值，以备长按松开作用
		order(addr+64);							//长指令行
	}
	else 										//按键长按停止
	{
		Key_Value=0;
		order(addr+128);						//停指令行
	}		
}
/*--------------------------------------------------------------*/
/*--------------指令报文：声音传感器动作报文--------------------*/
/*--------------Switch_Sound_Flag1=1时由主函数调用--------------*/
/*--------------------------------------------------------------*/
void tele_pack_sound()                 								//暂定一路声控                             
{	                                           	
//	unsigned char Which_Digital=0;
	unsigned int addr=0x9200;						//测试声控
//	addr=0x9200+(Which_Digital<<9);

	Sound_Delay[0]=rom_r(addr+9);					//四个目标执行相同的延时时间
//	Sound_Delay[1]=rom_r(addr+25);					//占时同一延时时间（测试用）
//	Sound_Delay[2]=rom_r(addr+41);
//	Sound_Delay[3]=rom_r(addr+57);
	if(!Sound_Pack2_Flag)							//不延时之前的报文
	{					
		order(addr);
		Sound_Delay1_Flag=1;						//置延时标志位，开始计时延时时间
	}
	if(Sound_Pack2_Flag)							//触发消失，停指令行
	{
		order(addr+128);
		Sound_Pack2_Flag=0;		
	}
}
/*--------------------------------------------------------------*/
/*-------------指令报文：光照传感器阈值动作报文-----------------*/
/*--------------Lux_Low_Pack_Flag=1时由主函数调用---------------*/
/*--------------------------------------------------------------*/
void tele_pack_lux()                         	//                    
{	
	unsigned int addr;    
	addr=0xA000;                                           	

	if(Lux_High_Flag)
	{
		Lux_High_Flag=0;
		order(addr);							//触发出现,短指令

	}
	if(Lux_Low_Flag)							//触发消失，停指令行
	{
		Lux_Low_Flag=0;
		order(addr+128);
	}	
}
/*--------------------------------------------------------------*/
/*--------------指令报文：继电器输出状态动作报文----------------*/
/*--------------Switch_Sound_Flag1=1时由主函数调用--------------*/
/*--------------------------------------------------------------*/
void tele_pack_relay()                          	//
{		                                          	
//	unsigned char Which_Relay=0;
	unsigned int addr=0;
	addr=0xA800+(Which_Relay<<9);
	
	if(((Relay_Value>>Which_Relay)&0x01)==0x01)
		Relay_On_Flag=1;
	if(((Relay_Value>>Which_Relay)&0x01)==0x00)
		Relay_Off_Flag=1;
	Which_Relay=0;									//清除某一路继电器标识
	if(Relay_On_Flag)								//触发出现,短指令
	{												
		Relay_On_Flag=0;							//关闭标志位防止再次组装
		order(addr);								//短指令行
		Relay_Check_Flag=1;							//开启状态检测标志位
	}
	if(Relay_Off_Flag)								//触发消失，停指令行
	{
		Relay_Off_Flag=0;							//关闭标志位防止再次组装
		order(addr+128);							//停指令行
		Relay_Check_Flag=1;							//开启状态检测标志位
	}	
}
/*--------------------------------------------------------------*/
/*--------------指令报文：可控硅1输出状态动作报文----------------*/
/*--------------Switch_Sound_Flag1=1时由主函数调用--------------*/
/*--------------------------------------------------------------*/
void tele_pack_scr1()                          //             还没设定触发       
{		                                          	
	unsigned int addr;
	addr=0xB800;

	if(SCR1_value==0xF0)	
		Scr_100_Flag=1;	
	if(SCR1_value==0x01)
		Scr_Off_Flag=1;		
	if(Scr_100_Flag)							//触发出现,短指令
	{												
		Scr_100_Flag=0;							//关闭标志位防止再次组装
		order(addr+128);						//停指令行
		Scr_Check_Flag=1;						//开启状态检测标志位
	}
	if(Scr_Off_Flag)							//触发消失，停指令行
	{
		Scr_Off_Flag=0;							//关闭标志位防止再次组装
		order(addr);							//短指令行	
		Scr_Check_Flag=1;						//开启状态检测标志位
	}	
}
/*--------------------------------------------------------------*/
/*--------------指令报文：可控硅2输出状态动作报文----------------*/
/*--------------Switch_Sound_Flag1=1时由主函数调用--------------*/
/*--------------------------------------------------------------*/
void tele_pack_scr2()                          //             还没设定触发       
{		                                          	
	unsigned int addr;
	addr=0xBA00;

	if(SCR2_value==0xF0)	
		Scr_100_Flag=1;	
	if(SCR2_value==0x01)
		Scr_Off_Flag=1;		
	if(Scr_100_Flag)							//触发出现,短指令
	{												
		Scr_100_Flag=0;							//关闭标志位防止再次组装
		order(addr+128);						//停指令行
		Scr_Check_Flag=1;						//开启状态检测标志位
	}
	if(Scr_Off_Flag)							//触发消失，停指令行
	{
		Scr_Off_Flag=0;							//关闭标志位防止再次组装
		order(addr);							//短指令行	
		Scr_Check_Flag=1;						//开启状态检测标志位
	}	
}
/*-------------------------------------------------------------*/
/*-------------------反馈报文：继电器状态信息------------------*/
/*-----------------Relay_Flag=1时由主函数调用------------------*/
/*-------------------------------------------------------------*/
void tele_pack_relayval()
{
	unsigned char Relay_Date[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	Relay_Flag=0;                                                //清空继电器动作标志为
	Relay_Date[0]=0x84;						//反馈报文属性帧【反馈报文+继电器状态反馈+报文数据帧4帧】
	Relay_Date[1]=0x00;						//群ID
	Relay_Date[2]=0x04;						//接收模块ID为网关04
	
	Relay_Date[3]=0x70;						//反馈种类帧1
	Relay_Date[4]=0x00;						//反馈种类帧2
	Relay_Date[5]=Relay_Value;				//状态信息帧1-----历史状态----Relay_Value1
	Relay_Date[6]=Relay_Value;				//状态信息帧2-----当前状态
	write_tele_t(Relay_Date,7);
	Relay_Value1=Relay_Value;                                            //更新继电器控制值
}
/*-------------------------------------------------------------*/
/*-----------------反馈报文：可控硅亮度值----------------------*/
/*----------SCR1_Flag=1或者SCR2_Flag=1时由主函数调用-----------*/
/*-------------------------------------------------------------*/
void tele_pack_scrval()
{
	unsigned char SCR_Date[6]={0x00,0x00,0x00,0x00,0x00,0x00};
	if(SCR1_Flag)
	{
		SCR1_Flag=0;
		SCR_Date[0]=0x83;   					//反馈报文属性帧【反馈报文+硅1状态+报文数据帧3帧】
		SCR_Date[1]=0x00;						//接收群ID
		SCR_Date[2]=0x04;						//接收模块网关04
		
		SCR_Date[3]=0x80;						//硅控口1反馈
		SCR_Date[4]=0x00;										
		SCR_Date[5]=SCR1_value;
		write_tele_t(SCR_Date,6);
	}
	if(SCR2_Flag)
	{
		SCR2_Flag=0;
		SCR_Date[0]=0x83;                                 //报文种类域置为10，反馈报文
		SCR_Date[1]=0x00;						//接收群ID
		SCR_Date[2]=0x04;						//接收模块网关04
		SCR_Date[3]=0x81;						//硅控口2反馈
		SCR_Date[4]=0x00;
															
		SCR_Date[5]=SCR2_value;
		write_tele_t(SCR_Date,6);
	}
}
/*--------------------------------------------------------------*/
/*------------------反馈报文：ID信息----------------------------*/
/*-----------------ID_Flag=1时由主函数调用----------------------*/
/*--------------------------------------------------------------*/
void tele_pack_id()
{
	unsigned char ID_Date[6]={0x00,0x00,0x00,0x00,0x00,0x00};                                                          
	ID_Date[0]=0x83;						//反馈报文属性帧【反馈+id地址+报文数据帧3帧】
	ID_Date[1]=0x00;						//接收群ID
	ID_Date[2]=0x04;						//接收模块网关04
	ID_Date[3]=0x00;						//反馈种类帧1
	ID_Date[4]=0x00;						//反馈种类帧2														
	ID_Date[5]=ID;
	write_tele_t(ID_Date,6);
}
/*-----------------------------------------------------------------*/
/*-------------------反馈报文：模块所属组报告信息------------------*/
/*-----------------------------------------------------------------*/
void tele_pack_Module_Contain_Group()
{
	unsigned char Module_Contain_Group[6]={0x00,0x00,0x00,0x00,0x00,0x00};
                                                          //ID信息发送标志位清零
	Module_Contain_Group[0]=0x83;						//反馈报文属性帧【反馈+id地址+报文数据帧3帧】												
	Module_Contain_Group[1]=0x00;						//接收群ID
	Module_Contain_Group[2]=0x04;						//接收模块网关04
	
	Module_Contain_Group[3]=0x01;						//反馈种类帧1
	Module_Contain_Group[4]=0x00;						//反馈种类帧2														
	Module_Contain_Group[5]=ID_GP_Q[2];
	write_tele_t(Module_Contain_Group,6);
}
/*-------------------------------------------------------------*/
/*-----------------反馈报文：组信息报告报文--------------------*/
/*-----------------本模块属于哪些组内--------------------------*/
/*-------------------------------------------------------------*/
void tele_pack_group()
{
	unsigned char Group[6]={0X00,0X00,0X00,0X00,0X00,0x00};
	Group[0]=0x93;
	Group[1]=0x00;
	Group[2]=0x04;
	
	Group[3]=0x01;
	Group[4]=0x00;
	Group[5]=Group_a;											//上发的这一个组反馈
	write_tele_t(Group,6);	
}
/*-------------------------------------------------------------*/
/*----------------反馈报文：光照度报文组装函数-----------------*/
/*-----------------由查询组装报文函数调用----------------------*/
/*-------------------------------------------------------------*/
void tele_pack_luxvalue()
{
	unsigned char Lux_Data[6]={0xFF,0x00,0x00,0x00,0x00,0x00};
	Lux_Data[0]=Lux_Data[0]&0xBF;                                       //报文种类域置为10，反馈报文
	Lux_Data[0]=Lux_Data[0]&0xF3;                                    //反馈数据域帧数为3	
	Lux_Data[1]=0x00; 									  //接收群ID
	Lux_Data[2]=0x04;									  //接收模块ID为网关04
	
	Lux_Data[3]=0x60;   
	Lux_Data[4]=0x00;                                                   //0为光照度反馈
	Lux_Data[5]=Lux_Value/10;       //接收的数值范围为0-2550，先用一个字节反馈模拟量的值                                                             
	write_tele_t(Lux_Data,6);
}
/*--------------------------------------------------------------*/
/*---------------反馈报文：温度报文组装函数--------------------*/
/*--------------------由查询报文函数调用-----------------------*/
/*-------------------------------------------------------------*/
void tele_pack_temperature()
{
	unsigned char Temperature_Date[6]={0xFF,0x00,0x00,0x00,0x00,0x00};
	float t;
	Temperature_Date[0]=Temperature_Date[0]&0x8F;                     //报文种类域置为10，反馈报文
	Temperature_Date[0]=Temperature_Date[0]&0xF3;                     //反馈数据域帧数为3

	Temperature_Date[1]=0x00;
	Temperature_Date[2]=0x04;
	
	Temperature_Date[3]=0x60;							//反馈种类帧1
	Temperature_Date[4]=0x01;                                               //1为温度值反馈
	t=Temperature/1;
	if(t<0)
	{
		t=0-t;
		Temperature_Date[5]=(unsigned char)t;
		Temperature_Date[5]=Temperature_Date[3]|0x80;                      //最高位置零代表是负温度
	}
	else
		Temperature_Date[5]=(unsigned char)t;
	write_tele_t(Temperature_Date,6);
}
/*--------------------------------------------------------------*/
/*------------------反馈报文：湿度报文组装函数------------------*/
/*----------------Humidity_Flag=1时由主函数调用-----------------*/
/*--------------------------------------------------------------*/
void tele_pack_humidity()
{
	unsigned char Humidity_Date[6]={0xFF,0x00,0x00,0x00,0x00,0x00};
	float t;                                             
	Humidity_Date[0]=Humidity_Date[0]&0x8F;                              //报文种类域置为10，反馈报文
	Humidity_Date[0]=Humidity_Date[0]&0xF3;                              //数据域为3帧

	Humidity_Date[1]=0x00;
	Humidity_Date[2]=0x04;
	
	Humidity_Date[3]=0x60;
	Humidity_Date[4]=0x02;                                                  //2为湿度值反馈
	t=Humidity/1;
	if(t<0)                                                              //湿度值肯定不会是负的
		Humidity_Date[5]=0;
	else
		Humidity_Date[5]=(unsigned char)t;
	write_tele_t(Humidity_Date,6);
}
/*--------------------------------------------------------------*/
/*----------------反馈报文：指令配置信息反馈 -------------------*/
/*-----------------------查询报文函数调用-----------------------*/
/*--------------------------------------------------------------*/
void tele_pack_shortconfig(unsigned int address)				//所有短指令行反馈
{
	unsigned char i,n;
	unsigned char Config_Date[12]={0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00};
	Config_Date[0]=Config_Date[0]&0x8F;                  //报文属性，反馈报文
//	Config_Date[0]=Config_Date[0]&0xF7;                  //报文属性报文数据域长度为7
	
	Config_Date[1]=0x00;					//接收群ID
	Config_Date[2]=0x04;					//接收模块04

	if(Key_Num1)		
		Config_Date[3]=Key_Num1;		//按键反馈种类帧1
	else if(Digital_Num1)
		Config_Date[3]=Digital_Num1;
	else if(Analog_Num1)
		Config_Date[3]=Analog_Num1;
	else if(Relay_Num1)
		Config_Date[3]=Relay_Num1;
	else if(Scr_Num1)
		Config_Date[3]=Scr_Num1;

	if(rom_r(address+1)!=0xFF)							//目标1反馈
	{
		n=(rom_r(address+1)&0x0F)+0x03;			//报文内容[22 00 08 00 12]   5帧 
		Config_Date[0]=Config_Date[0]&(0xF0|n+2);                  //反馈报文属性报文数据域长度7
		Config_Date[4]=0x00|style;					//反馈种类帧2
		for(i=0;i<n;i++)
		{
			Config_Date[5+i]=rom_r(address+1+i);
		}
		write_tele_t(Config_Date,n+5);						
	}
	if(rom_r(address+17)!=0xff)						//目标2的反馈
	{
		n=(rom_r(address+17)&0x0F)+0x03;
		Config_Date[0]=Config_Date[0]&(0xF0|n+2);                  //反馈报文属性报文数据域长度7
		Config_Date[4]=0x02|style;					//反馈种类帧2	
		for(i=0;i<n;i++)
		{
			Config_Date[5+i]=rom_r(address+17+i);
		}
		write_tele_t(Config_Date,n+5);
	}
}
/*-----------------------------------------------------------------*/
/*-------------------反馈报文：配置信息配置成功--------------------*/
/*-----------------------------------------------------------------*/
void config_success()
{
	unsigned char ID_Date[6]={0x00,0x00,0x00,0x00,0x00,0x00};                                                          
	ID_Date[0]=0x83;						//反馈报文属性帧【反馈+id地址+报文数据帧3帧】												
	ID_Date[1]=0x00;						//接收群ID
	ID_Date[2]=0x04;						//接收模块网关04
	ID_Date[3]=0xF0;						//反馈种类帧1
	ID_Date[4]=0x00;						//反馈种类帧2
	ID_Date[5]=0xbb;																				
	write_tele_t(ID_Date,6);	
}
/*--------------------------------------------------------------*/
/*-----------------函数调用：指令调取-------------------------*/
/*--------------------------------------------------------------*/
void order(unsigned int address)
{
	unsigned char n,i;
	unsigned char Cache[8];
	if(rom_r(address+1)!=0xFF)							//目标1指令
	{
		for(i=0;i<8;i++)
			Cache[i]=rom_r(address+1+i);
		n=(Cache[0]&0x0F)+3;
		write_tele_t(Cache,n);
	}
	if(rom_r(address+17)!=0xff)							//目标2 指令
	{
		for(i=0;i<8;i++)
			Cache[i]=rom_r(address+17+i);
		n=(Cache[0]&0x0F)+3;
		write_tele_t(Cache,n);
	}
	if(rom_r(address+33)!=0xff)							//目标3指令
	{			
		for(i=0;i<8;i++)
			Cache[i]=rom_r(address+33+i);
		n=(Cache[0]&0x0F)+3;
		write_tele_t(Cache,n);
	}
	if(rom_r(address+49)!=0xff)							//目标4指令
	{
		for(i=0;i<8;i++)
			Cache[i]=rom_r(address+49+i);
		n=(Cache[0]&0x0F)+3;
		write_tele_t(Cache,n);
	}	
}