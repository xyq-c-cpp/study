#include"WDialog.h"
#include"WDraw.h"

WDialog::WDialog(int posx, int posy, int width,int height):
WWindow(posx,posy,width,height)
{
}

void WDialog::OnInit()
{

}


void WDialog::SetBackGround(WbmpRes bk)
{
	m_BackGround = bk;
}

void WDialog::SetText(string str)
{
	m_Text=str;
}


void WDialog::Show()
{
	WDraw::DrawBitmap(m_LeftTopX,m_LeftTopY,m_Width,m_Height,m_BackGround);
	//WDraw::DrawWText(GetAbsLeftTopX(),GetAbsLeftTopY(),m_Width,m_Height,m_str,true,true);

	WDraw::DrawWText(m_LeftTopX,m_LeftTopY,m_Width,m_Height,m_Text,true,true,false);

	
    list<WBaseControl *>::iterator it;
	for(it=m_ControlList.begin();it!=m_ControlList.end();it++)
	{
		WBaseControl *pControl=*it;
		pControl->Show();
	}
}

void WDialog::AddControl(WBaseControl *pControl)
{
	if(pControl!=NULL)
	{
		m_ControlList.push_back(pControl);
		pControl->SetFatherWindow(this);
	}
	
}
bool WDialog::Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam)
{
	bool bProcessed=false;
	list<WBaseControl*>::iterator it;
	for(it=m_ControlList.begin();it!=m_ControlList.end();it++)
	{
		WBaseControl *pControl=*it;
		if(pControl->Domessage(msgType,wParam,lParam))
		{
			bProcessed=true;
			this->Show();
			break;
		}
	}
	return bProcessed;
}

bool WDialog::IsShowCaret()
{
	bool bShowCaret=false;
	list<WBaseControl*>::iterator it;
	for(it=m_ControlList.begin();it!=m_ControlList.end();it++)
	{
		WBaseControl *pControl=*it;
		if(pControl->IsShowCaret())
		{
			bShowCaret=true;
			break;
		}
	}
	return bShowCaret;
}
