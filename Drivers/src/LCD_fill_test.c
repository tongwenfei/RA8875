

#include <stdlib.h>  /* for rand */
#include "bsp.h"
#include "stdio.h"
extern uint32_t OS_TimeMS;
extern FONT_T textfont;
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
uint32_t GetTime(void)
{
  return OS_TimeMS;
}
static const uint16_t _aColor[8] = {
  CL_WHITE      ,
  CL_BLACK     ,
  CL_RED          ,
  CL_GREEN      ,
  CL_BLUE        ,
  CL_YELLOW       ,
  CL_CYAN,
  CL_GREY			,
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetPixelsPerSecond
*/
static uint32_t _GetPixelsPerSecond(void) {
  uint16_t Color, BkColor;
  uint32_t x0, y0, x1, y1, xSize, ySize;
  int32_t t, t0;
  uint32_t Cnt, PixelsPerSecond, PixelCnt;
  
  //
  // Find an area which is not obstructed by any windows
  //
  xSize   = LCD_GetWidth();
  ySize   = LCD_GetHeight();
  Cnt     = 0;
  x0      = 0;
  x1      = xSize - 1;
  y0      = 65;
  y1      = ySize - 60 - 1;
  Color   = RA8875_GetFrontColor();
  BkColor = RA8875_GetBackColor();
  RA8875_SetFrontColor(BkColor);
  //
  // Repeat fill as often as possible in 100 ms
  //
  t0 = GetTime();
  do {
    LCD_Fill_Rect(x0, y0, y1-y0, x1-x0,BkColor);
    Cnt++;
    t = GetTime();    
  } while ((t - (t0 + 100)) <= 0);
  //
  // Compute result
  //
  t -= t0;
  PixelCnt = (x1 - x0 + 1) * (y1 - y0 + 1) * Cnt;
  PixelsPerSecond = PixelCnt / t * 1000;   
  RA8875_SetFrontColor(Color);
  return PixelsPerSecond;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIDEMO_Speed
*/
void LCD_test_Speed(void) {
  int      TimeStart, i;
  uint32_t      PixelsPerSecond;
  int      xSize, ySize;
  uint32_t x0,y0,x1,y1;
  char     cText[40] = { 0 };
  
  xSize  = LCD_GetWidth();
  ySize  = LCD_GetHeight();
  
  
  
  LCD_ClrScr(CL_BLUE);
  textfont.BackColor=CL_BLUE;
  textfont.FrontColor=CL_BLACK;
  textfont.FontCode=FC_ST_16;
    textfont.Space=0;
    LCD_DispStr(100,100,"High speed",&textfont);
     LCD_DispStr(100,120,"Multi layer clipping",&textfont);
      LCD_DispStr(100,140,"Highly optimized drivers",&textfont);

  
  TimeStart = GetTime();
  for (i = 0; ((GetTime() - TimeStart) < 5000); i++)
  {
    
    //
    // Calculate random positions
    //
    x0 = rand() % xSize - xSize / 2;
    y0 = rand() % ySize - ySize / 2;
    x1 =  x0+20 + rand() % xSize;
    y1 =y0+ 20 + rand() % ySize;
    LCD_Fill_Rect(x0, y0, (y1-y0), (x1-x0),_aColor[i&7]);
    //
    // Clip rectangle to visible area and add the number of pixels (for speed computation)
    //
    if (x1 >= xSize) {
      x1 = xSize - 1;
    }
    if (y1 >= ySize) {
      y1 = ySize - 1;
    }
    if (x0 < 0 ) {
      x0 = 0;
    }
    if (y1 < 0) {
      y1 = 0;
    }
    
    //
    // Allow short breaks so we do not use all available CPU time ...
    //
  }
 
  PixelsPerSecond = _GetPixelsPerSecond();
  LCD_ClrScr(CL_BLUE);
  sprintf(cText,"Pixels/sec: %d ",PixelsPerSecond);
   LCD_DispStr(100,100,cText,&textfont);
  
  
  delay_ms(1000);
  delay_ms(1000);
  delay_ms(1000);
  delay_ms(1000);
  
}



/*************************** End of file ****************************/
