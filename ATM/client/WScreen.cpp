#include"WScreen.h"
#include "WDraw.h"
WScreen::WScreen()
{
	WDraw::InitDrawer();
}
WScreen* WScreen::getSingletonScreen()
{
	static WScreen screen;
	return &screen;
}

void WScreen::AddDialog(WDialog* pdlg)
{	
	if(pdlg != NULL)
	{
		m_DlgList.push_back(pdlg);
	}
}

void WScreen::DeleteDialog()
{
	WDialog* pLastDlg=m_DlgList.back();
	if(pLastDlg != NULL)//删除当前（最后一个）对话框
	{
		m_DlgList.pop_back();
		//Refresh();
	}
}

void WScreen::draw()
{
	list<WDialog*>::iterator it;
	for(it = m_DlgList.begin(); it !=  m_DlgList.end(); ++it)
	{
		WDialog* pdlg = *it;
		pdlg->Show();
	}
	WDraw::DrawToScreen();
	WDialog* pLastDlg=m_DlgList.back();
	if(pLastDlg!=NULL && pLastDlg->IsShowCaret())
	{
		WDraw::WShowCaret();
	}
	else
	{
		WDraw::WHideCaret();
	}
}
bool WScreen::Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam)//只处理最上层对话框的消息
{
	bool bProcessed=false;
	if(msgType==MSG_PAINT)
	{
		draw();
		bProcessed=true;
	}
	else
	{
		WDialog *pLastDlg=m_DlgList.back();
		if(pLastDlg!=NULL)
		{
			pLastDlg->Domessage(msgType,wParam,lParam);
		}
	}
	return bProcessed;
}