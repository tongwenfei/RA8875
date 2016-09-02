/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */


FONT_T textfont;
FATFS fs1[2];
int main(void)
{
	
  
	uint32_t fontcnt;		  
	uint8_t i,j;
	uint8_t fontx[2];//gbk码
	uint8_t key,t;	  


 
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200	
        delay_init(168);	    	 //延时函数初始化
        TIM7_Int_Init(999,83);
        KEY_Init();
	
        //AT24CXX_Init();		//初始化24CXX
       // AT24CXX_Read( 40,data_r, 14); 
        //data_r[0]=AT24CXX_ReadOneByte(53);
        FSMC_SRAM_Init(); 		//SRAM初始化
	LED_Init();
        bsp_InitI2C();
 textfont.BackColor=CL_BLUE;
  textfont.FrontColor=CL_BLACK;
  textfont.FontCode=FC_ST_16;
    textfont.Space=0;
  /*时钟*/
  bsp_InitRTC();

	mem_init(SRAMIN); 		//初始化内部内存池
	mem_init(SRAMEX);  		//初始化外部内存池
	mem_init(SRAMCCM); 		//初始化CCM内存池
        TIM3_Int_Init(999,83);  
	W25QXX_Init();
        LCD_InitHard();
	 TOUCH_InitHard(); 
	 LCD_ClrScr(CL_WHITE);
       
	 
	 exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(&fs1[0],"0:",1); 		//挂载SD卡 
 	f_mount(&fs1[1],"1:",1); 		//挂载FLASH.
		
      
	
	//LCD_DispStr(20,20,"test",&textfont);
      
//	while(font_init()) 			//检查字库
//	  
//	{
//  
//		   	   	  
//		RA8875_DispAscii(30,50,"Explorer STM32F4");
//		while(SD_Init())			//检测SD卡
//		{
//			RA8875_DispAscii(30,70,"SD Card Failed!");
//			delay_ms(200);
////			LCD_Fill(30,70,200+30,70+16,WHITE);
////			delay_ms(200);		    
//		}								 						    
//		RA8875_DispAscii(30,70,"SD Card OK");
//		RA8875_DispAscii(30,90,"Font Updating...");
//		key=update_font(20,110,16,"0:");//更新字库
//		while(key)//更新失败		
//		{			 		  
//			RA8875_DispAscii(30,110,200,16,16,"Font Update Failed!");
//			delay_ms(200);
//			RA8875_FillRect(20,110,200,110,CL_WHITE);
//			delay_ms(200);		       
//		} 		  
//		RA8875_DispAscii(30,110,200,16,16,"Font Update Success!   ");
//		delay_ms(1500);	
//		LCD_ClrScr(CL_WHITE);      
//	}  
//	RA8875_SetFrontColor(CL_RED);     
//	Show_Str(30,50,200,16,"探索者STM32F407开发板",16,0);				    	 
//	Show_Str(30,70,200,16,"GBK字库测试程序",16,0);				    	 
//	Show_Str(30,90,200,16,"正点原子@ALIENTEK",16,0);				    	 
//	Show_Str(30,110,200,16,"2014年5月15日",16,0);
//	Show_Str(30,130,200,16,"按KEY0,更新字库",16,0);
// 	RA8875_SetFrontColor(CL_BLUE); 
//	Show_Str(30,150,200,16,"内码高字节：",16,0);				    	 
//	Show_Str(30,170,200,16,"内码低字节：",16,0);				    	 
//	Show_Str(30,190,200,16,"汉字计数器：",16,0);
//
//	Show_Str(30,220,200,24,"对应汉字为：",24,0); 
//	Show_Str(30,244,200,16,"对应汉字(16*16)为：",16,0);			 
//	Show_Str(30,260,200,12,"对应汉字为：",12,0);			 
//	while(1)
//	{
//		fontcnt=0;
//		for(i=0x81;i<0xff;i++)
//		{		
//			fontx[0]=i;
//			RA8875_DispNum(118,150,i,3,16);		//显示内码高字节    
//			for(j=0x40;j<0xfe;j++)
//			{
//				if(j==0x7f)continue;
//				fontcnt++;
//				RA8875_DispNum(118,170,j);	//显示内码低字节	 
//				RA8875_DispNum(118,190,fontcnt);//汉字计数显示	 
//			 	fontx[1]=j;
//				Show_Font(30+132,220,fontx,24,0);	  
//				Show_Font(30+144,244,fontx,16,0);	  		 		 
//				Show_Font(30+108,260,fontx,12,0);	  		 		 
//				t=200;
//				while(t--)//延时,同时扫描按键
//				{
//					delay_ms(1);
//					
//				}
//				
//			}   
//		}	
//	}
	
	
	
	
	
//        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);
//	WM_SetCreateFlags(WM_CF_MEMDEV);
//
//       
//       
//       printf("Back color is 0x%04X\r\n",RA8875_GetBackColor());
//       printf("Front color is 0x%04X\r\n",RA8875_GetFrontColor());
//	GUI_Init();                /* 初始化 */
//	
//	keypad_demo();
  	while(1) 
	{
      LCD_test_Speed();
        }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
