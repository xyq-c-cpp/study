#include"WDrawMoneyDlg.h"
#include"WCommon.h"
#include"WScreen.h"
#include"WQueryDlg.h"
#include"WTip.h"


WDrawMoneyDlg::WDrawMoneyDlg(int posx,int posy,int width,int height)
:WDialog(posx,posy,width,height)
{
}

void WDrawMoneyDlg::OnDrawClick(void *pThis)
{
	
	WDrawMoneyDlg* pObj=(WDrawMoneyDlg*) pThis;
	if(pObj)
	{
		pObj->OnDrawClick();
	}
}

void WDrawMoneyDlg::OnDrawClick()
{	
	////////////////////////////触发取钱操作
	int i;
	string str_money=m_pMoney->GetText();

	char sendbuf[50];
	sendbuf[0]='2';
    for(i=1;i<=10;i++)
	{
		sendbuf[i]=CardId[i-1];
	}

	for(i=11;i<str_money.length()+11;i++)
	{
		sendbuf[i]=str_money.at(i-11);
	}
	sendbuf[i]='\0';
	
	
	
	//添加数据库连接操作，检查账号密码是否匹配
	//char sendbuf[20]={'1','2','3','4'};
	if(send0(sockCli,sendbuf))
	{	
		WTip* tip = WTip::CreatWWindow(200,100,400,250);
		WbmpRes bmp;
		bmp.strPath="res/tip.bmp";
		bmp.isTransParent = false;
		tip->SetBackGround(bmp);
		tip->SetText("取款成功!");
		WScreen::getSingletonScreen()->AddDialog(tip);
	}
}


void WDrawMoneyDlg::OnReturnClick(void *pThis)
{
	
	WDrawMoneyDlg* pObj=(WDrawMoneyDlg*) pThis;
	if(pObj)
	{
		pObj->OnReturnClick();
	}
}

void WDrawMoneyDlg::OnReturnClick()
{	
	WScreen::getSingletonScreen()->DeleteDialog();
}

void WDrawMoneyDlg::OnInit()
{
	//“输入金额”lable
	m_pLabel=WLabel::CreateWWindow(260,180,260, 60);
	m_pLabel->SetCaption("取款金额:");
	this->AddControl(m_pLabel);

	//金额输入框
	m_pMoney=WTextBox::CreateWWindow(540, 180, 200, 60);
	WColor clr={255,255,255};
	m_pMoney->SetBackGroundColor(clr);
	

	//确认取款btn
	m_pDrawBtn=WButton::CreateWWindow(250,300,227,75);
	WbmpRes bmp;
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = true;
	m_pDrawBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = true;
	m_pDrawBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = true;
	m_pDrawBtn->SetBackGroundPush(bmp);
	m_pDrawBtn->SetCaption("确认");
	m_pDrawBtn->SetClickCallBack(OnDrawClick,this);
	
	
	//返回主菜单btn
	m_pReturnBtn=WButton::CreateWWindow(500,300,227,75);
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

	this->AddControl(m_pMoney);
	this->AddControl(m_pReturnBtn);

	this->AddControl(m_pDrawBtn);

}