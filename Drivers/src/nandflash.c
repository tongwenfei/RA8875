

#include "nandflash.h"

#define K9F2G08_delay_times       5000000L

/*定义NAND Flash的物理地址。这个是由硬件决定的*/
#define Bank2_NAND_ADDR           ((WINT32U_F)0x70000000)
#define Bank_NAND_ADDR            Bank2_NAND_ADDR

/* NAND Area definition  for STM3210E-EVAL Board RevD */
#define CMD_AREA                  (WINT32U_F)(1<<16)  /* A16 = CLE  high */
#define ADDR_AREA                 (WINT32U_F)(1<<17)  /* A17 = ALE high */
#define DATA_AREA                 ((WINT32U_F)0x00000000)

#define FLASH_DATA		  (Bank_NAND_ADDR)
#define FLASH_COMMAND             (Bank_NAND_ADDR|CMD_AREA)
#define FLASH_ADDRESS             (Bank_NAND_ADDR|ADDR_AREA)


#define W_Flash(data)             *(volatile unsigned char*)FLASH_DATA=(data);      /* 向flash中写数据*/
#define R_Flash()                 *(volatile unsigned char*)FLASH_DATA;             /* 从flash中读取数据*/
#define W_Flash_Comand(comand)    *(volatile unsigned char*)FLASH_COMMAND=(comand); /* 向flash中写命令*/
#define W_Flash_Address(address)  *(volatile unsigned char*)FLASH_ADDRESS=(address);/* 向flash中写地址*/

#define K9F2G08_IS_READY(uch)           (uch&0x40)!=0
#define K9F2G08_IS_ERROR(uch)           (uch&0x01)!=0
#define K9F2G08_IS_ECC_ERROR(uch)       (uch&0x07)!=4

/*本地函数声明*/
static void FlashFSMCcInit(void);
static WINT8U_F FlshSectorWrite0(WINT16U_F block,WINT16U_F sector,WINT16U_F offset,WINT16U_F size,const WINT8U_F *data_buf);
static WINT8U_F K9F2G08_sector_check(WINT16U_F block,WINT16U_F sector,WINT16U_F offset,WINT16U_F size,const WINT8U_F *data_buf);


/*
********************************************************************************
* 函 数 名: MemSetFun
* 功能说明: 将缓冲区中的数据擦除为指定数值,可以用来擦擦缓冲区;
* 形    参: 无
* 返 回 值: 无
********************************************************************************
*/
//static void MemSetFun(void* buf,WINT8U_F byte,WINT32U_F count)
//{
//  WINT8U_F* pbuf;
//  
//  if((pbuf=(WINT8U_F*)buf)==(WINT8U_F*)0) return;
//  for(WINT32U_F i=0;i<count;i++) pbuf[i]=byte;
//}
/*
********************************************************************************
* 函 数 名: NANDInit
* 功能说明: Flash初始化;
* 形    参: 无
* 返 回 值: 无
********************************************************************************
*/
void NANDFlashInit(void)
{
  FlashFSMCcInit();          /* 配置FSMC和GPIO用于NAND Flash接口 */
}
/*
********************************************************************************
* 函 数 名: FlashFSMCcInit
* 功能说明: FSMS\GPIO初始化;
* 形    参: 无
* 返 回 值: 无
********************************************************************************
*/
static void FlashFSMCcInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  FSMC_NANDInitTypeDef  FSMC_NANDInitStructure;
  FSMC_NAND_PCCARDTimingInitTypeDef  p;

/*
******************************************************************************
  --NAND Flash GPIOs 配置  ------
  
    PD0/FSMC_D2
    PD1/FSMC_D3
    PD4/FSMC_NOE
    PD5/FSMC_NWE
    PD7/FSMC_NCE2
    PD11/FSMC_A16
    PD12/FSMC_A17
    
    PD14/FSMC_D0
    PD15/FSMC_D1
    PE7/FSMC_D4
    PE8/FSMC_D5
    PE9/FSMC_D6
    PE10/FSMC_D7
    PG6/FSMC_INT2  (本例程用查询方式判忙，此口线作为普通GPIO输入功能使用)
******************************************************************************
*/

  /* 使能 GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

  /* 使能 FSMC 时钟 */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  /*  配置GPIOD */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0,  GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1,  GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4,  GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5,  GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7,  GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_7 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /*  配置GPIOE */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /*  配置GPIOG, PG6作为忙信息，配置为输入 */

  /* INT2 引脚配置为内部上来输入，用于忙信号 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* 配置 FSMC 时序 */
  /*
    Defines the number of HCLK cycles to setup address before the command assertion for NAND-Flash
    read or write access  to common/Attribute or I/O memory space (depending on the memory space
    timing to be configured).This parameter can be a value between 0 and 0xFF.
  */
    //p.FSMC_SetupTime = 0x01;
    p.FSMC_SetupTime = 0x1;

  /*
    Defines the minimum number of HCLK cycles to assert the command for NAND-Flash read or write
    access to common/Attribute or I/O memory space (depending on the memory space timing to be
    configured). This parameter can be a number between 0x00 and 0xFF
  */
  //p.FSMC_WaitSetupTime = 0x03;
  p.FSMC_WaitSetupTime = 0x3;

  /*
    Defines the number of HCLK clock cycles to hold address (and data for write access) after the
    command deassertion for NAND-Flash read or write access to common/Attribute or I/O memory space
     (depending on the memory space timing to be configured).
    This parameter can be a number between 0x00 and 0xFF
  */
  //p.FSMC_HoldSetupTime = 0x02;
  p.FSMC_HoldSetupTime = 0x2;

  /*
    Defines the number of HCLK clock cycles during which the databus is kept in HiZ after the start
    of a NAND-Flash  write access to common/Attribute or I/O memory space (depending on the memory
    space timing to be configured). This parameter can be a number between 0x00 and 0xFF
  */
  //p.FSMC_HiZSetupTime = 0x01;
  p.FSMC_HiZSetupTime = 0x1;


  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;                     /* 定义FSMC NAND BANK 号 */
  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;     /* 插入等待时序使能 */
  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;  /* 数据宽度 8bit */
  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;                      /* ECC错误检查和纠正功能使能 */
  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;   /* ECC 页面大小 */
  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x01;                       /* CLE低和RE低之间的延迟，HCLK周期数 */
  FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x01;                        /* ALE低和RE低之间的延迟，HCLK周期数 */
  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;               /* FSMC Common Space Timing */
  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;            /* FSMC Attribute Space Timing */

  FSMC_NANDInit(&FSMC_NANDInitStructure);

  /* FSMC NAND Bank 使能 */
  FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
}
/*
********************************************************************************
* 函 数 名: FlashGetId
* 功能说明: 获取Flash ID值;
* 形    参: blocks_per_dev-存储器总块数;
*           sectors_per_block-每块的扇区数;
*           bytes_per_sector-每个扇区的字节数;
* 返 回 值: 0-成功;
*           1-失败;
********************************************************************************
*/
WINT8U_F FlashGetId(WINT16U_F *blocks_per_dev,WINT16U_F *sectors_per_block,WINT16U_F *bytes_per_sector)
{
  WINT8U_F info[8]={0};
  WINT8U_F i;
  
  W_Flash_Comand(0x90);
  W_Flash_Address(0x00);
  for(i=0;i<5;i++)
  {
    info[i]=R_Flash();
  }
  if (info[0]==0xAD&&info[4]==0xAD)
  {
//    if (blocks_per_dev) *blocks_per_dev=1024;
    if (blocks_per_dev) *blocks_per_dev=512;
    if (sectors_per_block) *sectors_per_block=64;
    if (bytes_per_sector) *bytes_per_sector=2048;
    return(0);/*成功*/
  }
  return(1);/*失败*/
}
/*
********************************************************************************
* 函 数 名: FlashSectorRead
* 功能说明: 读取一个扇区指定字节数;
* 形    参: block-块号;
*           sector-扇区号;
*           offset-扇区偏移;
*           size-读取字节数;
*           data_buf-数据存储缓冲区,起始位置;
* 返 回 值: 0-成功;
*           1-失败;
********************************************************************************
*/
WINT8U_F FlashSecRead(WINT16U_F block,WINT16U_F sector,WINT16U_F offset,WINT16U_F size,WINT8U_F *data_buf)
{
  
  WINT32U_F i;
  WINT8U_F uch;

  W_Flash_Comand(NAND_CMD_AREA_A);
  W_Flash_Address((WINT8U_F)(offset));
  W_Flash_Address((WINT8U_F)(offset>>8));
  
  uch=(WINT8U_F)((block<<6)&0x00FF);
  uch|=(WINT8U_F)(sector&0x003F);
  W_Flash_Address(uch);
  
  uch=(WINT8U_F)((block>>2)&0x00FF);
  W_Flash_Address(uch);
  
  uch=(WINT8U_F)((block>>10)&0x00FF);
  W_Flash_Address(uch);
  W_Flash_Comand(NAND_CMD_AREA_TRUE1);
  
  /*等待执行完成*/
  for (i = 0; i < 20; i++);
  i=500000;
  while((GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0)&&(i--!=0));
  if(i==0) return 1;
  
  /* 读数据到缓冲区pBuffer */
  for(i=0;i<size;i++)
  {
    data_buf[i]=R_Flash();
  }  
  return(0);  
}
/*
********************************************************************************
* 函 数 名: FlshSectorWrite
* 功能说明: block-块号;
*           sector-扇区号;
*           offset-扇区偏移;
*           size-读取字节数;
*           data_buf-待写入数据存储缓冲区,起始位置;
* 形    参: 无
* 返 回 值: 0-成功;
*           1-失败;
********************************************************************************
*/
static WINT8U_F FlshSectorWrite0(WINT16U_F block,WINT16U_F sector,WINT16U_F offset,WINT16U_F size,const WINT8U_F *data_buf)
{
  WINT32U_F tmp=0;
  WINT8U_F uch;

  /*发送命令*/
  W_Flash_Comand(0x80);  

  /*写地址*/
  W_Flash_Address((WINT8U_F)(offset));  
  W_Flash_Address((WINT8U_F)(offset>>8));  
  uch=(WINT8U_F)((block<<6)&0x00FF);
  uch|=(WINT8U_F)(sector&0x003F);
  W_Flash_Address(uch);
  uch=(WINT8U_F)((block>>2)&0x00FF);
  W_Flash_Address(uch);
  uch=(WINT8U_F)((block>>10)&0x00FF);
  W_Flash_Address(uch);

  /*写数据*/
  for (tmp=0;tmp<size;tmp++)
  {
    W_Flash(data_buf[tmp]);
  }

  /*写命令*/
  W_Flash_Comand(0x10);
  
  /*等待执行完成*/
  while(tmp<K9F2G08_delay_times)
  {
    W_Flash_Comand(0x70);
    uch=R_Flash();
    if (K9F2G08_IS_READY(uch)) break;
    tmp++;
  }
  
  /*返回状态*/
  if (tmp>=K9F2G08_delay_times) return(1);/*超时失败：器件一直忙*/
  if (K9F2G08_IS_ERROR(uch)) return(2); /*操作失败*/
  return (0);
  
}
/*
********************************************************************************
* 函 数 名: K9F2G08_sector_check
* 形    参: block-块号;
*           sector-扇区号;
*           offset-扇区偏移;
*           size-读取字节数;
*           data_buf-待写入数据存储缓冲区,起始位置;
* 功能说明: 检查写入的数据是否正确;
* 返 回 值: 0-成功;
*           1-失败;
********************************************************************************
*/
static WINT8U_F K9F2G08_sector_check(WINT16U_F block,WINT16U_F sector,WINT16U_F offset,WINT16U_F size,const WINT8U_F *data_buf)
{
  WINT32U_F tmp=0;
  WINT8U_F uch;
  
  W_Flash_Comand(0x00);
  
  W_Flash_Address((WINT8U_F)(offset));
  
  W_Flash_Address((WINT8U_F)(offset>>8));
  
  uch=(WINT8U_F)((block<<6)&0x00FF);
  uch|=(WINT8U_F)(sector&0x003F);
  W_Flash_Address(uch);
  
  uch=(WINT8U_F)((block>>2)&0x00FF);
  W_Flash_Address(uch);
  
  uch=(WINT8U_F)((block>>10)&0x00FF);
  W_Flash_Address(uch);
  
  W_Flash_Comand(0x30);
  
  /*等待执行完成*/
  while(tmp<K9F2G08_delay_times)
  {
    W_Flash_Comand(0x70);
    uch=R_Flash();
    if (K9F2G08_IS_READY(uch)) break;
    tmp++;
  }
  
  /*返回状态*/
  if (tmp>=K9F2G08_delay_times)
  {
    return(1);/*超时失败：器件一直忙*/
  }
  
  /*返回数据*/
  W_Flash_Comand(0x00);
  for (tmp=0;tmp<size;tmp++)
  {
    uch=R_Flash();
    if (uch!=data_buf[tmp])
    {
      return(2);
    }
  }
  return(0);                            /*操作成功*/
}
/*
********************************************************************************
* 函 数 名: FlashSecWrite
* 形    参: block-块号;
*           sector-扇区号;
*           offset-扇区偏移;
*           size-读取字节数;
*           data_buf-待写入数据存储缓冲区,起始位置;
* 功能说明: 写入数据;
* 返 回 值: 0-成功;
*           1-失败;
********************************************************************************
*/
WINT8U_F FlashSecWrite(WINT16U_F block,WINT16U_F sector,WINT16U_F offset,WINT16U_F size,const WINT8U_F *data_buf)
{
  WINT8U_F err=0;
  
  err=FlshSectorWrite0(block,sector,offset,size,data_buf);/*先写入*/
  if(err) return err;
  err=K9F2G08_sector_check(block,sector,offset,size,data_buf);/*再校验*/
  if(err) return err;
  return 0;
}
/*
********************************************************************************
* 函 数 名: K9F2G08_block_erase
* 功能说明: 擦除一个块;
* 形    参: block-块号
* 返 回 值: 0-成功;
*           1-失败;
********************************************************************************
*/
WINT8U_F FlshBlockErase(WINT16U_F block)
{
  WINT32U_F tmp=0;
  WINT8U_F uch;
  
  
  W_Flash_Comand(0x60);
  
  uch=(WINT8U_F)((block<<6)&0x00FF);
  W_Flash_Address(uch);
  
  uch=(WINT8U_F)((block>>2)&0x00FF);
  W_Flash_Address(uch);
  
  uch=(WINT8U_F)((block>>10)&0x00FF);
  W_Flash_Address(uch);
  
  W_Flash_Comand(0xD0);
  
  /*等待执行完成*/
  while(tmp<K9F2G08_delay_times)
  {
    W_Flash_Comand(0x70);
    uch=R_Flash();
    if (K9F2G08_IS_READY(uch)) break;
    tmp++;
  }
  
  /*返回状态*/
  if (tmp>=K9F2G08_delay_times) return(1);/*超时失败：器件一直忙*/
  if (K9F2G08_IS_ERROR(uch)) return(2); /*操作失败*/
  return(0);/*操作成功*/
}
