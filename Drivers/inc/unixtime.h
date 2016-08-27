/*
********************************************************************************
* felename :                    unixtime.h
* author   :                    sunlishuang
* date     :                    2014.08.28
* describe :                    unixtime header file.
********************************************************************************
*/
#ifndef __UNIXTIME_H
#define __UNIXTIME_H

#define BYTE_LIB_ARM              char
#define INT8U_LIB_ARM    unsigned char
#define INT16U_LIB_ARM   unsigned short
#define INT32U_LIB_ARM   unsigned int

/*unixtime structure*/
typedef struct
{
  /*普通时间*/
  INT16U_LIB_ARM UnixYear;
  INT8U_LIB_ARM  UnixMonth;
  INT8U_LIB_ARM  UnixDay;
  INT8U_LIB_ARM  UnixWeekDay;
  INT8U_LIB_ARM  UnixHour;
  INT8U_LIB_ARM  UnixMinute;
  INT8U_LIB_ARM  UnixSecond;

  /*unix时间戳*/
  INT32U_LIB_ARM Unixtime ;
}TIME_UNIX;

/*functions of unixtime converting declaration*/
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
INT8U_LIB_ARM Localtime_To_Unixtime(TIME_UNIX* Localtime);

/*
********************************************************************************
* 函数名称: Unixtime_To_Localtime(存在bug,但是方法值得深入研究)
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
INT8U_LIB_ARM Unixtime_To_Localtime_0(TIME_UNIX * Localtime);

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
* 功能描述: 将Localtime->Unixtime32位无符号整数时间格式转换成完整的Localtime时间格式;            
********************************************************************************
*/
INT8U_LIB_ARM Unixtime_To_Localtime(TIME_UNIX * Localtime);
#endif /*unixtime.h*/

