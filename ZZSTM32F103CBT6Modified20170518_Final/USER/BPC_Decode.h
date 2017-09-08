#ifndef __BPC_Decode_H__
#define __BPC_Decode_H__
#include "sys.h"

#define LED PAout(15)

#define BPC_EN  PAout(1)         //BPCģ�鹩��ʹ��
#define BPC_IN 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) //PA2


extern unsigned char Decode_OK;                            //0��ʾͬ��ʱ��ʧ�ܣ�1��ʾͬ���ɹ�
extern unsigned char AUTO_BPC;                     //��ҹ�Զ�Уʱ�ɹ���־	

extern u16 YYY;
extern unsigned char MON;
extern unsigned char DDD;
extern unsigned char HHH;                        //����õ���Сʱ,����,��
extern unsigned char MMM;
extern unsigned char WWW;
extern unsigned char SSS;                  
extern unsigned char PP3;

void BPC_Decode(void);
void BPC_Decode_RE(void);
void IO_Init(void);
#endif
