#ifndef __RNG_H
#define __RNG_H	 
#include "sys.h" 

	
u8  RNG_Init(void);			//RNG��ʼ�� 
unsigned int RNG_Get_RandomNum(u8 ch);//�õ������
int RNG_Get_RandomRange(u8 ch,int min,int max);//����[min,max]��Χ�������

#endif

















