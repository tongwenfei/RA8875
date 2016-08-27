/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   i2c EEPROM(AT24C02)应用函数bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "at24cxx.h"

/* STM32 I2C 快速模式 */
#define I2C_Speed              100000

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C1_OWN_ADDRESS7      0X0A   
#define EEPROM_ADDRESS        0XA0
/* AT24C01/02每页有8个字节 */
#define I2C_PageSize           64
#define  EEP_Firstpage      0x00
/* AT24C04/08A/16A每页有16个字节 */
//#define I2C_PageSize           16			


u8 I2c_Buf_Write[256];
u8 I2c_Buf_Read[256];
/**
  * @brief  I2C2 I/O配置
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

	/* 使能与 I2C2 有关的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);   
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);  
    
  /* PB6-I2C1_SCL、PB7-I2C1_SDA*/
 GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2);  
 GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_I2C2);
 
 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;   
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    
 GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL;  
 GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
static void I2C_Mode_Configu(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	
	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	
  I2C_InitStructure.I2C_OwnAddress1 =I2C1_OWN_ADDRESS7; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	
	/* I2C的寻址模式 */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	/* 通信速率 */
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  
	/* I2C2 初始化 */
  I2C_Init(I2C2, &I2C_InitStructure);
  
	/* 使能 I2C2 */
  I2C_Cmd(I2C2, ENABLE);   
}

/**
  * @brief  I2C 外设(EEPROM)初始化
  * @param  无
  * @retval 无
  */
void AT24CXX_Init(void)
{
  I2C_GPIO_Config(); 
  I2C_Mode_Configu();
}

/**
  * @brief   将缓冲区中的数据写到I2C EEPROM中
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
void AT24CXX_Write( u16 WriteAddr,u8* pBuffer, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      AT24CXX_PageWrite(WriteAddr,pBuffer , NumOfSingle);
      AT24CXX_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        AT24CXX_PageWrite(WriteAddr,pBuffer, I2C_PageSize); 
    	AT24CXX_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        AT24CXX_PageWrite(WriteAddr,pBuffer , NumOfSingle);
        AT24CXX_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      AT24CXX_PageWrite(WriteAddr,pBuffer , NumOfSingle);
      AT24CXX_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        AT24CXX_PageWrite(WriteAddr,pBuffer , count);
        AT24CXX_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        AT24CXX_PageWrite(WriteAddr,pBuffer , I2C_PageSize);
        AT24CXX_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        AT24CXX_PageWrite(WriteAddr,pBuffer , NumOfSingle); 
        AT24CXX_WaitEepromStandbyState();
      }
    }
  }  
}

/**
  * @brief   写一个字节到I2C EEPROM中
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址 
  * @retval  无
  */
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 pBuffer)
{
  AT24CXX_Write(WriteAddr,&pBuffer,1);
}

/**
  * @brief   在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数
  *          不能超过EEPROM页的大小，AT24C02每页有8个字节
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
void AT24CXX_PageWrite(u16 WriteAddr,u8* pBuffer, u8 NumByteToWrite)
{
   u8 temp_addr[2]={(uint8_t)(WriteAddr>>8),(uint8_t)WriteAddr};
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008
    
  /* Send START condition */
  I2C_GenerateSTART(I2C2, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(I2C2, temp_addr[0]);  

  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   I2C_SendData(I2C2, temp_addr[1]);  

  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(I2C2, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C2, ENABLE);
}

/**
  * @brief   从EEPROM里面读取一块数据 
  * @param   
  *		@arg pBuffer:存放从EEPROM读取的数据的缓冲区指针
  *		@arg WriteAddr:接收数据的EEPROM的地址
  *     @arg NumByteToWrite:要从EEPROM读取的字节数
  * @retval  无
  */
void AT24CXX_Read(u16 ReadAddr,u8* pBuffer , u16 NumByteToRead)
{  
   u8 temp_addr[2]={(uint8_t)(ReadAddr>>8),(uint8_t)ReadAddr};
  //*((u8 *)0x4001080c) |=0x80; 
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008    
    
  /* Send START condition */
  I2C_GenerateSTART(I2C2, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C2, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C2, temp_addr[0]);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   I2C_SendData(I2C2, temp_addr[1]);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C2, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* While there is data to be read */
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C2, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C2, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2C2);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C2, ENABLE);
}

/**
  * @brief  Wait for EEPROM Standby state 
  * @param  无
  * @retval 无
  */
void AT24CXX_WaitEepromStandbyState(void)      
{
  vu16 SR1_Tmp = 0;

  do
  {
    /* Send START condition */
    I2C_GenerateSTART(I2C2, ENABLE);
    /* Read I2C2 SR1 register */
    SR1_Tmp = I2C_ReadRegister(I2C2, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  }while(!(I2C_ReadRegister(I2C2, I2C_Register_SR1) & 0x0002));
  
  /* Clear AF flag */
  I2C_ClearFlag(I2C2, I2C_FLAG_AF);
    /* STOP condition */    
    I2C_GenerateSTOP(I2C2, ENABLE); 
}

u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
   u8 temp_addr[2]={(uint8_t)(ReadAddr>>8),(uint8_t)ReadAddr},pBuffer;
  //*((u8 *)0x4001080c) |=0x80; 
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Added by Najoua 27/08/2008    
    
  /* Send START condition */
  I2C_GenerateSTART(I2C2, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C2, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C2, temp_addr[0]);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   I2C_SendData(I2C2, temp_addr[1]);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C2, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C2, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* While there is data to be read */
 
I2C_AcknowledgeConfig(I2C2, DISABLE);
 I2C_GenerateSTOP(I2C2, ENABLE);
    /* Test on EV7 and clear it */
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
    {
    }
      
      /* Read a byte from the EEPROM */
      pBuffer = I2C_ReceiveData(I2C2);
   

   
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C2, ENABLE);
  return pBuffer;
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
 u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}		
	
}
//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 data_temp[4];
	u32 temp=0;
if(Len==2)
  {
     AT24CXX_Read(ReadAddr,data_temp,2);
     temp=data_temp[0]|(data_temp[1]<<8);
  }
else if(Len==4)
{
AT24CXX_Read(ReadAddr,data_temp,4);
temp=data_temp[0]|(data_temp[1]<<8)|(data_temp[2]<<16)|(data_temp[3]<<24);
}
       
	return temp;												    
}
void I2C_Test(void)
{
	u16 i;

	printf("写入的数据\n\r");
    
	for ( i=0; i<=255; i++ ) //填充缓冲
  {   
    I2c_Buf_Write[i] = i;

    printf("0x%02X ", I2c_Buf_Write[i]);
    if(i%16 == 15)    
        printf("\n\r");    
   }

  //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
   //LED1(ON);
	AT24CXX_Write( EEP_Firstpage,I2c_Buf_Write, 256);
	//LED1(OFF);   
  
  printf("\n\r写成功\n\r");
   
   printf("\n\r读出的数据\n\r");
  //将EEPROM读出数据顺序保持到I2c_Buf_Read中
    //LED2(ON);   
	AT24CXX_Read( EEP_Firstpage,I2c_Buf_Read, 256); 
   //LED2(OFF);
   
  //将I2c_Buf_Read中的数据通过串口打印
	for (i=0; i<256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			printf("错误:I2C EEPROM写入与读出的数据不一致\n\r");
			return;
		}
    printf("0x%02X ", I2c_Buf_Read[i]);
    if(i%16 == 15)    
        printf("\n\r");
    
	}
  printf("I2C(AT24C02)读写测试成功\n\r");
}








/*********************************************END OF FILE**********************/
