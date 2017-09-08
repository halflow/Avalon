#ifndef __Display_H
#define __Display_H
#include "sys.h"

//#define OE    PAout(6)
//#define SCK   PAout(7)
//#define SI    PAout(8)
//#define RCK   PAout(9)
#define LE    PAout(5)
#define CLK   PAout(8)
#define DIN4  PAout(9)
#define DIN3  PAout(10)
#define DIN2  PAout(11)
#define DIN1  PAout(12)
#define DOT1  PAout(6)
#define BL  PAout(7)

extern unsigned char show_flag;
extern unsigned char show_CAL;
void Display_Init(void);
void Display_CAL(void);
void Display_C(void);
void ShowNixie(void);
void Close(void);
void Open(void);
void Clock_STOP(void);
void BPC_STOP(void);
void Show_STOP(void);
void show6num(u8 n[6]);
#endif
