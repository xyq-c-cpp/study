#include"WSaveDlg.h"
#include"WCommon.h"
#include"WScreen.h"
#include"WQueryDlg.h"
#include"WTip.h"

void WSaveDlg::OnReturnClick(void *pThis)
{
	
	WSaveDlg* pObj=(WSaveDlg*) pThis;
	if(pObj)
	{
		pObj->OnReturnClick();
	}
}

void WSaveDlg::OnReturnClick()
{	
	WScreen::getSingletonScreen()->DeleteDialog();
}


void WSaveDlg::OnSaveClick(void *pThis)
{
	
	WSaveDlg* pObj=(WSaveDlg*) pThis;
	if(pObj)
	{
		pObj->OnSaveClick();
	}
}

void WSaveDlg::OnSaveClick()
{	
	int i;
	string str_money=m_pMoney->GetText();

	char sendbuf[50];
	sendbuf[0]='3';
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
		tip->SetText("存款成功!");
		WScreen::getSingletonScreen()->AddDialog(tip);
	}
	
}

WSaveDlg::WSaveDlg(int posx,int posy,int width,int height)
:WDialog(posx,posy,width,height)
{
}

void WSaveDlg::OnInit()
{
	//“输入金额”lable
	m_pLabel=WLabel::CreateWWindow(260,180,260, 60);
	m_pLabel->SetCaption("存入金额:");
	this->AddControl(m_pLabel);

	//金额输入框
	m_pMoney=WTextBox::CreateWWindow(540, 180, 200, 60);
	WColor clr={255,255,255};
	m_pMoney->SetBackGroundColor(clr);
	

	//确认存储btn
	m_pSaveBtn=WButton::CreateWWindow(250,300,227,75);
	WbmpRes bmp;
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = true;
	m_pSaveBtn->SetBackGroundNormal(bmp);
	

	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = true;
	m_pSaveBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = true;
	m_pSaveBtn->SetBackGroundPush(bmp);
	m_pSaveBtn->SetCaption("确认");

	m_pSaveBtn->SetClickCallBack(OnSaveClick,this);
	
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

	this->AddControl(m_pSaveBtn);

}