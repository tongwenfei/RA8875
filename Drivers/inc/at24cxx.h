#ifndef __AT24CXX_H
#define	__AT24CXX_H

#include "stm32f4xx.h"
#include "sys.h"
#include <string.h>
#include "usart.h"

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */



void AT24CXX_Init(void);
void AT24CXX_Write(u16 WriteAddr,u8* pBuffer, u16 NumByteToWrite);
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 pBuffer);
void AT24CXX_PageWrite(u16 WriteAddr,u8* pBuffer, u8 NumByteToWrite);
void AT24CXX_Read( u16 ReadAddr, u8* pBuffer,u16 NumByteToRead);
u8 AT24CXX_ReadOneByte(u16 WriteAddr);
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);
void AT24CXX_WaitEepromStandbyState(void);
void I2C_Test(void);
#endif /* __I2C_EE_H */
