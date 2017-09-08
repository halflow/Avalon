#ifndef __BPC_Decode_H__
#define __BPC_Decode_H__
#include "sys.h"

#define LED PAout(15)

#define BPC_EN  PAout(1)         //BPC模块供电使能
#define BPC_IN 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) //PA2


extern unsigned char Decode_OK;                            //0表示同步时间失败；1表示同步成功
extern unsigned char AUTO_BPC;                     //午夜自动校时成功标志	

extern u16 YYY;
extern unsigned char MON;
extern unsigned char DDD;
extern unsigned char HHH;                        //计算得到的小时,分钟,秒
extern unsigned char MMM;
extern unsigned char WWW;
extern unsigned char SSS;                  
extern unsigned char PP3;

void BPC_Decode(void);
void BPC_Decode_RE(void);
void IO_Init(void);
#endif
