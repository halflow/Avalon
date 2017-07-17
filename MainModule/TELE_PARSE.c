/*****************************************************************
                        TELE_PARSE.c  file

主要功能：根据感应器输入或者接收的报文命令组装报文

创建时间：2015.10.31
*****************************************************************/
/*--------------------------------------------------------------*/
/*----------------------------头文件----------------------------*/
/*--------------------------------------------------------------*/
#include "TELE_PARSE.h"
#include "TELE_MANAGE.h"
#include "CONFIG.h"
#include "RELAY.h"
#include "SCR.h"
#include "TELE_T_R.h"
#include "TELE_PACK.h"
#include "SENSOR_T_H.h"
#include "LUX_LIGHT.h"
#include "CONTROL.h"

/*--------------------------------------------------------------*/
/*--------------------------报文解析函数-----------------------*/
/*--------------------------------------------------------------*/
unsigned char tele[10];            					//用于存放从接收缓冲队列中取出的报文
unsigned char tele_backup[10];						//用于延时跳转指令缓存
unsigned char tele_backup2[10];						//用于非延时跳转指令缓存
unsigned char jump_tele[28]={0};
unsigned char Key_Num1,Digital_Num1,Analog_Num1,Relay_Num1,Scr_Num1;
unsigned int addr;
unsigned char style;								//用以区分长短停
unsigned char Delay_Time1,Delay_Time2,Delay_Time3,Delay_Time4;							//跳转前延时时间
unsigned char Which_One;
bit Delay_Flag1,Delay_Flag2,Delay_Flag3,Delay_Flag4;
unsigned char temper;
void tele_parse()
{
	unsigned char i,Key_Num,Digital_Num,Analog_Num,Relay_Num,Scr_Num;
	unsigned char k;									//取出报文的个数
	Tele_Handle_R=0;                                  	//清空接收缓冲区报文处理标志位
	Tele_Cheak_R=0;                                   	//暂停报文接收缓冲队列检测

	k=read_tele_r(tele);								//将调用的函数里面date[]进行报文取出，赋给tele[]
	
	/*------------------------*////模块操作//*-------------------------------------*//
	if(Module_Operate_Flag)
	{	
		Module_Operate_Flag=0;
		 /*-----------------------解析报文：指令报文---------------------------*/
		if((tele[2]&0xC0)==0x00) //属性判断帧                   
		{
			if((tele[3]&0xC0)==0x00)								//判断为非跳转指令
			{
				//////继电器控制指令		
				if((tele[4]&0x0F)==0x00)                      //继电器口控制
				{
					if(rom_r(0xA801)!=0xff)						//继电器1状态发生变化判断是否已经配置并且需要触发
						Relay1_Alconfig_Flag=1;					//表示继电器1输出已配置
					if(rom_r(0xAA01)!=0xff)
						Relay2_Alconfig_Flag=1;					//表示继电器1输出已配置
					if(rom_r(0xAC01)!=0xff)
						Relay3_Alconfig_Flag=1;					//表示继电器1输出已配置
					if(rom_r(0xAE01)!=0xff)
						Relay4_Alconfig_Flag=1;					//表示继电器1输出已配置					
					switch(tele[5]&0x03)                          //检测继电器口1控制命令
					{										   
						case 0x00:	Relay_Value=Relay_Value&0xFE;   //继电器口1关
												break;
						case 0x01:	Relay_Value=Relay_Value|0x01;   //继电器口1开
												break;
						case 0x02:	Relay_Value=Relay_Value^0x01;   //继电器口1状态切换
												break;
						default:    break;                          //继电器口1不做任何改变
					}
					
					switch(tele[5]&0x0C)                          //检测继电器口2控制命令
					{
						case 0x00:	Relay_Value=Relay_Value&0xFD;   //继电器口2关
												break;
						case 0x04:	Relay_Value=Relay_Value|0x02;   //继电器口2开
												break;
						case 0x08:	Relay_Value=Relay_Value^0x02;   //继电器口2状态切换
												break;
						default:    break;                          //继电器口2不做任何改变
					}
					
					switch(tele[5]&0x30)                          //检测继电器口3控制命令
					{
						case 0x00:	Relay_Value=Relay_Value&0xFB;   //继电器口3关
												break;
						case 0x10:	Relay_Value=Relay_Value|0x04;   //继电器口3开
												break;
						case 0x20:	Relay_Value=Relay_Value^0x04;   //继电器口3状态切换
												break;
						default:    break;                          //继电器口3不做任何改变
					}
					
					switch(tele[5]&0xC0)                          //检测继电器口4控制命令
					{
						case 0x00:	Relay_Value=Relay_Value&0xF7;   //继电器口4关
												break;
						case 0x40:	Relay_Value=Relay_Value|0x08;   //继电器口4开
												break;
						case 0x80:	Relay_Value=Relay_Value^0x08;   //继电器口4状态切换
												break;
						default:    break;                          //继电器口4不做任何改变
					}
					
					switch(tele[6]&0x03)                          //检测继电器口5控制命令
					{
						case 0x00:	Relay_Value=Relay_Value&0xEF;   //继电器口5关
												break;
						case 0x01:	Relay_Value=Relay_Value|0x10;   //继电器口5开
												break;
						case 0x02:	Relay_Value=Relay_Value^0x10;   //继电器口5状态切换
												break;
						default:    break;                          //继电器口5不做任何改变
					}
					
					switch(tele[6]&0x0C)                          //检测继电器口6控制命令
					{
						case 0x00:	Relay_Value=Relay_Value&0xDF;   //继电器口6关
												break;
						case 0x04:	Relay_Value=Relay_Value|0x20;   //继电器口6开
												break;
						case 0x08:	Relay_Value=Relay_Value^0x20;   //继电器口6状态切换
												break;
						default:    break;                          //继电器口6不做任何改变
					}
					
					switch(tele[6]&0x30)                          //检测继电器口7控制命令
					{
						case 0x00:	Relay_Value=Relay_Value&0xBF;   //继电器口7关
												break;
						case 0x10:	Relay_Value=Relay_Value|0x40;   //继电器口7开
												break;
						case 0x20:	Relay_Value=Relay_Value^0x40;   //继电器口7状态切换
												break;
						default:    break;                          //继电器口7不做任何改变
					}
					
					switch(tele[6]&0xC0)                          //检测继电器口8控制命令
					{
						case 0x00:	Relay_Value=Relay_Value&0x7F;   //继电器口8关
												break;
						case 0x40:	Relay_Value=Relay_Value|0x80;   //继电器口8开
												break;
						case 0x80:	Relay_Value=Relay_Value^0x80;   //继电器口8状态切换
												break;
						default:    break;                          //继电器口8不做任何改变
					}
					
					relay_handle();                              /*继电器报文解析完毕，进行处理*/
				}
				//////硅控口控制指令
				else if((tele[4]&0x0F)==0x01)                   //硅控口1控制指令
				{
					if(rom_r(0xB801)!=0xff)						//可控硅状态发生变化判断是否需要触发
						Scr1_Alconfig_Flag=1;					//表示可控硅1已配置
					switch(tele[4]&0x70)
					{
						case 0x00:  SCR1_value=tele[5];             //bit6-bit4=000//硅控口1值设定
												SCR1_Flag=1;		//组装反馈报文标志位置1
												break;					
						case 0x10:	if(SCR1_value==0xF0)            //硅控口1状态切换
										SCR1_value=1;
									else
										SCR1_value=0xF0;								
												SCR1_Flag=1;       //可控硅口1反馈标志位置1---->>主函数
												break;
					  
						case 0x20:	SCR1_control_flag=1;            //硅控口1自增自减
												SCR1_Flag=1;
												break;
					    case 0x40:  SCR1_control_flag=1;            //硅控口1自增到X值
									
								  				break;
						case 0x50:	SCR1_control_flag=0;            //硅控口1自增自减停止
									SCR_control_counter=0;
									SCR1_status=!SCR1_status;
												SCR1_Flag=1;
												break;											
						default:		break;
					}
				}
				else if((tele[4]&0x0F)==0x02)                   //硅控口2控制指令
				{
					if(rom_r(0xBA01)!=0xff)
						Scr2_Alconfig_Flag=1;					//表示可控硅2已配置
					switch(tele[4]&0x70)
					{
						case 0x00:  SCR2_value=tele[5];             //硅控口2值设定
												SCR2_Flag=1;
												break;
						
						case 0x10:	if(SCR2_value==0xF0)            //硅控口2状态切换
										SCR2_value=1;
									else
										SCR2_value=0xF0;
												SCR2_Flag=1;
												break;
					  
						case 0x20:	SCR2_control_flag=1;            //硅控口2自增自减
												SCR2_Flag=1;
												break;
					  
						case 0x50:	SCR2_control_flag=0;            //硅控口2自增自减停止
									SCR_control_counter=0;
									SCR2_status=!SCR2_status;
												SCR2_Flag=1;
												break;											
						default:break;
					}
				}
			}
			else if((tele[3]&0x80)==0x80)					//若为跳转指令
			{
				if((tele[3]&0x40)==0x40)					//若跳转指令带有延时
				{
					led0=~led0;
					for(i=0;i<10;i++)
					{
						tele_backup[i]=tele[i];				//将报文进行备份，为了防止非跳转指令破坏跳转指令的数据
					}
					Which_One=jumptele_write();				//判断缓存空间是否满，不满写入缓存			
					switch(Which_One)						//经过此函数，已将tele_backup换写到jump_tele数组
					{
						case 1:
							switch(jump_tele[6]&0xC0)							//判断延时单位
							{
								case 0x00:Delay_Time1=(jump_tele[6]&0x0F);break;	//取出延时时间
								case 0x40:break;
								default:break;
							}
							Delay_Flag1=1;
							break;
						case 2:	
							switch(jump_tele[13]&0xC0)							//判断延时单位
							{
								case 0x00:Delay_Time2=(jump_tele[13]&0x0F);break;	//取出延时时间
								case 0x40:break;
								default:break;
							}
							Delay_Flag2=1;
							break;
						case 3:	
							switch(jump_tele[20]&0xC0)							//判断延时单位
							{
								case 0x00:Delay_Time3=(jump_tele[20]&0x0F);break;	//取出延时时间
								case 0x40:break;
								default:break;
							}
							Delay_Flag3=1;
							break;
						case 4:							
							switch(jump_tele[26]&0xC0)							//判断延时单位
							{
								case 0x00:Delay_Time4=(jump_tele[26]&0x0F);break;	//取出延时时间
								case 0x40:break;
								default:break;
							}
							Delay_Flag4=1;
							break;
						default:break;
					}
				}
				else										//若跳转指令不带有延时
				{
					led1=~led1;
					for(i=0;i<10;i++)
					{
						tele_backup2[i]=tele[i];			//将报文进行备份，为了防止非跳转指令破坏跳转指令的数据
					}
					jump_order2();
				}
			}
		}
		 /*----------------解析报文为配置报文----------------------------*///tele[2]属性，tele[3]种类帧1，tele[4]种类帧2
		else if((tele[2]&0xC0)==0x40)            			//判断报文属性 类型配置报文
		{
			if((tele[3]&0xF0)==0x00)								/*--------配置模块属性------------*/
			{				
				switch(tele[3]&0x0F)
				{
					case 0x00:
					{
						switch(tele[4]&0xF0)
						{
							case 0x00: 	ID_GP_Q[0]=tele[5];			//群ID
										break;
							case 0x10:	ID_GP_Q[1]=tele[5];			//模块ID 
										break;
							case 0x20:	ID_GP_Q[2]=tele[5];			//组ID							 
										break;		
							default: break;					
						}
						id_Gp_Q_save();
						config_success();
					}break;
					case 0x01:	
					{
						switch(tele[4])
						{
							case 0x00:delete_all_e2prom();
								break;
							default:break;
						}
						config_success();
					}break;
					default:break;					
				}
		   }		
			if((tele[3]&0xF0)==0x30)							//模拟量配置信息			
			{			
				switch(tele[3]&0x0F)						/*-------配置3路模拟量传感的阈值------*/
				{
					case 0x00:
							switch(tele[4])
							{
								case 0x50:Analog_Threshold[0]=tele[5];		//第一路上阈值
											break;
								case 0x60:Analog_Threshold[1]=tele[5];		//第一路下阈值
											break;
								default:    break;
								
							}
							break;
					case 0x01:
							switch(tele[4])
							{
								case 0x50:Analog_Threshold[2]=tele[5];		//第二路上阈值
											break;
								case 0x60:Analog_Threshold[3]=tele[5];		//第二路下阈值
											break;
								default:    break;								
							}					
							break;
					case 0x02:
							switch(tele[4])
							{
								case 0x50:Analog_Threshold[4]=tele[5];		//第三路上阈值	
											break;
								case 0x60:Analog_Threshold[5]=tele[5];		//第三路下阈值
											break;
								default:    break;
							}					
							break;
					default:break;
				}
				id_Gp_Q_save();
				config_success();

			}				
			if((tele[3]&0xF0)==0x10)				/*-----配置8路按键开关绑定--------*/
			{
				Key_Num=tele[3]&0x0F;						//获取配置的哪路按键
				for(i=0;i<191;i++)							//将先前配置信息缓存到RAM
				{
					Write_Cache[i]=rom_r(0x8200+(Key_Num<<9)+i);		//相当于key_num*512
				}
				switch(Key_Num)//tele[3]&0x0F
				{
					case 0x00:key_config();								//第一路开关						
							break;
					case 0x01:key_config();								//第二路开关							
							break;
					case 0x02:key_config();								//第三路开关
							break;
					case 0x03:key_config();								//第四路开关
							break;
					case 0x04:key_config();								//第五路开关
							break;
					case 0x05:key_config();								//第六路开关//使用中
							break;
					case 0x06:key_config();								//第七路开关//使用中
							break;
					case 0x07:key_config();								//第八路开关//使用中
							break;
					default:break;
				}
				rom_c(0x8200+(Key_Num<<9));				//先擦出对应的扇区
				for(i=0;i<191;i++)							//将缓存到RAM中修改的数据写到EEPROM
				{
					rom_w((0x8200+(Key_Num<<9)+i),Write_Cache[i]);
				}
				config_success();
			}
	        if((tele[3]&0xF0)==0x20)					/*-------配置8路数字量开关类传感器---------*/
			{
				for(i=0;i<191;i++)
					Write_Cache[i]=rom_r(0x9200+i);
				switch(tele[3]&0x0F)							//第一路：声音
				{			
					case 0x00: key_config();
							   	break;
					default:break;
				}
				rom_c(0x9200);
				for(i=0;i<191;i++)
					rom_w(0x9200+i,Write_Cache[i]);
				config_success();
			}   
			if((tele[3]&0xF0)==0x30)					/*-------配置3路模拟量传感器---------*/
			{
				for(i=0;i<191;i++)
					Write_Cache[i]=rom_r(0xA200+i);
				switch(tele[3]&0x0F)							//第1路：光照
				{
					case 0x00: key_config();
								break;
					default:break;
				}
				rom_c(0xA200);
				for(i=0;i<191;i++)
					rom_w(0xA200+i,Write_Cache[i]);	
				config_success();			
			}  
			if((tele[3]&0xF0)==0x40)					/*-----配置8路继电器状态输出----------*/
			{
				Relay_Num=tele[3]&0x0F;
				for(i=0;i<191;i++)
					Write_Cache[i]=rom_r(0xA800+(Relay_Num<<9)+i);
				switch(tele[3]&0x0F)							//第1路继电器/////先做测试
				{
					case 0x00: key_config();
								break;
					case 0x01: key_config();
								break;
					case 0x02: key_config();
								break;
					case 0x03: key_config();
								break;
					default:break;
				}
				rom_c(0xA800+(Relay_Num<<9));
//				rom_w(0x6800,0xAA);								//第一路继电器配置标志位
				for(i=0;i<191;i++)
					rom_w(0xA800+(Relay_Num<<9)+i,Write_Cache[i]);
				config_success();	
			}
			if((tele[3]&0xF0)==0x50)					/*--------配置2路可控硅状态输出-------*/
			{
				Scr_Num=(tele[3]&0x0F);
				for(i=0;i<191;i++)
					Write_Cache[i]=rom_r(0xB800+(Scr_Num<<9)+i);
				switch(Scr_Num)//tele[3]&0x0F							
				{
					case 0x00: key_config();					//第1路可控硅配置
								break;
					case 0x01: key_config();					//第二路可控硅配置
								break;
					default:break;
				}
				rom_c(0xB800+(Scr_Num<<9));
				for(i=0;i<191;i++)
					rom_w(0xB800+(Scr_Num<<9)+i,Write_Cache[i]);
				config_success();	
			}			                    			
		}
		 /*---------------------------解析报文为反馈报文------------------------------------*/
		else if((tele[2]&0xC0)==0x80)                      
		{
			//主模块不会接收到反馈报文，只有网关模块会接收反馈报文
		}
		/*----------------------------解析报文为查询报文-----------------------------------*/
		else if((tele[2]&0xC0)==0xC0)                       
		{				
				switch(tele[3]&0xF0)							//数据域的第一帧高四位：查询类型
				{			
					case 0x00:										/*------查询模块属性-------*/
						switch(tele[3]&0x0F)
						{
							case 0x00: tele_pack_id();break;					//模块ID查询
							case 0x01: tele_pack_Module_Contain_Group();break;	//模块组查询
							default: break;
						}
						break;	
					case 0x10:										/*--------查询开关配置-----*/
						Key_Num=tele[3]&0x0F;
						Key_Num1=tele[3];
						Digital_Num1=0;Analog_Num1=0;Relay_Num1=0;Scr_Num1=0;
						addr=0x8200+(Key_Num<<9);

						switch(Key_Num)//tele[3]&0x0F							
						{
							case 0x00: 	key_inquire(addr);								//第1路开关查询									
									break;
							case 0x01:	key_inquire(addr);								//第2路开关查询									
									break;
							case 0x02:	key_inquire(addr);								//第3路开关查询
									break;
							case 0x03:	key_inquire(addr);								//第4路开关查询	
									break;
							case 0x04:	key_inquire(addr);								//第5路开关查询
									break;
							case 0x05: 	key_inquire(addr);								//第6路开关查询
									break;
							case 0x06:	key_inquire(addr);								//第7路开关查询
									break;
							case 0x07:	key_inquire(addr);								//第8路开关查询
									break;
							default: break;
						}
						break;
					case 0x20:									/*-----查询数字量传感器配置----*/
						Digital_Num=tele[3]&0x0F;				//判断第几路
						Digital_Num1=tele[3];
						Key_Num1=0;Analog_Num1=0;Relay_Num1=0;Scr_Num1=0;
						addr=0x9200+(Digital_Num<<9);

						switch(Digital_Num)//tele[3]&0x0F				
						{
							case 0x00:						//第一路数字量传感器
								switch(tele[4]&0xF0)
								{
									case 0x00:key_inquire(addr);break;		//触发出现
									case 0x10:key_inquire(addr);break;		//触发消失
									default: break;
								}break;
							case 0x01:						//第二路数字量传感器
								switch(tele[4]&0xF0)
								{
									case 0x00:key_inquire(addr);break;
									case 0x10:key_inquire(addr);break;
									default: break;
								}break;
							default: break;
						}
						break;
					case 0x30: 									/*-----查询模拟量传感器配置----*/
							Analog_Num=tele[3]&0x0F;
							Analog_Num1=tele[3];
							Key_Num1=0;Digital_Num1=0;Relay_Num1=0;Scr_Num1=0;
							addr=0xA200+(Analog_Num<<9);
							
							switch(Analog_Num)//tele[3]&0x0F
							{
								case 0x00:
										switch(tele[4])
										{
											case 0x00:key_inquire(addr);					//上阈值触发配置信息查询
												break;
											case 0x10:key_inquire(addr);					//下阈值配置信息查询
												break;
											case 0x40:									//上下阈值配置信息查询
												break;
											default: break;
										}
									break;
							}
							break;							
					case 0x40:								/*-----查询继电器输出状态配置-------*/
							Relay_Num=tele[3]&0x0F;
							Relay_Num1=tele[3];
							Key_Num1=0;Digital_Num1=0;Analog_Num1=0;Scr_Num1=0;
							addr=0xA800+(Relay_Num<<9);
						
							switch(Relay_Num)//tele[3]&0x0F
							{
								case 0x00:					//第一路继电器配置查询
										switch(tele[4])
										{
											case 0x00:key_inquire(addr);break;			//继电器开启配置查询
											case 0x01:key_inquire(addr);break;			//继电器关闭配置查询
											default:break;
										}
										break;
								case 0x01:					//第二路继电器配置查询
										switch(tele[4])
										{
											case 0x00:key_inquire(addr);break;			//继电器开启配置查询
											case 0x10:key_inquire(addr);break;			//继电器关闭配置查询
											default:break;
										}break;
								case 0x02:					//第三路继电器配置查询
										switch(tele[4])
										{
											case 0x00:key_inquire(addr);break;			//继电器开启配置查询
											case 0x10:key_inquire(addr);break;			//继电器关闭配置查询
											default:break;
										}break;	
								case 0x03:					//第三路继电器配置查询
										switch(tele[4])
										{
											case 0x00:key_inquire(addr);break;			//继电器开启配置查询
											case 0x10:key_inquire(addr);break;			//继电器关闭配置查询
											default:break;
										}break;								
							}break;
					case 0x50:									/*------查询可控硅状态输出配置--------*/
							Scr_Num=tele[3]&0x0F;
							Scr_Num1=tele[3];
							Key_Num1=0;Digital_Num1=0;Analog_Num1=0;Relay_Num1=0;
							addr=0xB800+(Scr_Num<<9);
			
							switch(Scr_Num)//tele[3]&0x0f
							{
								case 0x00:						//第一路可控硅配置查询
										switch(tele[4])
										{
											case 0x00:key_inquire(addr);break;	//查询亮度0%配置信息
											case 0x10:key_inquire(addr);break;
											case 0x20:key_inquire(addr);break;	//查询亮度100%配置信息
											default:break;
										}break;
								case 0x01:						//第二路可控硅配置查询
										switch(tele[4])
										{
											case 0x00:key_inquire(addr);break;
											case 0x10:key_inquire(addr);break;
											case 0x20:key_inquire(addr);break;
											default:break;
										}break;
							}break;
					case 0x60:									/*------查询模拟量信息--------*/
							switch(tele[3]&0x0F)
							{
								case 0x00: tele_pack_luxvalue();			//光照强度信息
										break;
								case 0x01: tele_pack_temperature();			//温度值组装函数
										break;
								case 0x02: tele_pack_humidity();			//湿度值组装函数
										break;
								default: break;
							} 
							break;
					case 0x70:	Relay_Flag=1;					/*-------继电器状态查询-------*/
										break;
					case 0x80:	SCR1_Flag=1;					/*--------可控硅状态查询------*/											
								SCR2_Flag=1;
									break;
					default: break;
				}			
		}
	}
	/*---------------------------*///组操作////*--------------------------------*/
	else if(Group_Operate_Flag)							
	{
		Group_Operate_Flag=0;
		if((tele[2]&0xC0)==0xC0)				//查询报文
		{
				switch(tele[3])			
				{
					case 0x00: 
							switch(tele[4])
							{						
							   case 0x00:Group_a=ID_GP_Q[2];
							   			 tele_pack_group();					//该模块从属于哪些组内
							  		     break;
								default: break;
							}
							break;
					default :break;					
				}	
		}
		/*-----------------------解析报文：指令报文---------------------------*/
		if((tele[2]&0xC0)==0x00)                    
		{
			//////继电器控制指令		
			if((tele[4]&0x0F)==0x00)                      //继电器口控制
			{
				switch(tele[5]&0x03)                          //检测继电器口1控制命令
				{										   
					case 0x00:	Relay_Value=Relay_Value&0xFE;   //继电器口1关
											break;
					case 0x01:	Relay_Value=Relay_Value|0x01;   //继电器口1开
											break;
					case 0x02:	Relay_Value=Relay_Value^0x01;   //继电器口1状态切换
											break;
					default:    break;                          //继电器口1不做任何改变
				}
				
				switch(tele[5]&0x0C)                          //检测继电器口2控制命令
				{
					case 0x00:	Relay_Value=Relay_Value&0xFD;   //继电器口2关
											break;
					case 0x04:	Relay_Value=Relay_Value|0x02;   //继电器口2开
											break;
					case 0x08:	Relay_Value=Relay_Value^0x02;   //继电器口2状态切换
											break;
					default:    break;                          //继电器口2不做任何改变
				}
				
				switch(tele[5]&0x30)                          //检测继电器口3控制命令
				{
					case 0x00:	Relay_Value=Relay_Value&0xFB;   //继电器口3关
											break;
					case 0x10:	Relay_Value=Relay_Value|0x04;   //继电器口3开
											break;
					case 0x20:	Relay_Value=Relay_Value^0x04;   //继电器口3状态切换
											break;
					default:    break;                          //继电器口3不做任何改变
				}
				
				switch(tele[5]&0xC0)                          //检测继电器口4控制命令
				{
					case 0x00:	Relay_Value=Relay_Value&0xF7;   //继电器口4关
											break;
					case 0x40:	Relay_Value=Relay_Value|0x08;   //继电器口4开
											break;
					case 0x80:	Relay_Value=Relay_Value^0x08;   //继电器口4状态切换
											break;
					default:    break;                          //继电器口4不做任何改变
				}
				
				switch(tele[6]&0x03)                          //检测继电器口5控制命令
				{
					case 0x00:	Relay_Value=Relay_Value&0xEF;   //继电器口5关
											break;
					case 0x01:	Relay_Value=Relay_Value|0x10;   //继电器口5开
											break;
					case 0x02:	Relay_Value=Relay_Value^0x10;   //继电器口5状态切换
											break;
					default:    break;                          //继电器口5不做任何改变
				}
				
				switch(tele[6]&0x0C)                          //检测继电器口6控制命令
				{
					case 0x00:	Relay_Value=Relay_Value&0xDF;   //继电器口6关
											break;
					case 0x04:	Relay_Value=Relay_Value|0x20;   //继电器口6开
											break;
					case 0x08:	Relay_Value=Relay_Value^0x20;   //继电器口6状态切换
											break;
					default:    break;                          //继电器口6不做任何改变
				}
				
				switch(tele[6]&0x30)                          //检测继电器口7控制命令
				{
					case 0x00:	Relay_Value=Relay_Value&0xBF;   //继电器口7关
											break;
					case 0x10:	Relay_Value=Relay_Value|0x40;   //继电器口7开
											break;
					case 0x20:	Relay_Value=Relay_Value^0x40;   //继电器口7状态切换
											break;
					default:    break;                          //继电器口7不做任何改变
				}
				
				switch(tele[6]&0xC0)                          //检测继电器口8控制命令
				{
					case 0x00:	Relay_Value=Relay_Value&0x7F;   //继电器口8关
											break;
					case 0x40:	Relay_Value=Relay_Value|0x80;   //继电器口8开
											break;
					case 0x80:	Relay_Value=Relay_Value^0x80;   //继电器口8状态切换
											break;
					default:    break;                          //继电器口8不做任何改变
				}
				
				relay_handle();                              /*继电器报文解析完毕，进行处理*/
			}
			//////硅控口控制指令
			else if((tele[4]&0x0F)==0x01)                   //硅控口1控制指令
			{
				switch(tele[4]&0x70)
				{
					case 0x00:  SCR1_value=tele[5];             //bit6-bit4=000//硅控口1值设定
											SCR1_Flag=1;		//组装反馈报文标志位置1
											break;
					
					case 0x10:	if(SCR1_value==0xF0)            //001//硅控口1状态切换
									SCR1_value=1;
								else
									SCR1_value=0xF0;								
											SCR1_Flag=1;                    //可控硅口1反馈标志位置1
											break;
				  
					case 0x20:	SCR1_control_flag=1;            //硅控口1自增自减
											SCR1_Flag=1;
											break;
				  
					case 0x50:	SCR1_control_flag=0;            //硅控口1自增自减停止
								SCR_control_counter=0;
								SCR1_status=!SCR1_status;
											SCR1_Flag=1;
											break;											
					default:		break;
				}
			}
			else if((tele[4]&0x0F)==0x02)                   //硅控口2控制指令
			{
				switch(tele[5]&0x70)
				{
					case 0x00:  SCR2_value=tele[4];             //硅控口2值设定
								SCR2_Flag=1;
											break;
					
					case 0x10:	if(SCR2_value==0xF0)            //硅控口2状态切换
									SCR2_value=1;
								else
									SCR2_value=0xF0;
								SCR2_Flag=1;
											break;
				  
					case 0x20:	SCR2_control_flag=1;            //硅控口2自增自减
								SCR2_Flag=1;
											break;
				  
					case 0x50:	SCR2_control_flag=0;            //硅控口2自增自减停止
								SCR_control_counter=0;
								SCR2_status=!SCR2_status;
								SCR2_Flag=1;
											break;										
					default:break;
				}
			}
		}
	}
	Tele_Cheak_R=1;                                   //恢复报文接收缓冲队列检测
}
/*-------------------------------------------------------------*/
/*------------------保存配置信息：信息存储函数-----------------*/
/*-------------------------------------------------------------*/
void key_config()									//开关配置
{
	unsigned char i,j;
	switch(tele[4]&0xF0)
	{
		case 0x00:							//短按配置
				switch(tele[4]&0x07)
				{
					case 0x00:					
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+1]=tele[5+i];			//目标1短按报文帧配置
							}				
							break;
					case 0x01:	
							Write_Cache[9]=tele[5];					//目标1短按参数配置（暂定一个延时时间）
							break;								
					case 0x02:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+17]=tele[5+i];		//目标2短按报文帧配置
							}				
							break;
					case 0x03:	
							Write_Cache[25]=tele[5];				//目标2短按参数配置
							break;
					case 0x04:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+33]=tele[5+i];		//目标3短按报文帧配置
							}				
							break;
					case 0x05:	
							Write_Cache[41]=tele[5];				//目标3短按参数配置
							break;
					case 0x06:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+49]=tele[5+i];		//目标4短按报文帧配置
							}				
							break;
					case 0x07:	
							Write_Cache[57]=tele[5];				//目标4短按参数配置
							break;
					default:break;
				}
				break;
		case 0x10:										//长按配置
				switch(tele[4]&0x07)
				{
					case 0x00:									
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+65]=tele[5+i];		//目标1长按报文帧配置
							}				
							break;
					case 0x01:	
							Write_Cache[73]=tele[5];				//目标1长按参数配置
							break;								
					case 0x02:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+81]=tele[5+i];		//目标2长按报文帧配置
							}				
							break;
					case 0x03:	
							Write_Cache[89]=tele[5];				//目标2长按参数配置
							break;
					case 0x04:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+97]=tele[5+i];		//目标3长按报文帧配置
							}				
							break;
					case 0x05:	
							Write_Cache[105]=tele[5];				//目标3长按参数配置
							break;
					case 0x06:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+113]=tele[5+i];		//目标4长按报文帧配置
							}				
							break;
					case 0x07:	
							Write_Cache[121]=tele[5];				//目标4长按参数配置
							break;
					default:break;
				}
				break;
		case 0x20:								//长按停止配置
				switch(tele[4]&0x07)
				{
					case 0x00:									
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+129]=tele[5+i];		//目标1长停按报文帧配置
							}				
							break;
					case 0x01:	
							Write_Cache[137]=tele[5];				//目标1长停按参数配置
							break;								
					case 0x02:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+145]=tele[5+i];		//目标2长停按报文帧配置
							}				
							break;
					case 0x03:	
							Write_Cache[153]=tele[5];				//目标2长停按参数配置
							break;
					case 0x04:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+161]=tele[5+i];		//目标3长停按报文帧配置
							}				
							break;
					case 0x05:	
							Write_Cache[169]=tele[5];				//目标3长停按参数配置
							break;
					case 0x06:
							for(i=0;i<((tele[5]&0x0F)+3);i++)
							{
								Write_Cache[i+177]=tele[5+i];		//目标4长停按报文帧配置
							}				
							break;
					case 0x07:	
							Write_Cache[185]=tele[5];				//目标4长停按参数配置
							break;
					default:break;
				}
				break;
		case 0x30:
				switch(tele[4]&0x07)								//删除目标配置
				{
					case 0x00:										//删除目标1配置
							for(j=0;j<3;j++)
							{
								for(i=0;i<16;i++)
								{
									Write_Cache[i+64*j]=0xff;
								}	
							}break;
					case 0x01:										//删除目标2配置
							for(j=0;j<3;j++)
							{
								for(i=0;i<16;i++)
								{
									Write_Cache[16+i+64*j]=0xff;
								}	
							}break;
					case 0x02:										//删除目标3配置
							for(j=0;j<3;j++)
							{
								for(i=0;i<16;i++)
								{
									Write_Cache[32+i+64*j]=0xff;
								}	
							}break;
					case 0x03:										//删除目标4配置
							for(j=0;j<3;j++)
							{
								for(i=0;i<16;i++)
								{
									Write_Cache[48+i+64*j]=0xff;
								}	
							}break;
					default:	break;									
				}
				break;	
		case 0x40:													//删除所有目标配置
				{
					switch(tele[4]&0x0F)
					{
						case 0x00:
								for(i=0;i<192;i++)
								{
									Write_Cache[i]=0xFF;
								}break;
						default:	break;		
					}
				}break;	
		default:	break;										
	}
}
void key_inquire(unsigned int add)
{
	switch(tele[4]&0xF0)
	{
		case 0x00:  style=0x00;
					tele_pack_shortconfig(add);					//短指令行配置查询
					break;						
		case 0x10:  style=0x10;
					tele_pack_shortconfig(add+64); 				//长指令行配置查询
					break;
		case 0x20:  style=0x20;
					tele_pack_shortconfig(add+128); 			//停指令行配置查询
					break;
//		case 0x30: key_all();  break;							//查询所有指令行(空留)
		default:   break;
	}
}
void jump_order(unsigned char JumpNum)									//定时器时间到就执行
{
	unsigned char tele_backup3[7];
	unsigned char i;
	for(i=0;i<7;i++)
		tele_backup3[i]=jump_tele[JumpNum+i];

	switch(tele_backup3[4]&0xC0)		//指令表A--------对应的8路按键的指令内容
	{
		case 0x40:								//指令表A的“短”指令行
					if(tele_backup3[5]&0x01)			//指令表A指令组1
					{Key_Value=0x80;tele_pack_key();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x81;tele_pack_key();}
					if(tele_backup3[5]&0x04)			//指令表A指令组3
					{Key_Value=0x82;tele_pack_key();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x83;tele_pack_key();}
					if(tele_backup3[5]&0x10)			//指令表A指令组5
					{Key_Value=0x84;tele_pack_key();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x85;tele_pack_key();}
					if(tele_backup3[5]&0x40)			//指令表A指令组7
					{Key_Value=0x86;tele_pack_key();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x87;tele_pack_key();}																
				break;
		case 0x80:								//指令表A的“长”指令行
					if(tele_backup3[5]&0x01)			//指令表A指令组1
					{Key_Value=0x40;tele_pack_key();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x41;tele_pack_key();}
					if(tele_backup[5]&0x04)			//指令表A指令组3
					{Key_Value=0x42;tele_pack_key();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x43;tele_pack_key();}
					if(tele_backup3[5]&0x10)			//指令表A指令组5
					{Key_Value=0x44;tele_pack_key();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x45;tele_pack_key();}
					if(tele_backup3[5]&0x40)			//指令表A指令组7
					{Key_Value=0x46;tele_pack_key();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x47;tele_pack_key();}																
				break;
		case 0xC0:								//指令表A的“停”指令行
					if(tele_backup3[5]&0x01)			//指令表A指令组1
					{Key_Value=0x20;tele_pack_key();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x21;tele_pack_key();}
					if(tele_backup3[5]&0x04)			//指令表A指令组3
					{Key_Value=0x22;tele_pack_key();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x23;tele_pack_key();}
					if(tele_backup3[5]&0x10)			//指令表A指令组5
					{Key_Value=0x24;tele_pack_key();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x25;tele_pack_key();}
					if(tele_backup3[5]&0x40)			//指令表A指令组7
					{Key_Value=0x26;tele_pack_key();}		
					if(tele_backup3[5]&0x80)
					{Key_Value=0x27;tele_pack_key();}
				break;
		default :break;
	}
	switch(tele_backup3[4]&0x30)			//指令表B----对应着8路感应量的指令内容
	{
		case 0x10:								//指令表B的短指令行
				if(tele_backup3[5]&0x01)				//指令表B指令组1
					{Key_Value=0x80;tele_pack_sound();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x81;tele_pack_sound();}
					if(tele_backup3[5]&0x04)			//指令表B指令组3
					{Key_Value=0x82;tele_pack_sound();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x83;tele_pack_sound();}
					if(tele_backup3[5]&0x10)			//指令表B指令组5
					{Key_Value=0x84;tele_pack_sound();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x85;tele_pack_sound();}
					if(tele_backup3[5]&0x40)			//指令表B指令组7
					{Key_Value=0x86;tele_pack_sound();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x87;tele_pack_sound();}																
				break;
		case 0x20:								//指令表B的“长”指令行
					if(tele_backup3[5]&0x01)			//指令表B指令组1
					{Key_Value=0x40;tele_pack_sound();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x41;tele_pack_sound();}
					if(tele_backup3[5]&0x04)			//指令表B指令组3
					{Key_Value=0x42;tele_pack_sound();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x43;tele_pack_sound();}
					if(tele_backup3[5]&0x10)			//指令表B指令组5
					{Key_Value=0x44;tele_pack_sound();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x45;tele_pack_sound();}
					if(tele_backup3[5]&0x40)			//指令表B指令组7
					{Key_Value=0x46;tele_pack_sound();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x47;tele_pack_sound();}																
				break;	
		case 0x30:								//指令表B的“停”指令行
					if(tele_backup3[5]&0x01)			//指令表B指令组1
					{Key_Value=0x20;tele_pack_sound();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x21;tele_pack_sound();}
					if(tele_backup3[5]&0x04)			//指令表B指令组3
					{Key_Value=0x22;tele_pack_sound();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x23;tele_pack_sound();}
					if(tele_backup3[5]&0x10)			//指令表B指令组5
					{Key_Value=0x24;tele_pack_sound();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x25;tele_pack_sound();}
					if(tele_backup3[5]&0x40)			//指令表B指令组7
					{Key_Value=0x26;tele_pack_sound();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x27;tele_pack_sound();}
				break;
		default :break;				
	}
	switch(tele_backup3[4]&0x0C)				//指令表C-----对应着8（6）路继电器（和2路可控硅）
	{
		case 0x04:								//指令表C的短指令行
				if(tele_backup3[5]&0x01)				//指令表C指令组1
					{Key_Value=0x80;tele_pack_relay();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x81;tele_pack_relay();}
					if(tele_backup3[5]&0x04)			//指令表C指令组3
					{Key_Value=0x82;tele_pack_relay();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x83;tele_pack_relay();}
					if(tele_backup3[5]&0x10)			//指令表C指令组5
					{Key_Value=0x84;tele_pack_relay();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x85;tele_pack_relay();}
					if(tele_backup3[5]&0x40)			//指令表C指令组7
					{Key_Value=0x86;tele_pack_relay();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x87;tele_pack_relay();}																
				break;
		case 0x08:								//指令表C的“长”指令行
					if(tele_backup3[5]&0x01)			//指令表C指令组1
					{Key_Value=0x40;tele_pack_relay();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x41;tele_pack_relay();}
					if(tele_backup3[5]&0x04)			//指令表C指令组3
					{Key_Value=0x42;tele_pack_relay();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x43;tele_pack_relay();}
					if(tele_backup3[5]&0x10)			//指令表C指令组5
					{Key_Value=0x44;tele_pack_relay();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x45;tele_pack_relay();}
					if(tele_backup3[5]&0x40)			//指令表C指令组7
					{Key_Value=0x46;tele_pack_relay();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x47;tele_pack_relay();}																
				break;	
		case 0x0c:								//指令表C的“停”指令行
					if(tele_backup3[5]&0x01)			//指令表C指令组1
					{Key_Value=0x20;tele_pack_relay();}			
					if(tele_backup3[5]&0x02)
					{Key_Value=0x21;tele_pack_relay();}
					if(tele_backup3[5]&0x04)			//指令表C指令组3
					{Key_Value=0x22;tele_pack_relay();}			
					if(tele_backup3[5]&0x08)
					{Key_Value=0x23;tele_pack_relay();}
					if(tele_backup3[5]&0x10)			//指令表C指令组5
					{Key_Value=0x24;tele_pack_relay();}			
					if(tele_backup3[5]&0x20)
					{Key_Value=0x25;tele_pack_relay();}
					if(tele_backup3[5]&0x40)			//指令表C指令组7
					{Key_Value=0x26;tele_pack_relay();}			
					if(tele_backup3[5]&0x80)
					{Key_Value=0x27;tele_pack_relay();}
				break;
		default :break;				
	}	
}
void jump_order2()									//非延时跳转，仅跳转到“短”指令行		
{
	switch(tele_backup2[4]&0xC0)		//指令表A--------对应的8路按键的指令内容
	{
		case 0x40:
		{								//指令表A的“短”指令行
				if(tele_backup2[5]&0x01)			//指令表A指令组1
				{Key_Value=0x80;tele_pack_key();}	//led0=~led0;		
				if(tele_backup2[5]&0x02)
				{Key_Value=0x81;tele_pack_key();}//  led1=~led1;
				if(tele_backup2[5]&0x04)			//指令表A指令组3
				{Key_Value=0x82;tele_pack_key();}			
				if(tele_backup2[5]&0x08)
				{Key_Value=0x83;tele_pack_key();}
				if(tele_backup2[5]&0x10)			//指令表A指令组5
				{Key_Value=0x84;tele_pack_key();}			
				if(tele_backup2[5]&0x20)
				{Key_Value=0x85;tele_pack_key();}
				if(tele_backup2[5]&0x40)			//指令表A指令组7
				{Key_Value=0x86;tele_pack_key();}			
				if(tele_backup2[5]&0x80)
				{Key_Value=0x87;tele_pack_key();}																
			
		}break;
		default :break;
	}
	switch(tele_backup2[4]&0x30)			//指令表B----对应着8路感应量的指令内容
	{
		case 0x10:								//指令表B的短指令行
		{
					if(tele_backup2[5]&0x01)				//指令表B指令组1
					{Key_Value=0x80;tele_pack_sound();}			
					if(tele_backup2[5]&0x02)
					{Key_Value=0x81;tele_pack_sound();}
					if(tele_backup2[5]&0x04)			//指令表B指令组3
					{Key_Value=0x82;tele_pack_sound();}			
					if(tele_backup2[5]&0x08)
					{Key_Value=0x83;tele_pack_sound();}
					if(tele_backup2[5]&0x10)			//指令表B指令组5
					{Key_Value=0x84;tele_pack_sound();}			
					if(tele_backup2[5]&0x20)
					{Key_Value=0x85;tele_pack_sound();}
					if(tele_backup2[5]&0x40)			//指令表B指令组7
					{Key_Value=0x86;tele_pack_sound();}			
					if(tele_backup2[5]&0x80)
					{Key_Value=0x87;tele_pack_sound();}	
		}break;
		default :break;				
	}
	switch(tele_backup2[4]&0x0C)				//指令表C-----对应着8（6）路继电器（和2路可控硅）
	{
		case 0x04:								//指令表C的短指令行
		{
					if(tele_backup2[5]&0x01)				//指令表C指令组1
					{Key_Value=0x80;tele_pack_relay();}			
					if(tele_backup2[5]&0x02)
					{Key_Value=0x81;tele_pack_relay();}
					if(tele_backup2[5]&0x04)			//指令表C指令组3
					{Key_Value=0x82;tele_pack_relay();}			
					if(tele_backup2[5]&0x08)
					{Key_Value=0x83;tele_pack_relay();}
					if(tele_backup2[5]&0x10)			//指令表C指令组5
					{Key_Value=0x84;tele_pack_relay();}			
					if(tele_backup2[5]&0x20)
					{Key_Value=0x85;tele_pack_relay();}
					if(tele_backup2[5]&0x40)			//指令表C指令组7
					{Key_Value=0x86;tele_pack_relay();}			
					if(tele_backup2[5]&0x80)
					{Key_Value=0x87;tele_pack_relay();}	
		}break;																		
		default :break;				
	}	
}
bit jumptele_cache()
{
	unsigned char i;
	bit Tele_Full;
	for(i=0;i<22;i+=7)
	{
		temper=temper<<1;
		if(jump_tele[21-i]==0x00)
			temper=temper|0x01;						//用Temper的低四位来判断四个缓存区是否有空
		else 										//那一位为1代表空
			temper=temper&0xfe;
		temper&=0x0F;
	}
	if((temper&0x0F)!=0x00)
		Tele_Full=1;								//表示缓存有空
	else 
		Tele_Full=0;								//表示缓存已满		
	
	return Tele_Full;
}
unsigned char jumptele_write()
{
	unsigned char i;
	if(jumptele_cache())							//若有空，将报文写入缓存区
	{											//依次判断，优先机制
		if(temper&0x01)
		{	
			for(i=0;i<7;i++)
				jump_tele[i]=tele_backup[i];
			return 1;
		}
		else if(temper&0x02)
		{
			for(i=0;i<7;i++)
				jump_tele[7+i]=tele_backup[i];
			return 2;
		}
		else if(temper&0x04)
		{
			for(i=0;i<7;i++)
				jump_tele[14+i]=tele_backup[i];
			return 3;
		}
		else
		{
			for(i=0;i<7;i++)
				jump_tele[21+i]=tele_backup[i];
			return 4;
		}
	}
	else 
		return 0;
}
