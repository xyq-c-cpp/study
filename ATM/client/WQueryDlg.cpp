#include"WQueryDlg.h"
#include"WCommon.h"
//#include "ATMMenuDlg.h"
#include"WScreen.h"

void WQueryDlg::OnReturnClick(void *pThis)
{
	
	WQueryDlg* pObj=(WQueryDlg*) pThis;
	if(pObj)
	{
		pObj->OnReturnClick();
	}
}

void WQueryDlg::OnReturnClick()
{	
	WScreen::getSingletonScreen()->DeleteDialog();
}



WQueryDlg::WQueryDlg(int posx,int posy,int width,int height)
:WDialog(posx,posy,width,height)
{
}

void WQueryDlg::OnInit()
{
	
	//“余额”、余额信息label
	m_pLabel=WLabel::CreateWWindow(270,150,260, 60);
	m_pLabel->SetCaption("当前余额:");
	
	this->AddControl(m_pLabel);

	/*
	m_pMoney=WLabel::CreateWWindow(540, 180, 180, 60);
	m_pMoney->SetCaption("00.00");
	this->AddControl(m_pMoney);
	
*/
	//返回主菜单btn
	m_pReturnBtn=WButton::CreateWWindow(290,300,227,75);
	WbmpRes bmp;
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = true;
	m_pReturnBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = true;
	m_pReturnBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = true;
	m_pReturnBtn->SetBackGroundPush(bmp);
	m_pReturnBtn->SetCaption("返回");
	
	m_pReturnBtn->SetClickCallBack(OnReturnClick,this);

	this->AddControl(m_pReturnBtn);


}