
#include "rtc.h"
#include "unixtime.h"


#define RTC_Debug                      0     /* 用于选择调试模式0-普通模式,1-调试模式; */

/* 选择RTC的时钟源 */
#define RTC_CLOCK_SOURCE_LSE           0     /* LSE */
#define RTC_CLOCK_SOURCE_LSI           1     /* LSI */ 
#define RTC_SOURCE   RTC_CLOCK_SOURCE_LSE    /*时钟源选择*/


/* 变量 */
RTC_TimeTypeDef  RTC_TimeStructure;
RTC_InitTypeDef  RTC_InitStructure;
RTC_DateTypeDef  RTC_DateStructure;


TIME_UNIX TimeNow;                              /*系统时间-北京时间,年月日周时分秒信息*/
static void RTC_Config(void);

/* 用于设置RTC分频 */
__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
/*
*********************************************************************************************************
*  函 数 名: bsp_InitRTC
*  功能说明: 初始化RTC
*  形    参：无
*  返 回 值: 无            
*********************************************************************************************************
*/
void bsp_InitRTC(void)
{  
  /* 用于检测是否已经配置过RTC，如果配置过的话，会在配置结束时
  设置RTC备份寄存器为0x32F2。如果检测RTC备份寄存器不是0x32F2
  那么表示没有配置过，需要配置RTC.
  */
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
  {  
    /* RTC 配置  */
    RTC_Config();
    
    /* 打印调试信息 */
#if (RTC_Debug==1)
    printf("第一次使用RTC \n\r");
#endif   
    
    /* 检测上电复位标志是否设置 */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      /* 发生上电复位 */
#if (RTC_Debug==1)
      printf("发生上电复位 \n\r");
#endif
    }
  }
  else
  {
    /* 打印调试信息 */
#if (RTC_Debug==1)
    printf("第n次使用RTC \n\r");
#endif
    
    /* 检测上电复位标志是否设置 */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      /* 发生上电复位 */
#if (RTC_Debug==1)
      printf("发生上电复位 \n\r");
#endif
    }
    /* 检测引脚复位标志是否设置 */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      /* 发生引脚复位 */
#if (RTC_Debug==1)
      printf("发生引脚复位 \n\r");
#endif      
    }
    
    /* 使能PWR时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
    /* 允许访问RTC */
    PWR_BackupAccessCmd(ENABLE);
    
    /* 等待 RTC APB 寄存器同步 */
    RTC_WaitForSynchro();
    
    /* 清除RTC闹钟标志 */
    RTC_ClearFlag(RTC_FLAG_ALRAF);
    
    /* 清除RTC闹钟中断挂起标志 */
    EXTI_ClearITPendingBit(EXTI_Line17);
  }
  
}

/*
*********************************************************************************************************
*  函 数 名: RTC_Config
*  功能说明: 用于配置时间戳功能
*  形    参：无
*  返 回 值: 无
*********************************************************************************************************
*/
static void RTC_Config(void)
{  
  /* 使能PWR时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* 允许访问RTC */
  PWR_BackupAccessCmd(ENABLE);
  
  /* 选择LSI作为时钟源 */
#if (RTC_SOURCE==RTC_CLOCK_SOURCE_LSI)
  
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  
  /* 选择RTC时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;
  
  /* 选择LSE作为RTC时钟 */
#elif (RTC_SOURCE==RTC_CLOCK_SOURCE_LSE)
  /* 使能LSE振荡器  */
  RCC_LSEConfig(RCC_LSE_ON);
  
  /* 等待就绪 */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  
  /* 选择RTC时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;
  
#else
#error "Please select the RTC Clock source inside the main.c file"
#endif 
  
  /* 使能RTC时钟 */
  RCC_RTCCLKCmd(ENABLE);
  
  /* 等待RTC APB寄存器同步 */
  RTC_WaitForSynchro();
  
  /* 使能时间戳 */
  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);  
  
  RTC_TimeStampPinSelection(RTC_TamperPin_PC13);  
  
  /* 配置RTC数据寄存器和分频器  */
  RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  
  /* 检测RTC初始化 */
  if (RTC_Init(&RTC_InitStructure) == ERROR)
  {
    /* 打印调试信息 */
#if (RTC_Debug==1)
    printf("RTC初始化失败 \n\r");
#endif
  }
  
  /* 设置年月日和星期 */
  RTC_DateStructure.RTC_Year = 0x13;
  RTC_DateStructure.RTC_Month = RTC_Month_January;
  RTC_DateStructure.RTC_Date = 0x11;
  RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
  
  /* 设置时分秒，以及显示格式 */
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours   = 0x05;
  RTC_TimeStructure.RTC_Minutes = 0x20;
  RTC_TimeStructure.RTC_Seconds = 0x00; 
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
  
  /* 配置备份寄存器，表示已经设置过RTC */
  RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}
/*
*********************************************************************************************************
*  函 数 名: GetTimeNow
*  功能说明: 读取系统时间,并转换成unix时间戳;
*  形    参：无
*  返 回 值: 无
*********************************************************************************************************
*/
void GetTimeNow(void)
{  
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

  /*年月日时分秒*/
  TimeNow.UnixYear=RTC_DateStructure.RTC_Year+2000;
  TimeNow.UnixMonth=RTC_DateStructure.RTC_Month;
  TimeNow.UnixDay=RTC_DateStructure.RTC_Date;
  TimeNow.UnixWeekDay=RTC_DateStructure.RTC_WeekDay;
  TimeNow.UnixHour=RTC_TimeStructure.RTC_Hours;
  TimeNow.UnixMinute=RTC_TimeStructure.RTC_Minutes;
  TimeNow.UnixSecond=RTC_TimeStructure.RTC_Seconds;

  /*转换UNIXTIME*/
  Localtime_To_Unixtime(&TimeNow);
}

