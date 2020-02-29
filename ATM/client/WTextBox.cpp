#include"WTextBox.h"
#include"WDraw.h"
#define KEY_BACKSPACE 8
WTextBox::WTextBox(int posx,int posy,int width,int height)
:WBaseControl(posx,posy,width,height)
{
	m_IsFocus = false;
	m_iNextInsertPos = 0;
	m_font.width = 25;
	m_font.height = 50;
	m_font.sName = "ËÎÌו";
	IsHide = false;
	IsEdit=true;
}

void WTextBox::SetBackGroundColor(WColor clr)
{
	m_BackGroundColor=clr;
}

void WTextBox::SetStrText(string str)
{
	m_strText=str;
}


void WTextBox::Show()
{
	WDraw::DrawRectangle(GetAbsLeftTopX(),GetAbsLeftTopY(),m_Width,m_Height,m_BackGroundColor);
	
	WDraw::SetWTextColor(0,0,0);
	WDraw::SetFont(m_font.width,m_font.height,m_font.sName);
	WDraw::DrawWText(GetAbsLeftTopX(),GetAbsLeftTopY(),m_Width,m_Height,m_strText,false,true,IsHide);
	//WDraw::DrawCaret(GetAbsLeftTopX()+2,GetAbsLeftTopY(),1,m_Height);
	if(IsShowCaret()&&IsEdit)
	{
		int x = GetAbsLeftTopX() + WDraw::GetXAfterCharInString(m_iNextInsertPos,m_strText,m_font);
		WDraw::DrawCaret(x ,GetAbsLeftTopY(),2,m_Height);
	}
}

bool WTextBox::Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam)
{
	bool bProcessed = false;
	if(msgType == MSG_MOUSE_LBUTTONDOWN)
	{
		if(IsInWindow(wParam,lParam))
		{
			bProcessed = false;
		    m_IsFocus = true;
			m_iNextInsertPos = WDraw::GetNearIndexInString(wParam-GetAbsLeftTopX(),m_strText,m_font);
		}
		else
		{
			m_IsFocus = false;
		}
		Refresh();

	}
	if(msgType == MSG_KEY_CHAR && m_IsFocus&&IsEdit)
	{
		bProcessed = true;
		if(wParam == KEY_BACKSPACE)
		{
			m_strText.erase(m_iNextInsertPos-1,1);
			m_iNextInsertPos--;

		}
		else
		{
		   char* ch = (char*)&wParam;
	       m_strText.insert(m_iNextInsertPos,ch);
		   m_iNextInsertPos++;
		}
		Refresh();
	}
	return bProcessed;
}

bool WTextBox::IsShowCaret()
{
	return m_IsFocus;
}

void WTextBox::SetCaret()
{
	m_IsFocus=false;
}


string WTextBox::GetText()
{
	return m_strText;
}

void WTextBox::SetIsHide(bool ishide)
{
	IsHide = ishide;
}
