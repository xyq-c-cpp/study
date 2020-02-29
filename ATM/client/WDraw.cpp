#include"WDraw.h"
#include"windows.h"
#include <wingdi.h>

extern HWND g_hWnd; //Ö÷´°¿Ú¾ä±ú
extern int g_UIx;
extern int g_UIy;
extern int g_UIWidth;
extern int g_UIHeight;
HDC g_hMemDC=NULL;
HDC g_hDC=NULL;
HBITMAP g_BitMap=NULL;

void WDraw::WShowCaret()

{
	ShowCaret(g_hWnd);
}

void WDraw::WHideCaret()
{
	HideCaret(g_hWnd);
}

void WDraw::DrawCaret(int x,int y,int width,int height)
{
	static sWidth=0;
	static sHeight=0;
	if(width!=sWidth||height!=sHeight)
	{
		DestroyCaret();
		CreateCaret(g_hWnd,NULL,width,height);
		sWidth=width;
		sHeight=height;
	}
	SetCaretPos(x,y);
	//ShowCaret();
}

void WDraw::InitDrawer()
{
	g_hDC=GetDC(g_hWnd);
	g_hMemDC=CreateCompatibleDC(g_hDC);
	g_BitMap=CreateCompatibleBitmap(g_hDC,g_UIWidth,g_UIHeight);
	SelectObject(g_hMemDC,g_BitMap);
}
void WDraw::DrawBitmap(int x, int y,int width, int height,  WbmpRes bmp)
{
		HDC mdc = CreateCompatibleDC(g_hDC);
		HBITMAP hbmp = (HBITMAP)LoadImage(
			NULL,
			bmp.strPath.c_str(),
			IMAGE_BITMAP,
			width,
			height,
			LR_LOADFROMFILE
			);
		HGDIOBJ hOldObject=SelectObject(mdc,hbmp);

		BitBlt(
			g_hMemDC,
			x,y,
			width,
			height,
			mdc,
			0,0,
			SRCCOPY
			);
		SelectObject(mdc,hOldObject);
		DeleteObject(hbmp);
		DeleteDC(mdc);
		
}

void WDraw::DrawToScreen()
{
	BitBlt(
			g_hDC,
			g_UIx,g_UIy,
			g_UIWidth,
			g_UIHeight,
			g_hMemDC,
			0,0,
			SRCCOPY
			);
}

void WDraw::SetWTextColor(unsigned char r,unsigned char g,unsigned char b)
{
	//SetWTextColor(g_hMemDC,RGB(r,g,b));
	SetTextColor(g_hMemDC,RGB(r,g,b));
}

void WDraw::SetFont(int fontWidth,int fontHeight,string fontName)
{
	static int sfontWidth=0;
	static int sfontHeight=0;
	static string sfontName="";
	static HFONT hf=NULL;
	if(fontWidth!=sfontWidth||fontHeight!=sfontHeight||fontName!=sfontName)
	{
		if(hf!=NULL)
		{
			DeleteObject(hf);
		}
		hf=::CreateFont(fontHeight,fontWidth,0,0,0,0,0,0,0,0,0,0,0,fontName.c_str());
		::SelectObject(g_hMemDC,hf);
		sfontWidth=fontWidth;
		sfontHeight=fontHeight;
		sfontName=fontName;
	}
}

void WDraw::DrawWText(int x,int y,int width,int height,string strText,bool isHCenter,bool isVCenter,bool ishide)
{
	RECT rt={x,y,x+width,y+height};
	UINT uFormat=DT_SINGLELINE;
	if(isHCenter)
	{
		uFormat=uFormat|DT_CENTER;
	}
	if(isVCenter)
	{
		uFormat=uFormat|DT_VCENTER;
	}
	string text;
	if(ishide == true)
	{
		for(int i = 0;i < strText.size();i++)
		{
			text.insert(i,"*");
		}
	}
	else text = strText;
	
	::SetBkMode(g_hMemDC,TRANSPARENT);
	::DrawText(g_hMemDC,text.c_str(),text.size(),&rt,uFormat);
}

void WDraw::DrawRectangle(int x,int y,int width,int height,WColor clr)
{
	RECT rt={x,y,x+width,y+height};
	HBRUSH hBrush=CreateSolidBrush(RGB(clr.r,clr.g,clr.b));
	FillRect(g_hMemDC,&rt,hBrush);
	DeleteObject(hBrush);
}

int WDraw::GetNearIndexInString(int x,string strText,WFont ft)
{
	SetFont(ft.width,ft.height,ft.sName);

	if(strText.empty())
	{
		return 0;
	}

	int index = 0;
	SIZE sz;
	do
	{
	  index++;
	  ::GetTextExtentPoint(g_hMemDC,strText.c_str(),index,&sz);
	}while(sz.cx < x && index < strText.size());

	return index;
}

int WDraw::GetXAfterCharInString(int index,string strText,WFont ft)
{
	SetFont(ft.width,ft.height,ft.sName);
	SIZE sz;
	::GetTextExtentPoint(g_hMemDC,strText.c_str(),index,&sz);
	return sz.cx;
}
