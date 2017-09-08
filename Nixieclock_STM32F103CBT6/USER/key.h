#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
	 

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/


#define CAL 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)          //��ʾ������ͣ��������
#define SHOW 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)
#define RTC_SWITCH 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)

/*���淽ʽ��ͨ��λ��������ʽ��ȡIO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/


#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3
#define WKUP_PRES   4

void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(u8 mode);

#endif
