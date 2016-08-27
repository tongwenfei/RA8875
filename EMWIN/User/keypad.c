#include <stddef.h>
#include "DIALOG.h"
#include "keypad.h"
#include "string.h"
#include "stdlib.h"

#include "WM.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F4/F7开发板
//STemWin 软键盘
//此键盘实现了:英文(大小写)，数字，符号，中文混合输入
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/4/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

WM_HWIN hMulti=0;       //多行文本
KEYPADStructTypeDef keypad_dev;		

static int _DrawSkinFlex_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo); 

	
typedef struct 
{
	int          xPos;              //按钮X位置
	int          yPos;              //按钮Y位置
	int          xSize;             //按钮宽度
	int          ySize;             //按钮高度
	const char * acLabel;           //按钮所代表的字符
	void (* pfDraw)(WM_HWIN hWin);  //按钮的绘制函数
    const char * ButString;         //按钮上显示的字符
} BUTTON_DATA;


//在按钮上绘制位图，比如回车键，删除键等
static void _DrawCentered(WM_HWIN hWin, const GUI_BITMAP * pBM) 
{
	int xSizeWin;
	int ySizeWin;
	int xSizeBMP;
	int ySizeBMP;
	int xPos;
	int yPos;

	xSizeWin = WM_GetWindowSizeX(hWin);
	ySizeWin = WM_GetWindowSizeY(hWin);
	xSizeBMP = pBM->XSize;
	ySizeBMP = pBM->YSize;
	xPos = (xSizeWin - xSizeBMP) >> 1;
	yPos = (ySizeWin - ySizeBMP) >> 1;
	GUI_DrawBitmap(pBM, xPos, yPos);
}

static void _DrawBkSpc (WM_HWIN hWin) {  _DrawCentered(hWin, &bmBackSpace); }	//绘制退格键(删除键)
static void _DrawEnter (WM_HWIN hWin) {  _DrawCentered(hWin, &bmEnter); }		//绘制回车键
static void _DrawShift (WM_HWIN hWin) {  _DrawCentered(hWin, &bmShift); }		//绘制shift键
static void _DrawRetern(WM_HWIN hWin) {  _DrawCentered(hWin, &bmReturn); }		//绘制返回键


//英文键盘位置
static  BUTTON_DATA _aEngButtonData[] = {
	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "Q",0,"Q" },
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "W",0,"W" },
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "E",0,"E" },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "R",0,"R" },
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "T",0,"T" },
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "Y",0,"Y" },
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "U",0,"U" },
	{ (ENG_BUTTON_XPAC*8+ENG_BUTTON_WIDTH*7+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "I",0,"I" },
	{ (ENG_BUTTON_XPAC*9+ENG_BUTTON_WIDTH*8+ENG_BUTTONLINE1_XOFFSET),  ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "O",0,"O" },
	{ (ENG_BUTTON_XPAC*10+ENG_BUTTON_WIDTH*9+ENG_BUTTONLINE1_XOFFSET), ENG_BUTTON_LINE1YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "P",0,"P" },

	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "A",0,"A" },
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "S",0,"S" },
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "D",0,"D" },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "F",0,"F" },
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "G",0,"G" },
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "H",0,"H" },
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "J",0,"J" },
	{ (ENG_BUTTON_XPAC*8+ENG_BUTTON_WIDTH*7+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "K",0,"K" },
	{ (ENG_BUTTON_XPAC*9+ENG_BUTTON_WIDTH*8+ENG_BUTTONLINE2_XOFFSET),  ENG_BUTTON_LINE2YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "L",0,"L" },

	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE3_XOFFSET),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONSHIFT_XINC,ENG_BUTTON_HEIGHT, "\x19", _DrawShift },	//shift的键值为0X19
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "Z",0,"Z" },
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "X",0,"X" },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "C",0,"C" },
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "V",0,"V" },
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "B",0,"B" },
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "N",0,"N" },
	{ (ENG_BUTTON_XPAC*8+ENG_BUTTON_WIDTH*7+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "M",0,"M" },
	{ (ENG_BUTTON_XPAC*9+ENG_BUTTON_WIDTH*8+ENG_BUTTONLINE3_XOFFSET+ENG_BUTTONSHIFT_XINC),  ENG_BUTTON_LINE3YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONENTER_XINC,ENG_BUTTON_HEIGHT, "\x08",_DrawBkSpc},
	
	{ (ENG_BUTTON_XPAC*1+ENG_BUTTON_WIDTH*0+ENG_BUTTONLINE4_XOFFSET),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONFIGURE_XINC,ENG_BUTTON_HEIGHT, "\x02",0,"数"},						    //切换数组键盘				
	{ (ENG_BUTTON_XPAC*2+ENG_BUTTON_WIDTH*1+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONSYMBOL_XINC,ENG_BUTTON_HEIGHT,  "\x03",0,"符"},	//切换符号键盘
	{ (ENG_BUTTON_XPAC*3+ENG_BUTTON_WIDTH*2+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT,  ",",0,"," },
	{ (ENG_BUTTON_XPAC*4+ENG_BUTTON_WIDTH*3+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONSPACE_XINC,ENG_BUTTON_HEIGHT,  " ",0,"└┘"},
	{ (ENG_BUTTON_XPAC*5+ENG_BUTTON_WIDTH*4+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC+ENG_BUTTONSPACE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH,ENG_BUTTON_HEIGHT, "." ,0,"."},
	{ (ENG_BUTTON_XPAC*6+ENG_BUTTON_WIDTH*5+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC+ENG_BUTTONSPACE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONCE_XINC,ENG_BUTTON_HEIGHT, "\x0B",0,"中"},	//切换到中文键盘
	{ (ENG_BUTTON_XPAC*7+ENG_BUTTON_WIDTH*6+ENG_BUTTONLINE4_XOFFSET+ENG_BUTTONFIGURE_XINC+ENG_BUTTONSYMBOL_XINC+ENG_BUTTONSPACE_XINC+ENG_BUTTONCE_XINC),  ENG_BUTTON_LINE4YPOS, ENG_BUTTON_WIDTH+ENG_BUTTONBACK_XINC,ENG_BUTTON_HEIGHT, "\x0d",_DrawEnter},
};

//数字键盘位置
static  BUTTON_DATA _aNumButtonData[] = {
	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE1YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "+",0,"+" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "1" ,0,"1"},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "2" ,0,"2"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "3" ,0,"3"},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE1YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONBACKSPACE_XINC),NUM_BUTTON_HEIGHT, "\x08",_DrawBkSpc},	//删除键
  
	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE2YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "-",0,"-" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "4" ,0,"4"},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "5" ,0,"5"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "6" ,0,"6"},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE2YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONSPACE_XINC),NUM_BUTTON_HEIGHT,  " ",0,"└┘" },	//空格键
  
	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE3YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "/",0,"/" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "7" ,0,"7"},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "8" ,0,"8"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "9" ,0,"9"},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE3YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONENTER_XINC),NUM_BUTTON_HEIGHT, "\x0d",_DrawEnter},	//回车键
  
	{ (NUM_BUTTON_XPAC*1+NUM_BUTTON_WIDTH*0+NUM_BUTTON_XOFFSET),  NUM_BUTTON_LINE4YPOS,(NUM_BUTTON_WIDTH-NUM_BUTTONSING_XINC),NUM_BUTTON_HEIGHT, "@",0,"@" },
	{ (NUM_BUTTON_XPAC*2+NUM_BUTTON_WIDTH*1+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "," ,0,","},
	{ (NUM_BUTTON_XPAC*3+NUM_BUTTON_WIDTH*2+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "0" ,0,"0"},
	{ (NUM_BUTTON_XPAC*4+NUM_BUTTON_WIDTH*3+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, NUM_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "." ,0,"."},
	{ (NUM_BUTTON_XPAC*5+NUM_BUTTON_WIDTH*4+NUM_BUTTON_XOFFSET-NUM_BUTTONSING_XINC),  NUM_BUTTON_LINE4YPOS, (NUM_BUTTON_WIDTH-NUM_BUTTONRETURN_XINC),NUM_BUTTON_HEIGHT, "\x04",_DrawRetern },//返回键
};

//符号键盘参数
static  BUTTON_DATA _aSinButtonData[][20] = {
	{
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "~" ,0,"~"},
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "-" ,0,"-"},
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ":" ,0,":"},
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "_" ,0,"_"},
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x08",_DrawBkSpc},	//删除键
  
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "\"",0,"\"" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "," ,0,","},
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "<" ,0,"<"},
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ">" ,0,">"},
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT,  "\x05",0,"∧"},	//上一页
  
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "*",0,"*" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "+",0,"+" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, ".",0,"." },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "#",0,"#" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x06",0,"∨"},	//下一页
  
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "%",0,"%"},
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "@",0,"@" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "!",0,"!" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "/",0,"/" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x04",_DrawRetern },//返回键
	},
	
	{
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "?",0,"?" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ";",0,";" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "`",0,"`" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "$",0,"$" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE1YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x08",_DrawBkSpc},	//删除键
  
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "^" ,0,"^"},
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "\\",0,"\\" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, "(" ,0,"("},
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, SING_BUTTON_WIDTH,SIGN_BUTTON_HEIGHT, ")" ,0,")"},
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE2YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT,  "\x05",0,"∧" },	//上一页
  
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "&",0,"&" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "=",0,"=" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "'",0,"'" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "|",0,"|" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE3YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x06",0,"∨"},	//下一页
  
		{ (SIGN_BUTTON_XPAC*1+SING_BUTTON_WIDTH*0+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "[",0,"[" },
		{ (SIGN_BUTTON_XPAC*2+SING_BUTTON_WIDTH*1+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "]",0,"]" },
		{ (SIGN_BUTTON_XPAC*3+SING_BUTTON_WIDTH*2+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "{",0,"{" },
		{ (SIGN_BUTTON_XPAC*4+SING_BUTTON_WIDTH*3+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, SING_BUTTON_WIDTH,NUM_BUTTON_HEIGHT, "}",0,"}" },
		{ (SIGN_BUTTON_XPAC*5+SING_BUTTON_WIDTH*4+SIGN_BUTTON_XOFFSET),  SIGN_BUTTON_LINE4YPOS, (SING_BUTTON_WIDTH-SIGN_BUTTONCONTR_XINC),SIGN_BUTTON_HEIGHT, "\x04",_DrawRetern },//返回键
	},
};



//大小写切换处理
void changecase(u8 change_flag,BUTTON_DATA *buttondata)
{
	u16 i;
	char	   	NumToStr;
	char buffer[5];
	if(keypad_dev.LowerCase)		//小写,按钮修改为小写
	{
		//更改按钮的字符
		for (i = 0; i < 27; i++) 	
		{
			if(i==19) continue;
			NumToStr=(char)((int)*(buttondata[i].acLabel)+0x20);  
            sprintf(buffer,"%c",NumToStr);              //转换为字符
			BUTTON_SetFont(keypad_dev.EngButton_Handle[i],&GUI_Font24_ASCII); 
			BUTTON_SetText(keypad_dev.EngButton_Handle[i],buffer);
			BUTTON_SetFocussable(keypad_dev.EngButton_Handle[i], 0);
        }
    }
	else 	//大写，按钮上的字符改为大写
	{
		//更改按钮的字符
		for (i = 0; i < 27; i++) 	
		{
			if(i==19) continue;
			BUTTON_SetFont(keypad_dev.EngButton_Handle[i],&GUI_Font24_ASCII);
			BUTTON_SetText(keypad_dev.EngButton_Handle[i], buttondata[i].acLabel);
			BUTTON_SetFocussable(keypad_dev.EngButton_Handle[i], 0);
		}
	}
}

//绘制英文键盘
//hWin:父窗口
void drawenglish_pad(WM_HWIN hWin)
{
	u16 	i;
	
	//创建键盘按钮
	BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); //设置BUTTON的皮肤	
	for (i = 0; i < GUI_COUNTOF(_aEngButtonData); i++) 
	{
		//创建按钮
		keypad_dev.EngButton_Handle[i] = BUTTON_CreateEx(_aEngButtonData[i].xPos, _aEngButtonData[i].yPos, _aEngButtonData[i].xSize, _aEngButtonData[i].ySize, 
                             hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
		BUTTON_SetFont(keypad_dev.EngButton_Handle[i],&GUI_Font24_ASCII);
        BUTTON_SetText(keypad_dev.EngButton_Handle[i], _aEngButtonData[i].ButString);
		BUTTON_SetFocussable(keypad_dev.EngButton_Handle[i], 0);
	}
	changecase(keypad_dev.LowerCase,_aEngButtonData);
}

//绘制数字键盘
void drawnumber_pad(WM_HWIN hWin)
{
	u16 i;
	for(i=0;i<GUI_COUNTOF(_aNumButtonData);i++)								//创建数字键盘
	{
		//创建按钮
		BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); //设置BUTTON的皮肤
		keypad_dev.NumButton_Handle[i] = BUTTON_CreateEx(_aNumButtonData[i].xPos, _aNumButtonData[i].yPos, _aNumButtonData[i].xSize, _aNumButtonData[i].ySize, 
                             hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
		BUTTON_SetFont(keypad_dev.NumButton_Handle[i],&GUI_Font24_ASCII);
		BUTTON_SetText(keypad_dev.NumButton_Handle[i], _aNumButtonData[i].ButString);
		BUTTON_SetFocussable(keypad_dev.NumButton_Handle[i], 0);
	}
}

//绘制符号键盘
void drawsign_pad(WM_HWIN hWin)
{
	u16 i;
	for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++)					//创建符号键盘
	{
		//创建按钮
      	BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); //设置BUTTON的皮肤
		keypad_dev.SignButton_Handle[i] = BUTTON_CreateEx(_aSinButtonData[keypad_dev.signpad_flag][i].xPos, _aSinButtonData[keypad_dev.signpad_flag][i].yPos,
											_aSinButtonData[keypad_dev.signpad_flag][i].xSize,_aSinButtonData[keypad_dev.signpad_flag][i].ySize,
											hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
		BUTTON_SetFont(keypad_dev.SignButton_Handle[i],&GUI_Font24_ASCII);
		BUTTON_SetText(keypad_dev.SignButton_Handle[i],_aSinButtonData[keypad_dev.signpad_flag][i].ButString);
		BUTTON_SetFocussable(keypad_dev.SignButton_Handle[i], 0);
	}
}



//英文键盘处理函数
//buttondata:英文键盘按钮参数
//Id:按钮ID
//*pMsg:指向消息
void engkeypad_process(BUTTON_DATA *buttondata,int Id,WM_MESSAGE *pMsg)
{
	char 		c;
	WM_MESSAGE 	Msg;
    u8          KeyID;

    KeyID=Id-ID_BUTTON;
	if (buttondata[Id - ID_BUTTON].acLabel) 
	{
		c = buttondata[Id - ID_BUTTON].acLabel[0];
        if(KeyID == 19)                             //大小写切换
        {
            keypad_dev.LowerCase ^= 1;		        //对LowerCase进行异或操作
			changecase(keypad_dev.LowerCase,buttondata);                
        }
        else  if(KeyID == 28)                       //切换到数字键盘
        {
            keypad_dev.padtype=NUMBER_KEYPAD;		//数字键盘
			Msg.MsgId=CHANGE_ENGLISHPADTO_NUMPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }    
        else if(KeyID == 29)                        //切换到符号键盘
        {
            keypad_dev.padtype=SIGN_KEYPAD;		
			Msg.MsgId=CHANGE_ENGLISHPADTO_SIGNPAD1;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);
        }
        
        else
        {
            if (keypad_dev.LowerCase && (c>=0X41) && (c<=0X5A)) //小写
			{
				c += 0x20;
			}
			GUI_StoreKeyMsg(c, 1);	//把消息存进键盘缓冲器，按下状态
			GUI_StoreKeyMsg(c, 0);	//把消息存进键盘缓冲器，松开状态
        }
	}
}

//数字键盘处理函数
//buttondata:数字键盘按钮参数
//Id:按钮ID
//*pMsg:指向消息
void numkeypad_process(BUTTON_DATA *buttondata,int Id,WM_MESSAGE *pMsg)	
{
	char 		c;
	WM_MESSAGE 	Msg;
    u8          KeyID;
    
    KeyID=Id-ID_BUTTON;
	if (buttondata[Id - ID_BUTTON].acLabel) 
	{
		c = buttondata[Id - ID_BUTTON].acLabel[0];

        if(KeyID == 19)                             //返回英文键盘
        {
            keypad_dev.padtype=ENGLISH_KEYPAD;		//英文键盘
			Msg.MsgId=CHANGE_NUMPADTO_ENGLISHPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg); 
        }
        else
        {
            GUI_StoreKeyMsg(c, 1);				//把消息存进键盘缓冲器，按下状态
			GUI_StoreKeyMsg(c, 0);				//把消息存进键盘缓冲器，松开状态 
        }
	}
}
//符号键盘处理函数
//buttondata:数字键盘按钮参数
//Id:按钮ID
//*pMsg:指向消息
void signkeypad_process(BUTTON_DATA *buttondata,int Id,WM_MESSAGE *pMsg)	
{
	char 		c;
	WM_MESSAGE 	Msg;
    u8          KeyID;
    
    KeyID=Id-ID_BUTTON;
	if (buttondata[Id - ID_BUTTON].acLabel) 
	{
		c = buttondata[Id - ID_BUTTON].acLabel[0];
        if(KeyID == 19)                             //返回到英文键盘
        {
            keypad_dev.padtype=ENGLISH_KEYPAD;		//英文键盘
			Msg.MsgId=CHANGE_SIGNPADTO_ENGLISHPAD;
			Msg.hWinSrc=pMsg->hWin;
			WM_SendMessage(keypad_dev.hKeypad,&Msg);  
        }
        else if(KeyID == 14)                        //下一个符号键盘
        {
           
        } 
        else if(KeyID == 9)                         //上一个符号键盘
        {
            
        }
        else
        {
            GUI_StoreKeyMsg(c, 1);				//把消息存进键盘缓冲器，按下状态
			GUI_StoreKeyMsg(c, 0);				//把消息存进键盘缓冲器，松开状态 
        }
	}
}





//设置BUTTON要使用的皮肤
static int _DrawSkinFlex_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) 
{
	static GUI_MEMDEV_Handle ahMemEdges[4];
	GUI_MEMDEV_Handle        hMem;
	WM_HWIN                  hWin;
	unsigned                 i;
	int                      xPos;
	int                      yPos;
	int                      xSize;
	int                      ySize;
	int                      x;
	int                      y;
	int                      Id;
	int                      IsPressed;
    int                      butheigh;
	void (* pfDraw)(WM_HWIN hWin);

	x = 0;
	y = 0;
	switch (pDrawItemInfo->Cmd) 
	{
		case WIDGET_ITEM_CREATE:
			if (ahMemEdges[0] == 0) 
			{
				//创建一个小的帮助窗口，但是不可见
				hWin = WM_CreateWindowAsChild(0, 0, 16, 16, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, NULL, 0);
				WM_SelectWindow(hWin);
				//为button的每个边缘创建存储设备
				for (i = 0; i < GUI_COUNTOF(ahMemEdges); i++) 
				{
					switch (i) 
					{
						case 0:
							x = 0;
							y = 0;
							break;
						case 1:
							x = -16;
							y = 0;
							break;
						case 2:
							x = -16;
							y = -16;
							break;
						case 3:
							x = 0;
							y = -16;
							break;
					}
					//创建放大设备
					hMem = GUI_MEMDEV_CreateFixed(0, 0, 16, 16, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
					GUI_MEMDEV_Select(hMem);
					GUI_SetBkColor(GUI_TRANSPARENT);
					GUI_Clear();
					GUI_SetColor(COLOR_BORDER);
					GUI_DrawRoundedFrame(x, y, x + 31, y + 31, 16, 4);	//绘制换肤后的按钮四角的圆弧

					//收缩到指定的大小
					ahMemEdges[i] = GUI_MEMDEV_CreateFixed(0, 0, 4, 4, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
					GUI_MEMDEV_Select(ahMemEdges[i]);
					GUI_SetBkColor(GUI_TRANSPARENT);
					GUI_Clear();
					GUI_MEMDEV_RotateHQ(hMem, ahMemEdges[i], -6, -6, 0, 250);
					GUI_MEMDEV_Delete(hMem);
					GUI_MEMDEV_Select(0);
				}
				WM_DeleteWindow(hWin);
			}
			break;
		case WIDGET_ITEM_DRAW_TEXT:
			//在按钮上显示自定义或者默认的颜色或者文本
			hWin = pDrawItemInfo->hWin;
			Id   = WM_GetId(hWin);
			i    = Id - ID_BUTTON;
			if(keypad_dev.padtype==ENGLISH_KEYPAD) pfDraw = _aEngButtonData[i].pfDraw;
			else if(keypad_dev.padtype==NUMBER_KEYPAD) pfDraw = _aNumButtonData[i].pfDraw;
			else if(keypad_dev.padtype==SIGN_KEYPAD) pfDraw = _aSinButtonData[keypad_dev.signpad_flag][i].pfDraw;
           
			if (pfDraw) pfDraw(hWin);
			else BUTTON_DrawSkinFlex(pDrawItemInfo);
			break;
		case WIDGET_ITEM_DRAW_BACKGROUND:
			//绘制按钮的背景
			IsPressed = BUTTON_IsPressed(pDrawItemInfo->hWin); 	//判断是否按钮是否被按下
			xPos      = WM_GetWindowOrgX(pDrawItemInfo->hWin);	//获取按钮小工具的X方向原点位置
			yPos      = WM_GetWindowOrgY(pDrawItemInfo->hWin);	//获取按钮小工具的Y方向原点位置
			xSize     = WM_GetWindowSizeX(pDrawItemInfo->hWin);	//获取按钮小工具的X大小
			ySize     = WM_GetWindowSizeY(pDrawItemInfo->hWin);	//获取按钮小工具的Y大小
			if (IsPressed) 										//如果按钮被按下
			{
				GUI_DrawGradientRoundedV(0, 0, xSize - 1, ySize - 1, 4, BUTTON_COLOR2, BUTTON_COLOR3);//绘制垂直颜色梯度填充的圆角矩形
			} 
			else 
			{
				GUI_DrawGradientRoundedV(0, 0, xSize - 1, ySize - 1, 4, BUTTON_COLOR0, BUTTON_COLOR1);//绘制垂直颜色梯度填充的圆角矩形
			}
			GUI_SetColor(COLOR_BORDER);
			//绘制按钮的四条边界
			GUI_DrawHLine(        0, 4, xSize - 5);
			GUI_DrawHLine(ySize - 1, 4, xSize - 5);
			GUI_DrawVLine(        0, 4, ySize - 5);
			GUI_DrawVLine(xSize - 1, 4, ySize - 5);
			//将给定存储设备的内容写入当前选定存储设备的制定位置	
         
             butheigh = 68;
			GUI_MEMDEV_WriteAt(ahMemEdges[0], xPos +  0, yPos +  0);			    //按钮左上角圆弧
			GUI_MEMDEV_WriteAt(ahMemEdges[1], xPos + xSize - 4, yPos +  0);		    //按钮右上角圆弧
			GUI_MEMDEV_WriteAt(ahMemEdges[2], xPos + xSize - 4, yPos + butheigh);   //按钮右下角圆弧
			GUI_MEMDEV_WriteAt(ahMemEdges[3], xPos +  0, yPos + butheigh);			//按钮左下角圆弧
			break;
		default:
			return BUTTON_DrawSkinFlex(pDrawItemInfo);//其他命令就使用默认的皮肤
	}
	return 0;
}



//更新横向的6个按钮
void keypad_hbtn_updc(u8* str)
{
    vu8 i=0;
    if((keypad_dev.dispagecur>=keypad_dev.dispagenum)&&keypad_dev.dispagenum)return;  //当前页等于或者大于总页数
    for(i=0;i<6;i++) keypad_dev.butname[i]=NULL;
    i=0;
    for(i=0;i<6;i++)
    {
        if((i+6*(keypad_dev.dispagecur))>(keypad_dev.hznum-1)) //超出相应拼音的汉字字数
        {
            keypad_dev.disbuf[i][0]=' ';
            keypad_dev.disbuf[i][1]=' ';
            keypad_dev.disbuf[i][2]='\0';
            keypad_dev.butname[i]=keypad_dev.disbuf[i];
        }else
        {
            keypad_dev.disbuf[i][0]=*str++;
            keypad_dev.disbuf[i][1]=*str++;
            keypad_dev.disbuf[i][2]='\0';
            keypad_dev.butname[i]=keypad_dev.disbuf[i];
        }
    }
}

//绘制横向的6个按钮
void keypad_draw_hbtn(WM_HWIN *handle)
{
    u8 i;
    for(i=0;i<6;i++)
    {
        BUTTON_SetFont(handle[13+i],&GUI_Font24_ASCII);
        BUTTON_SetText(handle[13+i],(char*)keypad_dev.butname[i]);
    }

}
//keypad窗口调用的回调函数
static void _cbKeyPad(WM_MESSAGE * pMsg) 
{
	WM_HWIN    hWin;
	int        Id;
	int        NCode;
	int        xSize;
	int        ySize;
    u16        slen;
	unsigned   i;

	hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:				//此消息在窗口创建后立即发送，是窗口可以初始化并创建任何子窗口
			drawenglish_pad(hWin);	//创建英文键盘
			break;
		case WM_PAINT:
			//绘制背景
			xSize = WM_GetWindowSizeX(hWin);
			ySize = WM_GetWindowSizeY(hWin);
			GUI_SetColor(COLOR_BORDER);
			GUI_DrawRect(0, 0, xSize - 1, ySize - 1);
			GUI_DrawGradientV(1, 1, xSize - 2, ySize - 2, COLOR_KEYPAD0, COLOR_KEYPAD1);
			break;
		case WM_NOTIFY_PARENT:	//告诉父窗口，子窗口发生了改变
			//想当前焦点窗口发送按键消息
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED:				//按钮被释放
					if(keypad_dev.padtype==ENGLISH_KEYPAD)	//英文键盘
					{
						engkeypad_process(_aEngButtonData,Id,pMsg);		//处理英文键盘
					}
					else if(keypad_dev.padtype==NUMBER_KEYPAD)			//数字键盘
					{
						numkeypad_process(_aNumButtonData,Id,pMsg);		//处理数字键盘
					}
					else if(keypad_dev.padtype==SIGN_KEYPAD)			
					{
						signkeypad_process(_aSinButtonData[keypad_dev.signpad_flag],Id,pMsg);
					}
                  
					break;
			}
			break;	
		case CHANGE_ENGLISHPADTO_NUMPAD:			//从英文键盘切换到数字键盘
			for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) WM_DeleteWindow(keypad_dev.EngButton_Handle[i]);	
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) keypad_dev.EngButton_Handle[i]=0;
			drawnumber_pad(hWin);
			break;
		case CHANGE_NUMPADTO_ENGLISHPAD:			//从数字键盘切换到英文键盘
			for(i=0;i<GUI_COUNTOF(_aNumButtonData);i++) WM_DeleteWindow(keypad_dev.NumButton_Handle[i]);	
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) keypad_dev.NumButton_Handle[i]=0;
			drawenglish_pad(hWin);					
			break;
		case CHANGE_ENGLISHPADTO_SIGNPAD1:			//从英文键盘切换到符号键盘
			for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) WM_DeleteWindow(keypad_dev.EngButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aEngButtonData);i++) keypad_dev.SignButton_Handle[i]=0;
		    drawsign_pad(hWin);
			break;
		case CHANGE_SIGNPADTO_ENGLISHPAD:			//从符号键盘切换到英文键盘
			for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++) WM_DeleteWindow(keypad_dev.SignButton_Handle[i]);
            for(i=0;i<GUI_COUNTOF(_aSinButtonData[keypad_dev.signpad_flag]);i++) keypad_dev.SignButton_Handle[i]=0;
			drawenglish_pad(hWin);
			break;
		
	}
}


//背景窗口回调函数
static void _cbBk(WM_MESSAGE * pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_PAINT:
			GUI_DrawGradientV(0, 0,800,480, COLOR_BACK0, COLOR_BACK1);
			break;
	}
}


void keypad_demo(void) 
{
	WM_HWIN hFrame;
	
	WM_SetCallback(WM_HBKWIN, _cbBk);		        //是指背景窗口回调函数
	
	keypad_dev.xpos=0;
	keypad_dev.ypos=160;
	keypad_dev.width=800;
	keypad_dev.height=480;
	keypad_dev.padtype=ENGLISH_KEYPAD;				//默认为英文键盘
	keypad_dev.signpad_flag=0;	
	keypad_dev.signpad_num=2;
    keypad_dev.inputlen=0;
    keypad_dev.pynowpage=0;
    keypad_dev.cur_index=0;
    keypad_dev.sta=0;
	
	//设置keypad所使用的按钮的皮肤
	BUTTON_SetDefaultSkin(_DrawSkinFlex_BUTTON); 	//设置BUTTON的皮肤
	//创建keypad
	keypad_dev.hKeypad = WM_CreateWindowAsChild(keypad_dev.xpos,keypad_dev.ypos,keypad_dev.width,keypad_dev.height, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, _cbKeyPad, 0);
	
	//设置notepad属性
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	FRAMEWIN_SetDefaultTextAlign(GUI_TA_HCENTER);

	//创建FRAME窗口
	hFrame = FRAMEWIN_CreateEx(0, 0, 800, 480, WM_HBKWIN, WM_CF_SHOW, 0, 0, "NOTEPAD", 0);
	FRAMEWIN_SetTextColor(hFrame, GUI_YELLOW);
	FRAMEWIN_SetFont(hFrame, &GUI_Font20_ASCII);
	FRAMEWIN_SetClientColor(hFrame, GUI_WHITE);
	
	//创建一个multi edit(多行文本小工具)小工具
	hMulti = MULTIEDIT_CreateEx(0, 0, 0, 0, WM_GetClientWindow(hFrame), WM_CF_SHOW, 0, GUI_ID_MULTIEDIT0, 100, NULL);
	MULTIEDIT_EnableBlink(hMulti,500,1);			//开启光标,周期500ms
    MULTIEDIT_SetInsertMode(hMulti,1);  //开启插入模式
	MULTIEDIT_SetFont(hMulti, &GUI_Font24_ASCII);
	WM_SetFocus(hMulti);
	
	while(1)
	{
		GUI_Delay(100);
	}
}


