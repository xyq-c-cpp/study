#include "ATMMenuDlg.h"
#include "WScreen.h"
#include "WQueryDlg.h"
#include "WSaveDlg.h"
#include "WDrawMoneyDlg.h"
#include "WReturnTip.h"
#include "WTransfer.h"
void ATMMenuDlg::OnExitClick(void *pThis)
{
	
	ATMMenuDlg* pObj=(ATMMenuDlg*) pThis;
	if(pObj)
	{
		pObj->OnExitClick();
	}
}

void ATMMenuDlg::OnExitClick()
{
	//弹出提示框，确认是否退出
	WReturnTip* tip = WReturnTip::CreatWWindow(200,100,400,250);
	WbmpRes bmp;
	bmp.strPath="res/tip.bmp";
	bmp.isTransParent = false;
	tip->SetBackGround(bmp);
	tip->SetText("是否退出");
	WScreen::getSingletonScreen()->AddDialog(tip);

/*
	ATMMenuDlg* plg = ATMMenuDlg::CreatWWindow(0,0,800,480);
	WbmpRes bmp;
	bmp.strPath="res/menu.bmp";
	bmp.isTransParent = false;
	plg->SetBackGround(bmp);
	WScreen::getSingletonScreen()->AddDialog(plg);


	//添加数据库连接操作，检查账号密码是否匹配
	ATMMenuDlg* plg = ATMMenuDlg::CreatWWindow(0,0,800,480);
	WbmpRes bmp;
	bmp.strPath="res/menu.bmp";
	bmp.isTransParent = false;
	plg->SetBackGround(bmp);
	WScreen::getSingletonScreen()->AddDialog(plg);
	*/
}

void ATMMenuDlg::OnQueryClick(void *pThis)
{
	ATMMenuDlg* pObj=(ATMMenuDlg*) pThis;
	if(pObj)
	{
		pObj->OnQueryClick();
	}
}

void ATMMenuDlg::OnQueryClick()
{
	char sendbuf[50]={'1'};
    int i;
	for(i=1;i<11;i++)
	{
		sendbuf[i]=CardId[i-1];
	}
	sendbuf[i]='\0';

	char *getmoney=send1(sockCli,sendbuf);
	string str_money=getmoney;
	//数据库匹配成功在，则进入菜单页面

	WQueryDlg* plg = WQueryDlg::CreatWWindow(0,0,800,480);
	WbmpRes bmp;
	bmp.strPath="res/test.bmp";
	bmp.isTransParent = false;
	plg->SetBackGround(bmp);
	plg->SetText(str_money);
	WScreen::getSingletonScreen()->AddDialog(plg);
}


void ATMMenuDlg::OnSaveClick(void *pThis)
{
	ATMMenuDlg* pObj=(ATMMenuDlg*) pThis;
	if(pObj)
	{
		pObj->OnSaveClick();
	}
}
void ATMMenuDlg::OnSaveClick()
{
	WSaveDlg* plg = WSaveDlg::CreatWWindow(0,0,800,480);
	WbmpRes bmp;
	bmp.strPath="res/test.bmp";
	bmp.isTransParent = false;
	plg->SetBackGround(bmp);

	WScreen::getSingletonScreen()->AddDialog(plg);
}

void ATMMenuDlg::OnDrawClick(void *pThis)
{
	ATMMenuDlg* pObj=(ATMMenuDlg*) pThis;
	if(pObj)
	{
		pObj->OnDrawClick();
	}
}

void ATMMenuDlg::OnDrawClick()
{
	WDrawMoneyDlg* plg = WDrawMoneyDlg::CreatWWindow(0,0,800,480);
	WbmpRes bmp;
	bmp.strPath="res/test.bmp";
	bmp.isTransParent = false;
	plg->SetBackGround(bmp);

	WScreen::getSingletonScreen()->AddDialog(plg);
}



void ATMMenuDlg::OnTransferClick(void *pThis)
{
	ATMMenuDlg* pObj=(ATMMenuDlg*) pThis;
	if(pObj)
	{
		pObj->OnTransferClick();
	}
}

void ATMMenuDlg::OnTransferClick()
{
	WTransfer* plg = WTransfer::CreatWWindow(0,0,800,480);
	WbmpRes bmp;
	bmp.strPath="res/test.bmp";
	bmp.isTransParent = false;
	plg->SetBackGround(bmp);

	WScreen::getSingletonScreen()->AddDialog(plg);
}


ATMMenuDlg::ATMMenuDlg(int posx,int posy,int width,int height)
:WDialog(posx,posy,width,height)
{
}
void ATMMenuDlg::OnInit()
{
	//查询按钮
	m_pQueryBtn=WButton::CreateWWindow(285,20,227,75);
	WbmpRes bmp;
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = true;
	m_pQueryBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = true;
	m_pQueryBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = true;
	m_pQueryBtn->SetBackGroundPush(bmp);
	m_pQueryBtn->SetCaption("查询");
	
	m_pQueryBtn->SetClickCallBack(OnQueryClick,this);

	//存款按钮
	m_pSaveBtn=WButton::CreateWWindow(285,110,227,75);
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = false;
	m_pSaveBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = false;
	m_pSaveBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = false;
	m_pSaveBtn->SetBackGroundPush(bmp);
	m_pSaveBtn->SetCaption("存款");
	m_pSaveBtn->SetClickCallBack(OnSaveClick,this);

	//取款按钮
	m_pDrawBtn=WButton::CreateWWindow(285,200,227,75);
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = false;
	m_pDrawBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = false;
	m_pDrawBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = false;
	m_pDrawBtn->SetBackGroundPush(bmp);
	m_pDrawBtn->SetCaption("取款");
	m_pDrawBtn->SetClickCallBack(OnDrawClick,this);

	
	//转账按钮
	m_pTransfer=WButton::CreateWWindow(280,290,227,75);
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = false;
	m_pTransfer->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = false;
	m_pTransfer->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = false;
	m_pTransfer->SetBackGroundPush(bmp);
	m_pTransfer->SetCaption("转账");
	m_pTransfer->SetClickCallBack(OnTransferClick,this);

	//退出按钮
	m_pExitBtn=WButton::CreateWWindow(280,380,227,75);
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = false;
	m_pExitBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = false;
	m_pExitBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = false;
	m_pExitBtn->SetBackGroundPush(bmp);
	m_pExitBtn->SetCaption("退出");
	m_pExitBtn->SetClickCallBack(OnExitClick,this);//


	this->AddControl(m_pQueryBtn);
	this->AddControl(m_pSaveBtn);
	this->AddControl(m_pDrawBtn);
	this->AddControl(m_pTransfer);
	this->AddControl(m_pExitBtn);

	
}