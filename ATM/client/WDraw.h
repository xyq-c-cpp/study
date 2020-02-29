#ifndef _INC_WDRAW_
#define _INC_WDRAW_
#include"WCommon.h"
#include<string>

class WDraw
{
public:
	static void DrawBitmap(int x, int y,int width, int height,  WbmpRes bmp);
	static void SetWTextColor(unsigned char r,unsigned char g,unsigned char b);
	static void SetFont(int fontWidth,int fontHeight,string fontName);
	static void DrawWText(int x,int y,int width,int height,string strText,bool isHCenter,bool isVCenter,bool ishide);
	static int GetNearIndexInString(int x,string strText,WFont ft);
	static int GetXAfterCharInString(int index,string strText,WFont ft);
	
	static void DrawRectangle(int x,int y,int width,int height,WColor clr);
	static void DrawCaret(int x,int y,int width,int height);
	static void WShowCaret();
	static void WHideCaret();

	friend class WScreen;
private:
	static void InitDrawer();
	static void DrawToScreen();
};
#endif