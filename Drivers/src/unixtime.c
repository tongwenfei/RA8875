/*
********************************************************************************
* felename :               unixtime.c
* author   :               sunlishuang
* date     :               2014.08.28
* describe :               unix time transmit .
********************************************************************************
*/


/*
修改记录:
--------------------------------------------------------------------------------
2014.09.09
1.对1970年1月份进行了修正,因为北京时间是东八时区,与格林时间比,早了8个小时;
2.将类型定义格式进行了独立定义,为生成库文件做准备;
--------------------------------------------------------------------------------
*/
#define  UNIXTIME_C

/*include files*/
#include "unixtime.h"

#define START_YEAR                      1970                                    /*国际标准从1970-01-01 00:00:00开始*/
#define END_YEAR                        2106                                    /*时间戳结束时间,32位整数所代表的最大时间区间*/
#define TIME_AREA                          8
#define BIAS_TIMEAREA_TIME              (3600*TIME_AREA)                        /*默认东8时区*/
/*
********************************************************************************
* 函数名称: Unixtime_To_Localtime
* 参数    : Localtime-用户传递的形参,此函数将Localtime-Unixtime转换成普通时间并赋值
            给
            Localtime.UnixYear=***;
            Localtime.UnixMonth=***;
            Localtime.UnixDay=***;
            Localtime.UnixHour=***;
            Localtime.UnixMinute=***;
            Localtime.UnixSecond=***;            
* 功能描述: 将Localtime->Unixtime32位无符号整数时间格式转换成完整的Localtime时间格式;
* 返回值  : 0-转换成功;
            1-参数不合法;
********************************************************************************
*/
INT8U_LIB_ARM Unixtime_To_Localtime_0(TIME_UNIX * Localtime)
{
  const INT8U_LIB_ARM Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; 
  INT32U_LIB_ARM utime,n32_Pass4year=0;
  INT32U_LIB_ARM n32_hpery=0;
  
  if(!Localtime) return 1;                                                      /*checking validity of the parameter*/
  
  utime=Localtime->Unixtime;
  utime += 28800;                                                               /*加上北京时间*/  
  Localtime->UnixSecond=(INT8U_LIB_ARM)(utime % 60);                            /*取秒时间*/
  utime /= 60;  
  Localtime->UnixMinute=(INT8U_LIB_ARM)(utime % 60);                            /*取分钟时间*/  
  utime /= 60;                                                                  /*转换成小时*/
  n32_Pass4year=((INT32U_LIB_ARM)utime / ((365*4+1) * 24L ));                   /*计算过去多少个四年，每四年有 1461*24 小时*/ 
  Localtime->UnixYear=(n32_Pass4year << 2)+1970;                                /*计算年份*/  
  utime %= (365*4+1) * 24L;                                                     /*四年中剩下的小时数*/
  /*校正闰年影响的年份，计算一年中剩下的小时数*/
  for (;;)
  {    
    n32_hpery = 365 * 24;                                                       /*一年的小时数*/    
    if ((Localtime->UnixYear & 3) == 0)                                         /*判断闰年*/
    {
      n32_hpery += 24;                                                          /*是闰年，一年则多24小时，即一天*/
    }
    if (utime < n32_hpery)
    {
      break;
    }
     Localtime->UnixYear++;
     utime -= n32_hpery;
    }
    /*小时数*/
    Localtime->UnixHour=(INT8U_LIB_ARM)(utime % 24);
    /*一年中剩下的天数*/
    utime /= 24;
    /*假定为闰年*/
    utime++;
    /*校正润年的误差，计算月份，日期*/
    if ((Localtime->UnixYear & 3) == 0)      
    {
      if (utime > 60)
      {
        utime--;
      }
      else
      {
      if (utime == 60)
      {
        Localtime->UnixMonth = 2;
        Localtime->UnixDay = 29;
        return 0;
      }
    }
  }
  /*计算月日*/
  for (Localtime->UnixMonth = 0; Days[Localtime->UnixMonth] < utime;Localtime->UnixMonth++)
  {
    utime -= Days[Localtime->UnixMonth];
  }
  Localtime->UnixMonth = Localtime->UnixMonth+1;
  Localtime->UnixDay = (INT8U_LIB_ARM)(utime);
  return 0;
}
/*
********************************************************************************
* 函数名称: Unixtime_To_Localtime
* 参数    : Localtime-用户传递的形参,此函数将Localtime-Unixtime转换成普通时间并赋值
            给
            Localtime.UnixYear=***;
            Localtime.UnixMonth=***;
            Localtime.UnixDay=***;
            Localtime.UnixHour=***;
            Localtime.UnixMinute=***;
            Localtime.UnixSecond=***;  
* 返回值  : 0-转换成功;
            1-参数不合法;
            2-
            3-
            4-转换的年限超出了限制;
* 功能描述: 将Localtime->Unixtime32位无符号整数时间格式转换成完整的Localtime时间格式;            
********************************************************************************
*/
INT8U_LIB_ARM Unixtime_To_Localtime (TIME_UNIX * Localtime)
{
  INT8U_LIB_ARM daytemp;
  INT32U_LIB_ARM unixtime,restime,i;
  INT32U_LIB_ARM monthtable[13]={0,2678400,5097600,7776000,10368000,13046400,15638400,18316800,
                         20995200,23587200,26265600,28857600,31536000};         /*累计天数,默认是按照平年计算*/
  TIME_UNIX t;
  
  if(!Localtime) return 1;
  
  /*获取待转换的unix时间戳*/  
  unixtime=Localtime->Unixtime;
  
  /*比较时间赋初值:  1971-01-01 00:00:00*/  
  t.UnixYear=START_YEAR+1;                                                      
  t.UnixMonth=1;
  t.UnixDay=1;
  t.UnixHour=0;
  t.UnixMinute=0;
  t.UnixSecond=0;
  Localtime_To_Unixtime(&t);

  /*判断年份时间*/
  Localtime->UnixYear=START_YEAR;
  for(i=START_YEAR+1;i<END_YEAR;i++)                                            /*通过此循环判断了年*/
  {
    t.UnixYear=i;
    Localtime_To_Unixtime(&t);

    if(unixtime>t.Unixtime)
    {
      Localtime->UnixYear=i;                                                    /*暂时将此年份赋值给unixtime->UnixYear,再进行下次判断*/
      continue;
    }
    else if(unixtime==t.Unixtime)                                               /*相等立即赋值,并跳出*/
    {
      *Localtime=t;
      return 0;
    }         
    else 
    {
      if(t.UnixYear==(START_YEAR+1))
      {
        t.UnixHour=TIME_AREA;
      }
      t.UnixYear-=1;
      Localtime_To_Unixtime(&t);
      break;
    }
  }
  if(i>=END_YEAR) return 4;
  restime=unixtime-t.Unixtime;                                                  /*计算出超过一年的秒数*/  
  
  if(((Localtime->UnixYear% 4) == 0 && ((Localtime->UnixYear% 100) != 0 || (Localtime->UnixYear % 400) == 0)))/*判断闰年*/
  {
    /*如果是闰年,月份秒数表依次加24*3600=86400s*/
    for(i=2;i<13;i++)
    {
      monthtable[i] = monthtable[i] + 86400;
    }    
  }
  /*判断是不是1970年,因为时差的原因,要进行补偿*/
  if(Localtime->UnixYear==START_YEAR)
  {
    for(i=1;i<12;i++)
    {
      monthtable[i]-=BIAS_TIMEAREA_TIME;
    }
  }  
  
  /*判断月份,初始1月开始判断*/
  i=0;                                                                          /*从1月份开始*/
  do
  {    
    i++;
    if(i>12) return 3;
    Localtime->UnixMonth=i;
  }
  while(restime>=monthtable[i]);                                                /*月份超出当前月份的秒数,进行下月份判断*/
  
  switch(Localtime->UnixMonth)                                                  /*根据月份,判断天*/
  {
    case 1:
    /*1970-1-1 8:00:00开始计算unix时间;*/
    if(Localtime->UnixYear==START_YEAR)
    {
      restime+=BIAS_TIMEAREA_TIME;
    }
    daytemp=restime/86400;
    break;
    case 2:
    restime-=monthtable[1];
    daytemp=restime/86400;
    break;
    case 3:
    restime-=monthtable[2];
    daytemp=restime/86400;
    break;
    case 4:
    restime-=monthtable[3];
    daytemp=restime/86400;
    break;
    case 5:
    restime-=monthtable[4];
    daytemp=restime/86400;
    break;
    case 6:
    restime-=monthtable[5];
    daytemp=restime/86400;
    break;
    case 7:
    restime-=monthtable[6];
    daytemp=restime/86400;
    break;
    case 8:
    restime-=monthtable[7];
    daytemp=restime/86400;
    break;
    case 9:
    restime-=monthtable[8];
    daytemp=restime/86400;
    break;
    case 10:
    restime-=monthtable[9];
    daytemp=restime/86400;
    break;
    case 11:
    restime-=monthtable[10];
    daytemp=restime/86400;
    break;
    case 12:
    restime-=monthtable[11];
    daytemp=restime/86400;
    break;
    default:    
    break;
  }  
  Localtime->UnixDay=daytemp+1;                                                 /*由于每个月的日期从1号开始*/

  restime-=daytemp*86400;
  Localtime->UnixHour=restime/3600;                                             /*小时\分钟\秒都是从0时开始算起*/
  restime%=3600;
  Localtime->UnixMinute=restime/60;
  Localtime->UnixSecond=restime%60;
  
  return 0;  
}

/*
********************************************************************************
* 函数名称: Localtime_To_Unixtime
* 形参    : Localtime-时间结构指针,结构体中包含普通时间格式和unix时间(32位无符号)
            时间格式;
* 返回值  : 返回转换状态信息
            0-转换成功;
            1-用户传递参数不合法;
* 功能描述: 此函数将Localtime中普通时间格式转换成unix时间戳时间格式,并赋值给Localtime->Unixtime;            
********************************************************************************
*/
INT8U_LIB_ARM Localtime_To_Unixtime(TIME_UNIX* Localtime)
{
  INT8U_LIB_ARM month=Localtime->UnixMonth,day=Localtime->UnixDay, hour=Localtime->UnixHour;
  INT8U_LIB_ARM minute=Localtime->UnixMinute, second=Localtime->UnixSecond;
  INT16U_LIB_ARM i,x,unixtimeyear,year=Localtime->UnixYear;
  INT16U_LIB_ARM monthtable[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};       /*累计天数*/
  INT32U_LIB_ARM  unixtimeday,extraday;  
  
  if(!Localtime) return 1;                                                      /*checking validity of the parameter*/  
  
  for(x=1970,extraday=0;x<year;x++)                                             /*计算1970年到现在有多少闰月的多一天*/
  {
    if(((x % 4) == 0 && ((x % 100) != 0 || (x % 400) == 0)))
    {
      extraday++;
    }
  }
  unixtimeyear = year - 1970;
  if((((year) % 4) == 0 && (((year) % 100) != 0 || ((year) % 400) == 0)))       /*判断当年是否是闰年,如果是闰年修改月累计天数数组*/
  {
    for(i=2;i<12;i++)
    {
      monthtable[i] = monthtable[i] + 1;
    }
  }  
  unixtimeday = unixtimeyear*365 + monthtable[month-1] + day-1 + extraday;      /*计算总共的天数*/
  Localtime->Unixtime = unixtimeday*86400 + hour*3600+minute*60 + second-BIAS_TIMEAREA_TIME;/*按照国际标准,北京时间(东8时区)1970-1-1 8:00:00开始计算unix时间为0;*/  
  return 0;
}


