#ifndef __RNG_H
#define __RNG_H	 
#include "sys.h" 

	
u8  RNG_Init(void);			//RNG初始化 
unsigned int RNG_Get_RandomNum(u8 ch);//得到随机数
int RNG_Get_RandomRange(u8 ch,int min,int max);//生成[min,max]范围的随机数

#endif

















