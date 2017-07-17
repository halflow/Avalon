/*****************************************************************
                          CONFIG.h  file

主要功能：整个系统关键参数的宏定义以功能可重配置相关参数定义

创建时间：2015.10.31
*****************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__ 
/*--------------------------------------------------------------*/
/*-----------------可控硅模块相关参数的宏定义-------------------*/
/*--------------------------------------------------------------*/
#define SCR_TOP_THRESHOLD  80                   //可控硅调光阈值下线，100为全关
#define SCR_BOTTOM_THRESHOLD  05                //可控硅调光阈值上线，0为全开
#define SCR_CHANGE_RATE  1000                   //1000*100us=0.1s，每0.1s可控硅值增加1或者减少1
#define SCR_TIGGER_TIME  10                     //可控硅触发维持时间，10*100us=1ms

/*--------------------------------------------------------------*/
/*------------------开关模块相关参数的宏定义--------------------*/
/*--------------------------------------------------------------*/
#define KEY_SHAKE_TIME  500                     //开关消抖时间，500*100us=50ms
#define KEY_BOUNDARY_TIME  5000                 //开关长短按界限时间，5000*100us=0.5s

/*--------------------------------------------------------------*/
/*-----------------继电器模块相关参数的宏定义-------------------*/
/*--------------------------------------------------------------*/
#define REWRITE_TIME  10                        //继电器重写次数

/*--------------------------------------------------------------*/
/*--------------报文发送和接收模块相关参数的宏定义--------------*/
/*--------------------------------------------------------------*/
#define IDLE_TIME  64                           //认定总线空闲的时间，64*52us（16位数据长度，波特率9600）
#define OVER_TIME  32                           //报文接收超时时间，32*52us，即8位数据长度
#define ACK_TIME   40                           //等待确认报文的时间，40*52us，即15位数据长度

/*--------------------------------------------------------------*/
/*-----------------报文管理模块相关参数的宏定义-----------------*/
/*--------------------------------------------------------------*/
#define QUEUE_LENGTH  60                        //报文发送缓冲区和接受缓冲区的队列长度，40Byte

/*--------------------------------------------------------------*/
/*--------------------------系统变量声明-----------------------*/
/*--------------------------------------------------------------*/
//#define ID  05                                   //本模块ID地址
#define ID rom_r(0x8002)

extern unsigned char ID1;                       //用于ID转换的中间变量1
extern unsigned char ID2;                       //用于ID转换的中间变量2
extern unsigned char bdata Temp1;               //用于位运算的中间变量1
extern unsigned char bdata Temp2;               //用于位运算的中间变量2

extern unsigned char Write_Cache[192];

extern unsigned char ID_GP_Q[3];
extern unsigned char Analog_Threshold[16];
extern unsigned char Sound_Delay[4];


extern  bit Temp1_0;                            //位变量定义
extern	bit Temp1_1;
extern	bit Temp1_2;
extern	bit Temp1_3;
extern	bit Temp1_4;
extern	bit Temp1_5;
extern	bit Temp1_6;
extern	bit Temp1_7;
extern	bit Temp2_0;
extern	bit Temp2_1;
extern	bit Temp2_2;
extern	bit Temp2_3;
extern	bit Temp2_4;
extern	bit Temp2_5;
extern	bit Temp2_6;
extern	bit Temp2_7;

/*--------------------------------------------------------------*/
extern bit  Sound_Check_Flag;
extern bit  Sound_Pack_Flag;
extern bit Sound_Delay1_Flag;
extern bit  Sound_Pack2_Flag;
extern unsigned int Sound_Sensor_Counter;
extern unsigned char Sound_Sensor_Counter1;

extern bit Relay1_Alconfig_Flag,Relay2_Alconfig_Flag,Relay3_Alconfig_Flag,Relay4_Alconfig_Flag;
extern bit Scr1_Alconfig_Flag;
extern bit Scr2_Alconfig_Flag;
/*-------------------------------------------------------------*/
/*---------------------------函数声明--------------------------*/
/*-------------------------------------------------------------*/
void init_config();
void id_chang();
void rom_c(unsigned int addr);
void rom_w(unsigned int addr,unsigned char dat);
unsigned char rom_r(unsigned int addr);
void config_save();
void id_Gp_Q_save();
void delete_all_e2prom();
#endif