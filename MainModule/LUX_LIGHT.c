/*****************************************************************
                        LUX_LIGHT.C  file

主要功能：负责定义光照度传感控制函数以及相应变量

创建时间：2015.10.31
*****************************************************************/


#include "LUX_LIGHT.h"
#include "STC89C5xRC.h"
#include <intrins.h>

//#define  SlaveAddress  0x46                                      //ADD引脚接地时，从地址为0x46，如果接电源，则为0xB8

bit      Lux_Start_Flag;                                         //光照度传感器启动标志位
bit      Lux_Read_Flag;                                          //光照度传感器读取标志位
bit      Lux_Flag;                                               //转换时间计算标志位

bit      Lux_Check_Flag;										//监听标志位

bit		Lux_Low_Flag;											//低光照强度标志位
bit		Lux_High_Flag;											//低光照强度标志位

unsigned int Lux_Value;                                          //光照度值
unsigned int Lux_Counter;                                        //转换时间计算器


/*--------------------------------------------------------------*/
/*-----------------------IIC相关操作函数-----------------------*/
/*--------------------------------------------------------------*/

/*------------------IIC起始信号--------------------*/
void IIC_start()
{
	Lux_Data=1;
	Lux_Sck=1;
	_nop_();
	_nop_();
	_nop_();
	Lux_Data=0;
	_nop_();
	_nop_();
	_nop_();
	Lux_Sck=0;
	_nop_();
	_nop_();
	_nop_();
}
/*------------------IIC停止信号--------------------*/
void IIC_stop()
{
	Lux_Data=0;
	Lux_Sck=1;
	_nop_();
	_nop_();
	_nop_();
	Lux_Data=1;
	_nop_();
	_nop_();
	_nop_();
}
/*----------------IIC应答信号发送------------------*/
void IIC_sendack(bit ack)
{
	Lux_Data=ack;                                //0为应答，1为非应答
	Lux_Sck=1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	Lux_Sck=0;
	_nop_();
	_nop_();
	_nop_();
}
/*----------------IIC应答信号接收------------------*/
bit IIC_recvack()
{
	Lux_Sck=1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CY=Lux_Data;
	Lux_Sck=0;
	_nop_();
	_nop_();
	_nop_();
	
	return CY;
}
/*---------------向IIC发送一字节数据---------------*/
void IIC_sendbyte(unsigned char dat)
{
	unsigned char i;
	
	for(i=0;i<8;i++)
	{
		dat<<=1;
		Lux_Data=CY;
		Lux_Sck=1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		Lux_Sck=0;
		_nop_();
		_nop_();
		_nop_();
	}
	IIC_recvack();
}
/*---------------从IIC接收一字节数据---------------*/
unsigned char IIC_recvbyte()
{
	unsigned char i;
	unsigned char dat=0;
	
	Lux_Data=1;
	for(i=0;i<8;i++)
	{
		dat<<=1;
		Lux_Sck=1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		dat|=Lux_Data;
		Lux_Sck=0;
		_nop_();
		_nop_();
		_nop_();
	}
	return dat;
}


/*--------------------------------------------------------------*/
/*---------------------光照度传感器初始化----------------------*/
/*-------------------------------------------------------------*/
void init_lux()
{
	Lux_Start_Flag=0;
	Lux_Read_Flag=0;
	Lux_Flag=0;
	Lux_Counter=0;
	Lux_Check_Flag=1;
	Lux_Low_Flag=0;
	Lux_High_Flag=0;
}

/*--------------------------------------------------------------*/
/*----------------------启动光照度传感器-----------------------*/
/*--------------------------------------------------------------*/
void start_lux()
{
	Lux_Start_Flag=0;                                       //清零光照度传感器启动标志位
	
	IIC_start();
	IIC_sendbyte(0x46);                      //Addr引脚接地      //发送设备地址（0110 011）+写信号（0）
	IIC_sendbyte(0x01);                                     //传感器通电命令
	IIC_stop();
	
	IIC_start();
	IIC_sendbyte(0x46);
	IIC_sendbyte(0x20);                                     //进行一次高分辨率测量命令
	IIC_stop();
	
	Lux_Flag=1;                         //计时标志位       //光照度传感器测量时间计数标志位置位
}

/*--------------------------------------------------------------*/
/*----------------------读取光照度传感器-----------------------*/
/*--------------------------------------------------------------*/
void read_lux()
{
	unsigned char j;
	unsigned int k=0;
	
	Lux_Read_Flag=0;                                         //清零光照度传感器读取标志位
	
	IIC_start();
	IIC_sendbyte(0x47);                                      //发送设备地址（0100 011）+读信号（1）
	
	j=IIC_recvbyte();                                        //读取高八位数据
	IIC_sendack(0);                                          //发送0应答，继续读取数据
	k=j;
	
	j=IIC_recvbyte();                                        //读取低八位数据
	IIC_sendack(1);                                          //发送1应答，结束数据读取
	Lux_Value=(k<<8)+j;
	
	IIC_stop();                                              //IIC停止
	
	Lux_Value=(float)Lux_Value/1.2;                          //准确的光照度值
	
	//测试用
	//Lux_Pack_Flag=1;
	//测试用
//	lux_judge();
}
