/*****************************************************************
                           CONFIG.C  file

主要功能：主模块可重配置功能实现

创建时间：2015.10.31
*****************************************************************/
/*--------------------------------------------------------------*/
/*-----------------------------头文件---------------------------*/
/*--------------------------------------------------------------*/
#include "CONFIG.h"
#include <intrins.h>
#include <STC89C5xRC.h>

unsigned char bdata Temp1;					//存放到位寻址区
unsigned char bdata Temp2;

sbit Temp1_0=Temp1^0;
sbit Temp1_1=Temp1^1;
sbit Temp1_2=Temp1^2;
sbit Temp1_3=Temp1^3;
sbit Temp1_4=Temp1^4;
sbit Temp1_5=Temp1^5;
sbit Temp1_6=Temp1^6;
sbit Temp1_7=Temp1^7;
sbit Temp2_0=Temp2^0;
sbit Temp2_1=Temp2^1;
sbit Temp2_2=Temp2^2;
sbit Temp2_3=Temp2^3;
sbit Temp2_4=Temp2^4;
sbit Temp2_5=Temp2^5;
sbit Temp2_6=Temp2^6;
sbit Temp2_7=Temp2^7;

unsigned char ID1,ID2;

unsigned char ID_GP_Q[3];					//群地址,ID地址，分组
unsigned char Analog_Threshold[16];
unsigned char Write_Cache[192];
unsigned char Sound_Delay[4];               //将定时时间另外存放起来，防止延时时间内缓存区占用     
bit Sound_Delay1_Flag;						//目标1的延时标志位
//	bit Sound_Delay2_Flag;
//	bit Sound_Target3_Flag;
//	bit Sound_Target4_Flag;
bit  Sound_Check_Flag=1;					//声音检测标志位，初始化为1，检测进行
bit  Sound_Pack_Flag=0;						//检测到声音后，报文组装标志位
bit  Sound_Pack2_Flag=0;					//延时后组装的报文标志位
unsigned int Sound_Sensor_Counter=0;
unsigned char Sound_Sensor_Counter1=0;


bit Relay1_Alconfig_Flag,Relay2_Alconfig_Flag,Relay3_Alconfig_Flag,Relay4_Alconfig_Flag;
bit Scr1_Alconfig_Flag;
bit Scr2_Alconfig_Flag;
/*--------------------------------------------------------------*/
/*--------------------------初始化程序--------------------------*/
/*--------------------------------------------------------------*/
void init_config()
{
	unsigned char i;
	
	i=rom_r(0x8000);	                                   
	if(i!=0xAA)          			        //判断EEPROM是否已经配置过，如果配置过，0x8000的值为0xAA
	{
		rom_c(0x8000);                                   //清空第一扇区，准备写入缺省配置   
		
		rom_w(0x8001,0);                                  //群ID    缺省为0
		rom_w(0x8002,5);                                 //模块ID  缺省为5
		rom_w(0x8003,0x00);                               //组ID低8位 	缺省为0	
		rom_w(0x8004,0x00);                               //组ID高8位 	缺省为0	
		for(i=0;i<3;i++)								  //3路模拟量上阈值  缺省0xFF
		{
			rom_w(0x8010+i,0xFF);
			i=i+1;
		}
		for(i=0;i<3;i++)								  //3路模拟量下阈值 缺省0x00
		{
			rom_w(0x8013+i,0x00);
			i=i+1;
		}		                       				
		rom_w(0x8000,0xAA);                                //将0x8000的值写为0xAA，标志第一扇区已经进行过配置
	}	
	for(i=0;i<3;i++)				/*将EEPROM之前配置读入到RAM中*/					
	{
		ID_GP_Q[i]=rom_r(0x8001+i);
	}
	for(i=0;i<6;i++)				/*将EEPROM之前配置读入到RAM中*/
	{
		Analog_Threshold[i]=rom_r(0x8010+i);
	}	
	id_chang();												//将本模块的ID拆分转换
}
/*--------------------------------------------------------------*/
/*--------------------------ID转换程序--------------------------*/
/*------程序功能：将一帧数据先翻转，然后在将一帧数据扩展成两成--*/
/*--------------------------------------------------------------*/
void id_chang()
{
	Temp1=ID;
	Temp2_0=Temp1_7;					//高低位转换
	Temp2_1=Temp1_6;
	Temp2_2=Temp1_5;
	Temp2_3=Temp1_4;
	Temp2_4=Temp1_3;
	Temp2_5=Temp1_2;
	Temp2_6=Temp1_1;
	Temp2_7=Temp1_0;
	Temp1=Temp2;
	Temp2=0xFF;
	Temp2_7=Temp1_7;					
	Temp2_5=Temp1_6;
	Temp2_3=Temp1_5;
	Temp2_1=Temp1_4;
	ID2=Temp2;							//ID2为取出的低4位
	Temp2=0xFF;
	Temp2_7=Temp1_3;
	Temp2_5=Temp1_2;
	Temp2_3=Temp1_1;
	Temp2_1=Temp1_0;
	ID1=Temp2;							//ID1为取出的高4位
}

/*--------------------------------------------------------------*/
/*--------------------------擦除扇区程序-----------------------*/
/*--------------------------------------------------------------*/
void rom_c(unsigned int addr)                         //EEPROM写入任何数据之前必须将整个扇区擦除，addr=某一扇区首地址
{	
	ISP_CONTR = 0x80;                                 //打开IAP功能，允许编程改变EEPROM，设置编程操作等待时间
	ISP_CMD   = 0x03;		                          //对EEPROM扇区进行擦除命令 
	ISP_ADDRL = addr;                                 //ISP/IAP操作地址低8位	
	ISP_ADDRH = addr>>8;                              //ISP/IAP操作地址高8位	   
	ISP_TRIG = 0x46;          
	ISP_TRIG = 0xB9;                                  //先写入0x46，再写入0xB9，触发ISP/IAP命令生效 
	_nop_();
	ISP_CONTR=0;                                      //关闭ISP/IAP
	ISP_CMD=0;                                        //待机模式，无ISP操作
	ISP_TRIG=0;	                                      //清空与ISP有关的特殊功能寄存器
}

/*--------------------------------------------------------------*/
/*--------------------向EEPROM中写入一个字节-------------------*/
/*--------------------------------------------------------------*/
void rom_w(unsigned int addr,unsigned char dat)    //addr=扇区单元地址，dat=待写入的数据
{
	ISP_CONTR = 0x80;		                           //打开IAP功能，允许编程改变EEPROM，设置编程操作等待时间
	ISP_CMD   = 0x02;	                               //对EEPROM进行字节编程   
	ISP_ADDRL = addr;        
	ISP_ADDRH = addr>>8;      
	ISP_DATA  = dat;          
	ISP_TRIG = 0x46;          
	ISP_TRIG = 0xB9;          
	_nop_();
	ISP_CONTR=0;                                      //关闭ISP/IAP
	ISP_CMD=0;                                        //待机模式，无ISP操作
	ISP_TRIG=0;	                                      //清空与ISP有关的特殊功能寄存器
}
/*--------------------------------------------------------------*/
/*--------------------从EEPROM中读出一个字节-------------------*/
/*--------------------------------------------------------------*/
unsigned char rom_r(unsigned int addr)              //addr=扇区单元地址
{   
	unsigned char dat;
	ISP_CONTR = 0x80;		  
	ISP_CMD   = 0x01;                               //对EEPROM进行字节读操作
	ISP_ADDRL = addr;         
	ISP_ADDRH = addr>>8;      
	ISP_TRIG = 0x46;          
	ISP_TRIG = 0xB9;          
	_nop_();
	dat = ISP_DATA;			  
	ISP_CONTR=0;                                      //关闭ISP/IAP
	ISP_CMD=0;                                        //待机模式，无ISP操作
	ISP_TRIG=0;	                                      //清空与ISP有关的特殊功能寄存器		       
	return dat;
}

/*--------------------------------------------------------------*/
/*-------------------将配置参数保存到EEPROM中-------------------*/
/*--------------------------------------------------------------*/
void id_Gp_Q_save() 									//群ID,模块ID及分组重新配置
{
	unsigned char i;
	rom_c(0x8000);
	for(i=0;i<3;i++)
	{
		rom_w(0x8001+i,ID_GP_Q[i]);
	}
	for(i=0;i<16;i++)
	{
		rom_w(0x8010+i,Analog_Threshold[i]);
	}
	rom_w(0x8000,0xAA);
	id_chang(); 										//重新配置后再次拆分转换，更新缓存在Ram的数据
}
void delete_all_e2prom()
{
	unsigned char i;
	for(i=0;i<30;i++)
	{
		rom_c(0x8200+(i<<9));
	}
}