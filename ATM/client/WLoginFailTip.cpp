#include"WLoginFailTip.h"
#include"WCommon.h"
#include"WScreen.h"
#include "WDraw.h"

void WLoginFailTip::OnYesClick(void *pThis)
{
	
	WLoginFailTip* pObj=(WLoginFailTip*) pThis;
	if(pObj)
	{
		pObj->OnYesClick();
	}
}

void WLoginFailTip::OnYesClick()
{	
	
	WScreen::getSingletonScreen()->DeleteDialog();
	
}



WLoginFailTip::WLoginFailTip(int posx,int posy,int width,int height)
:WDialog(posx,posy,width,height)
{
}


void WLoginFailTip::OnInit()
{
	
	//¡°ÊÇ¡±btn
	m_pYesBtn=WButton::CreateWWindow(125,170,150,70);
	WbmpRes bmp;
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = true;
	m_pYesBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = true;
	m_pYesBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = true;
	m_pYesBtn->SetBackGroundPush(bmp);
	m_pYesBtn->SetCaption("ÊÇ");
	
	m_pYesBtn->SetClickCallBack(OnYesClick,this);

	this->AddControl(m_pYesBtn);


}