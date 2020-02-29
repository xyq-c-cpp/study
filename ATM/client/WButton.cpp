#include"WButton.h"
#include"WDraw.h"

WButton::WButton(int posx,int posy,int width,int height)
:WBaseControl(posx,posy,width,height)
{
	m_btnStatus=BTN_NORMAL;
	m_pCallerObj=NULL;
}
void WButton::OnInit()
{
	
}

void WButton::SetCaption(string strCaption)
{
	m_strCaption=strCaption;
}

void WButton::Show()
{
	WbmpRes bkRes=m_BackGroundNormal;
	switch(m_btnStatus)
	{
	case BTN_NORMAL:
		bkRes=m_BackGroundNormal;
		break;
	case BTN_FOCUS:
		bkRes=m_BackGroundFocus;
		break;
	case BTN_PUSH:
        bkRes=m_BackGroundPush;
		break;
	default:
		break;
		
	}
	WDraw::DrawBitmap(GetAbsLeftTopX(),GetAbsLeftTopY(),m_Width,m_Height,bkRes);
	
	WDraw::SetWTextColor(255,255,255);
	WDraw::SetFont(m_font.width,m_font.height,m_font.sName);
	WDraw::DrawWText(GetAbsLeftTopX(),GetAbsLeftTopY(),m_Width,m_Height,m_strCaption,true,true,false);

}
WbmpRes WButton::GetBackGroundNormal()
{
	return m_BackGroundNormal;
}
WbmpRes WButton::GetBackGroundFocus()
{
	return m_BackGroundFocus;
}
WbmpRes WButton::GetBackGroundPush()
{
	return m_BackGroundPush;
}
void WButton::SetBackGroundNormal(WbmpRes bkRes)
{
	m_BackGroundNormal=bkRes;
}
void WButton::SetBackGroundFocus(WbmpRes bkRes)
{
	m_BackGroundFocus=bkRes;
}
void WButton::SetBackGroundPush(WbmpRes bkRes)
{
	m_BackGroundPush=bkRes;
}
bool WButton::Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam)
{
	bool bProcessed = false;
	EBUTTONSTATUS newStatus=BTN_NORMAL;
	if(msgType>MSG_MOUSE_START&&msgType<MSG_MOUSE_END)
	{
		if(IsInWindow(wParam,lParam))
		{
			bProcessed=true;
			switch(msgType)
			{
			case MSG_MOUSE_MOVE:
				{
					if(m_btnStatus==BTN_NORMAL)
					{
						newStatus=BTN_FOCUS;
						//m_btnStatus=BTN_FOCUS;
					}
					
				}
				break;
			case MSG_MOUSE_LBUTTONDOWN:
				{
					newStatus=BTN_PUSH;
				}
				break;
			case MSG_MOUSE_LBUTTONUP:
				{
					newStatus=BTN_FOCUS;
					if(m_pOnClick!=NULL)
					{
						m_pOnClick(m_pCallerObj);//=(*m_pOnClick)
					}
				}
				break;
			}
		}
		else
		{
			newStatus=BTN_NORMAL;
		}
		
	//	this->Show();
	}
	if(m_btnStatus!=newStatus)
	{
		m_btnStatus=newStatus;
		Refresh();
	}
	return bProcessed;
}

void WButton::SetClickCallBack(pClickCallBack pFunc,void* pCallerObj)
{
	m_pOnClick=pFunc;
	m_pCallerObj=pCallerObj;//保存这个类
}


