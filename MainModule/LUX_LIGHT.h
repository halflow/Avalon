/******************************************************************
                         LUX_LIGHT.H  file

主要功能：光照度传感器模块中全局变量及函数申明

创建时间：2015.10.31
*******************************************************************/

#ifndef __LUX_LIGHT_H__
#define __LUX_LIGHT_H__

#include <STC89C5xRC.h>

/*-------------------------全局变量声明--------------------------*/
sbit Lux_Sck=P2^0;
sbit Lux_Data=P2^1;

extern bit      Lux_Start_Flag;                                         //光照度传感器启动标志位
extern bit      Lux_Read_Flag;                                          //光照度传感器读取标志位
extern bit      Lux_Flag;                                               //转换时间计算标志位


extern bit      Lux_Check_Flag;

extern bit		Lux_Low_Flag;									//光照强度检测标志位
extern bit		Lux_High_Flag;


extern unsigned int Lux_Value;                                          //光照度值
extern unsigned int Lux_Counter;                                        //转换时间计算器
/*--------------------------函数声明-----------------------------*/
void init_lux();
void start_lux();
void read_lux();
void lux_judge();

#endif