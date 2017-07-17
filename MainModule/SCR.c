/*****************************************************************
                             SCR.C  file

主要功能：负责可控硅模块的控制及初始化

创建时间：2015.10.31
*****************************************************************/
/*--------------------------------------------------------------*/
/*---------------------------头文件-----------------------------*/
/*--------------------------------------------------------------*/
#include <STC89C5xRC.h>
#include "SCR.h"
/*--------------------------------------------------------------*/



/*--------------------------------------------------------------*/
/*-------------------------全局变量定义-------------------------*/
/*--------------------------------------------------------------*/
unsigned char SCR_counter;                           //可控硅延时计数器
unsigned char SCR1_value;                            //可控硅1延时导通值,不是实际的亮度值，亮度值为100-value
unsigned char SCR2_value;                            //可控硅2延迟导通值
unsigned char SCR1_timer;                            //可控硅1导通高脉冲时间计数器
unsigned char SCR2_timer;                            //可控硅2导通高脉冲时间计数器

bit  SCR1_Flag;                                      //可控硅口1状态改变标志位
bit  SCR2_Flag;                                      //可控硅口2状态改变标志位
bit  SCR1_status;                                    //可控硅口1状态标志位，标志调光的方向
bit  SCR2_status;                                    //可控硅口2状态标志位，标志调光的方向
bit  SCR1_control_flag;                              //可控硅口1调光标志位
bit  SCR2_control_flag;                              //可控硅口2调光标志位
unsigned int SCR_control_counter;                    //可控硅调光计数器

bit Scr_100_Flag;									//可控硅100%亮度状态触发标志位
bit Scr_Off_Flag;									//可控硅0%亮度状态触发标志位
bit Scr_Check_Flag;									//可控硅状态检测标志位
bit Scr1_Act_Flag;									//可控硅输出动作标志位
bit Scr2_Act_Flag;
/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/
/*-------------------------可控硅初始化------------------------*/
/*--------------------------------------------------------------*/

void init_scr()
{
	SCR_counter=0;
	SCR1_value=0xF0;
	SCR2_value=0xF0;
	SCR1_timer=0;
	SCR2_timer=0;
	
	SCR1_Flag=0;
	SCR2_Flag=0;
	SCR1_status=0;
	SCR2_status=0;
	SCR1_control_flag=0;
	SCR2_control_flag=0;
	SCR_control_counter=0;
	
	Scr_Check_Flag=1;
	
	IE1=0;                                             //开中断之前先清空一下中断标志位
	EX1=1;
	IT1=1;                                             //外部中断设置为下跳沿触发模式
}
/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/
/*-------------------------外部中断1程序- ----------------------*/
/*             功能：过零检测电路触发中断                       */
/*--------------------------------------------------------------*/
void interrupt_INT1() interrupt 2                    //外部中断1的中断号为2
{
	TH2=0xFF;TL2=0x0A;                                 //重新校准计数器2,防止灯光抖动
	SCR_counter=0;                                     //清零SCR计数器
	TF2=0;                                             //清空计数器2的中断标志位
}
/*--------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/*------------------------可控硅处理程序------------------------*/
/*--------------------------------------------------------------*/
//void scr_handle()
//{
	
//}