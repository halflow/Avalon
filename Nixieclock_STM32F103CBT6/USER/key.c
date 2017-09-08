#include "key.h"
#include "delay.h" 
#include "exti.h"

//������ʼ������
void KEY_Init(void){
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOCʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��ͨ����ģʽ  G��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//2M
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����    �����ӵ�
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC0,1 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��ͨ����ģʽ  G��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//2M
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����    �����ӵ�
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC0,1 

  EXTIX_Init();
} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(CAL==1||SHOW==0||RTC_SWITCH==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(CAL==1)return 1;
		else if(SHOW==0)return 2;
		else if(RTC_SWITCH==0)return 3;
	}else if(CAL==0&&SHOW==1&&RTC_SWITCH==1)key_up=1; 	    
 	return 0;// �ް�������
}



















