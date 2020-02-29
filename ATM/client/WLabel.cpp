#include "WLabel.h"
#include"WDraw.h"

WLabel::WLabel(int posx,int posy,int width,int height)
:WBaseControl(posx,posy,width,height)
{
}

void WLabel::OnInit()
{
}

bool WLabel::Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam)
{
	return false;
}


void WLabel::Show()
{
	WbmpRes bkRes=m_BackGroundNormal;
	WDraw::DrawBitmap(GetAbsLeftTopX(),GetAbsLeftTopY(),m_Width,m_Height,bkRes);
	WDraw::SetWTextColor(255,255,255);
	WDraw::SetFont(m_font.width,m_font.height,m_font.sName);
	WDraw::DrawWText(GetAbsLeftTopX(),GetAbsLeftTopY(),m_Width,m_Height,m_strCaption,true,true,false);
}



void WLabel::SetCaption(string strCaption)
{
	m_strCaption=strCaption;
}

