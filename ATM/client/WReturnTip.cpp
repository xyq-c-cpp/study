#include"WReturnTip.h"
#include"WCommon.h"
#include"WScreen.h"
#include "WDraw.h"

void WReturnTip::OnYesClick(void *pThis)
{
	
	WReturnTip* pObj=(WReturnTip*) pThis;
	if(pObj)
	{
		pObj->OnYesClick();
	}
}

void WReturnTip::OnYesClick()
{	
	WScreen::getSingletonScreen()->DeleteDialog();
	WScreen::getSingletonScreen()->DeleteDialog();
	
}


void WReturnTip::OnNoClick(void *pThis)
{
	
	WReturnTip* pObj=(WReturnTip*) pThis;
	if(pObj)
	{
		pObj->OnNoClick();
	}
}


void WReturnTip::OnNoClick()
{	
	WScreen::getSingletonScreen()->DeleteDialog();
}


WReturnTip::WReturnTip(int posx,int posy,int width,int height)
:WDialog(posx,posy,width,height)
{
}


void WReturnTip::OnInit()
{
	//提示信息label
	//m_pLabel=WLabel::CreateWWindow(0,0,260, 80);
	//m_pLabel->SetCaption("sftc");
	//this->AddControl(m_pLabel);
	
	
	//“是”btn
	m_pYesBtn=WButton::CreateWWindow(40,170,150,70);
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
	m_pYesBtn->SetCaption("是");
	
	m_pYesBtn->SetClickCallBack(OnYesClick,this);

	this->AddControl(m_pYesBtn);


	
	//“否”btn
	m_pNoBtn=WButton::CreateWWindow(210,170,150,70);
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = true;
	m_pNoBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = true;
	m_pNoBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = true;
	m_pNoBtn->SetBackGroundPush(bmp);
	m_pNoBtn->SetCaption("否");
	
	m_pNoBtn->SetClickCallBack(OnNoClick,this);

	this->AddControl(m_pNoBtn);
	

}