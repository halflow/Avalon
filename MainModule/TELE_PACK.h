/******************************************************************
                         TELE_PACK.H  file

主要功能：接口定义、全局变量定义及函数申明
创建时间：2015.10.31
*******************************************************************/
/*--------------------------------------------------------------*/
/*-------------------------全局变量定义-------------------------*/
/*--------------------------------------------------------------*/
#ifndef __TELE_PACK_H__
#define __TELE_PACK_H__



void tele_pack_key();
void tele_pack_relayval();
void tele_pack_relay();
void tele_pack_scrval();
void tele_pack_scr1();
void tele_pack_scr2();
void tele_pack_id();
void tele_pack_group();
void config_success();

void tele_pack_Module_Contain_Group();


void tele_pack_shortconfig(unsigned int address);				//所有短指令行反馈
void tele_pack_stopconfig();

void tele_pack_sound();
void tele_pack_temperature();
void tele_pack_humidity();
void tele_pack_luxvalue();

void tele_pack_lux();

void order(unsigned int address);
void long_order();
void stop_order();

#endif