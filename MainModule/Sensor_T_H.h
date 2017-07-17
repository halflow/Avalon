/******************************************************************
                             SENSOR_T_H.H  file

主要功能：温湿度传感器模块中全局变量及函数申明

创建时间：2015.10.31
*******************************************************************/

#ifndef __SENSOR_T_H_H__
#define __SENSOR_T_H_H__

#include <STC89C5xRC.h>

/*-------------------------全局变量声明--------------------------*/

sbit T_H_Sck=P1^6;                                                      //温湿度传感器IIC通信时钟信号
sbit T_H_Data=P1^7;                                                  //温湿度传感器IIC通信数据信号

extern bit      Sensor_T_H_Flag;                                        //温湿度传感器启动标志位
extern bit      Sensor_T_H_Flag1;                                       //温湿度传感器读取标志位
extern bit      T_Or_H;                                                 //0为温度测量，1为湿度测量


extern float Temperature;                                               //温度测量值，-40--123.8(摄氏度)
extern float Humidity;                                                  //湿度测量值，0--100（%RH）
extern unsigned int Sensor_T_H_Counter;                                 //温湿度传感器计数器，每隔4s进行一次温度测量和一次湿度测量


/*--------------------------函数声明-----------------------------*/

void init_sensor_t_h();
void star_sensor_t_h();
void read_sensor_t_h();

#endif