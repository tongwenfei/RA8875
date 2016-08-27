/*
*********************************************************************************************************
*
* 模块名称 : 串行EEPROM 24xx驱动模块
* 文件名称 : bsp_eeprom_24xx.c
* 版    本 : V1.0
* 说    明 : 实现24xx系列EEPROM的读写操作。写操作采用页写模式提高写入效率。
*
* 修改记录 :
* 版本号  日期        作者     说明
* V1.0    2013-02-01 armfly  正式发布
*
* 宇星科技-工业废气在线监测系统
*
*********************************************************************************************************
*/

/*
  应用说明：访问串行EEPROM前，请先调用一次 bsp_InitI2C()函数配置好I2C相关的GPIO.
*/

#include "bsp_eeprom_24xx.h"

/*
*********************************************************************************************************
*  函 数 名: ee_CheckOk
*  功能说明: 判断串行EERPOM是否正常
*  形    参:  无
*  返 回 值: 1 表示正常， 0 表示不正常
*********************************************************************************************************
*/
uint8_t FM_CheckOk(void)
{
  if (i2c_CheckDevice(EE_DEV_ADDR) == 0)
  {
    return 1;
  }
  else
  {
    /* 失败后，切记发送I2C总线停止信号 */
    i2c_Stop();
    return 0;
  }
}


/*
*********************************************************************************************************
*  函 数 名: FMReadBytes
*  功能说明: 从串行EEPROM指定地址处开始读取若干数据
*  形    参:  _usAddress : 起始地址
*            _usSize : 数据长度，单位为字节
*            _pReadBuf : 存放读到的数据的缓冲区指针
*  返 回 值: 0 表示失败，1表示成功
*********************************************************************************************************
*/
uint8_t FMReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
  uint16_t i;

  /* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

  /* 第1步：发起I2C总线启动信号 */
  i2c_Start();

  /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
  i2c_SendByte(EE_DEV_ADDR | I2C_WR);  /* 此处是写指令 */

  /* 第3步：发送ACK */
  if (i2c_WaitAck() != 0)
  {
    goto cmd_fail;  /* EEPROM器件无应答 */
  }

  /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
  if (EE_ADDR_BYTES == 1)
  {
    i2c_SendByte((uint8_t)_usAddress);
    if (i2c_WaitAck() != 0)
    {
      goto cmd_fail;  /* EEPROM器件无应答 */
    }
  }
  else
  {
    i2c_SendByte(_usAddress >> 8);
    if (i2c_WaitAck() != 0)
    {
      goto cmd_fail;  /* EEPROM器件无应答 */
    }

    i2c_SendByte(_usAddress);
    if (i2c_WaitAck() != 0)
    {
      goto cmd_fail;  /* EEPROM器件无应答 */
    }
  }

  /* 第6步：重新启动I2C总线。下面开始读取数据 */
  i2c_Start();

  /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
  i2c_SendByte(EE_DEV_ADDR | I2C_RD);  /* 此处是读指令 */

  /* 第8步：发送ACK */
  if (i2c_WaitAck() != 0)
  {
    goto cmd_fail;  /* EEPROM器件无应答 */
  }

  /* 第9步：循环读取数据 */
  for (i = 0; i < _usSize; i++)
  {
    _pReadBuf[i] = i2c_ReadByte();  /* 读1个字节 */

    /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
    if (i != _usSize - 1)
    {
      i2c_Ack();  /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
    }
    else
    {
      i2c_NAck();  /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
    }
  }
  /* 发送I2C总线停止信号 */
  i2c_Stop();
  return 1;  /* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
  /* 发送I2C总线停止信号 */
  i2c_Stop();
  return 0;
}

/*
*** 入口：			addr-地址
*** 出口：			读出的数据
*** 功能描述：		向固定地址读出数据
*/
uint8_t FM24C64_READ_BYTE (uint16_t addr)
{ 
  uint8_t SlaveADDR,DATA_R;
  SlaveADDR = (uint8_t)(addr>>8);
      SlaveADDR = (uint8_t)(addr>>8);
	  SlaveADDR = (uint8_t)(addr>>8);
          i2c_Start ();
	  i2c_SendByte (EE_DEV_ADDR | I2C_WR);
	  if (i2c_WaitAck() != 0)
          {
            while(1);
          }
      i2c_SendByte(SlaveADDR);
      if (i2c_WaitAck() != 0)
          {
            while(1);
          }
      i2c_SendByte(addr);
      if (i2c_WaitAck() != 0)
          {
            while(1);
          } 
      i2c_Start();
      i2c_SendByte(EE_DEV_ADDR | I2C_RD);  /* 此处是读指令 */
      if (i2c_WaitAck() != 0)
        {
          while(1);
        }
      DATA_R=i2c_ReadByte();
      i2c_NAck();
      i2c_Stop();
      return DATA_R;
}

/*
*** 入口：			StartAddr-起始地址；RdData-数据存放缓冲区；
						Numbers-读出的数据个数
*** 出口：			无
*** 功能描述：		从固定地址读出数据
*/
void FM24C64_READ_MUL(uint16_t StartAddr, uint8_t *RdData, uint16_t Numbers)
{ 
         
      uint8_t SlaveADDR;
	  SlaveADDR=(uint8_t)(StartAddr>>8);
      i2c_Start ();
	 i2c_SendByte (EE_DEV_ADDR | I2C_WR);//片选
         if (i2c_WaitAck() != 0)
          {
            while(1);
          }
      i2c_SendByte (SlaveADDR); 
      if (i2c_WaitAck() != 0)
          {
            while(1);
          }
      i2c_SendByte (StartAddr);  //ROM首地址(0-1FFF)
      if (i2c_WaitAck() != 0)
          {
            while(1);
          }
      i2c_Start ();
      i2c_SendByte (EE_DEV_ADDR | I2C_RD);//读
      if (i2c_WaitAck() != 0)
          {
            while(1);
          }
      while(Numbers>1)
      {   
          *RdData =i2c_ReadByte();
          i2c_Ack();   //应答
          Numbers--;   
          RdData++;
           
      }    
      *RdData =i2c_ReadByte();
      i2c_NAck();  //非应答
      i2c_Stop ();
}

/*
*********************************************************************************************************
*  函 数 名: FM_WriteBytes
*  功能说明: 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
*  形    参:  _pWriteBuf : 存放读到的数据的缓冲区指针
*            _usAddress : 起始地址
*            _usSize : 数据长度，单位为字节
*           
*  返 回 值: 0 表示失败，1表示成功
*********************************************************************************************************
*/
uint8_t FM_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
  uint16_t i,m;
  uint16_t usAddr;

  /*
    写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
    对于24xx02，page size = 8
    简单的处理方法为：按字节写操作模式，每写1个字节，都发送地址
    为了提高连续写的效率: 本函数采用page wirte操作。
  */

  usAddr = _usAddress;
  for (i = 0; i < _usSize; i++)
  {
    /* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
    if ((i == 0) || (usAddr & (EE_PAGE_SIZE - 1)) == 0)
    {
      /*　第０步：发停止信号，启动内部写操作　*/
      i2c_Stop();

      /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
        CLK频率为200KHz时，查询次数为30次左右
      */
      for (m = 0; m < 1000; m++)
      {
        /* 第1步：发起I2C总线启动信号 */
        i2c_Start();

        /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
        i2c_SendByte(EE_DEV_ADDR | I2C_WR);  /* 此处是写指令 */

        /* 第3步：发送一个时钟，判断器件是否正确应答 */
        if (i2c_WaitAck() == 0)
        {
          break;
        }
      }
      if (m  == 1000)
      {
        goto cmd_fail;  /* EEPROM器件写超时 */
      }

      /* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
      if (EE_ADDR_BYTES == 1)
      {
        i2c_SendByte((uint8_t)usAddr);
        if (i2c_WaitAck() != 0)
        {
          goto cmd_fail;  /* EEPROM器件无应答 */
        }
      }
      else
      {
        i2c_SendByte(usAddr >> 8);
        if (i2c_WaitAck() != 0)
        {
          goto cmd_fail;  /* EEPROM器件无应答 */
        }

        i2c_SendByte(usAddr);
        if (i2c_WaitAck() != 0)
        {
          goto cmd_fail;  /* EEPROM器件无应答 */
        }
      }
    }

    /* 第6步：开始写入数据 */
    i2c_SendByte(_pWriteBuf[i]);

    /* 第7步：发送ACK */
    if (i2c_WaitAck() != 0)
    {
      goto cmd_fail;  /* EEPROM器件无应答 */
    }

    usAddr++;  /* 地址增1 */
  }

  /* 命令执行成功，发送I2C总线停止信号 */
  i2c_Stop();
  return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
  /* 发送I2C总线停止信号 */
  i2c_Stop();
  return 0;
}

/*
*** 入口：			addr-地址；data-要写入的数据
*** 出口：			无
*** 功能描述：		向固定地址写入数据
*/
void FM24C64_WRITE_BYTE (uint16_t addr,uint8_t data)
{ 
     uint8_t SlaveADDR;
//        uint8_t i;
	SlaveADDR = (uint8_t)(addr>>8);	
    	i2c_Start ();
	  
	 i2c_SendByte(EE_DEV_ADDR | I2C_WR);
	 
      i2c_SendByte (SlaveADDR); // E2PROM 片选
//if (i2c_WaitAck() != 0)
//    {
//      while(1);
//    }
      i2c_SendByte ((uint8_t)addr);
//if (i2c_WaitAck() != 0)
//    {
//      while(1);
//    }
      i2c_SendByte (data);
//if (i2c_WaitAck() != 0)
//    {
//      while(1);
//    }
      i2c_Stop ();  
      //测试段开始
//      if(data == FM24C64_READ_BYTE(addr))
//        i = 0;
//      else
//        i = 1;
//      i = i;
      //测试段结束
}

/*
*** 入口：			addr-地址；RdData-要写入的数据地址
						Numbers-写入数据的个数
*** 出口：			无
*** 功能描述：		向固定地址写入多个数据
*/
void FM24C64_WRITE_MUL(uint16_t StartAddr, uint8_t *RdData, uint16_t Numbers)
{ 
     
      uint8_t SlaveADDR;
      
      SlaveADDR = (uint8_t)(StartAddr>>8);
      i2c_Start ();
      i2c_SendByte(EE_DEV_ADDR | I2C_WR);// E2PROM 片选
      if (i2c_WaitAck() != 0)
    {
      while(1);
    }
      i2c_SendByte (SlaveADDR); //写高8位地址
      if (i2c_WaitAck() != 0)
    {
     while(1);
    }
      i2c_SendByte (StartAddr); //写底8位地址
      if (i2c_WaitAck() != 0)
    {
      while(1);
    }
      while(Numbers--)
      {
          i2c_SendByte (*RdData);
          RdData++;
      }
      
      i2c_Stop ();
     
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
