/*
*********************************************************************************************************
*
*	模块名称 : RTC
*	文件名称 : bsp_rtc.c
*	版    本 : V1.0
*	说    明 : RTC底层驱动
*	
*
*********************************************************************************************************
*/

#ifndef __BSP_RTC_H
#define __BSP_RTC_H
#include "stm32f4xx.h"
#include "unixtime.h"
extern TIME_UNIX TimeNow;
void bsp_InitRTC(void);
void RTC_Config(void);
void GetTimeNow(void);
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

