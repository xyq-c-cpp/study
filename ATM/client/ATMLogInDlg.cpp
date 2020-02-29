#include"ATMLogInDlg.h"
#include"WCommon.h"
#include "ATMMenuDlg.h"
#include"WScreen.h"
#include"WLoginFailTip.h"
#include <string>
#include <stdlib.h>
#include<stdio.h>


//处理回调
void ATMLogInDlg::OnLoginClick(void *pThis)//static版本不生成隐式参数 
{
	ATMLogInDlg* pObj=(ATMLogInDlg*) pThis;
	if(pObj)
	{
		pObj->OnLoginClick();//通过传递获取当前类实例的指针，再去调用非static版本
	}
}

//非static成员函数会默认多一个参数（一个对象或者对象指针），static成员函数不需要传递对象指针（static成员和方法都不依赖对象存在）
void ATMLogInDlg::OnLoginClick()
{
	//获取输入框中的卡号、密码
	int i;
	string strCardNo=m_pCardID->GetText();
	string  strPwd=m_pPwd->GetText();
	
/*	int x=0;
	char str1[10];
	sprintf(str1,"%d",x);
	char str2[10];
	sprintf(str2,"%s",strCardNo);
	char str3[10];
	sprintf(str3,"%s",strPwd);

	char *strCat=(char*)malloc(strlen(str1)+strlen(str2)+strlen(str3));
	sprintf(strCat,"%s%s%s",str1,str2,str3);
*/	
	
	string str=strCardNo+strPwd;
	char sendstr[50];
	sendstr[0]='0';
	for(i=1;i<str.length()+1;i++)
	{
		sendstr[i]=str.at(i-1);
	}
	sendstr[i]='\0';
	//添加数据库连接操作，检查账号密码是否匹配
	//char sendbuf[20]={'1','2','3','4'};
	
	
	if(send0(sockCli,sendstr))
	{
	//数据库匹配成功，则进入菜单页面
		for(i=0;i<strCardNo.length();i++)
		{
			CardId[i]=strCardNo.at(i);
		}
		CardId[i]='\0';
		ATMMenuDlg* plg = ATMMenuDlg::CreatWWindow(0,0,800,480);
		WbmpRes bmp;
		bmp.strPath="res/menu.bmp";
		bmp.isTransParent = false;
		plg->SetBackGround(bmp);
		WScreen::getSingletonScreen()->AddDialog(plg);
		m_pCardID->SetStrText("");
		m_pPwd->SetStrText("");
		m_pCardID->Domessage(MSG_MOUSE_LBUTTONDOWN,300,200);
		m_pPwd->Domessage(MSG_MOUSE_LBUTTONDOWN,300,280);
		m_pPwd->SetCaret();
		m_pCardID->SetCaret();
		Refresh();
	}
	else
	{
		//清空输入框	
		m_pCardID->SetStrText("");
		m_pPwd->SetStrText("");
		
		m_pPwd->Domessage(MSG_MOUSE_LBUTTONDOWN,300,280);
		m_pCardID->Domessage(MSG_MOUSE_LBUTTONDOWN,300,200);
		
		m_pPwd->SetCaret();
		m_pCardID->SetCaret();
		//弹出提示框，密码错误，请重新输入

		WLoginFailTip* tip = WLoginFailTip::CreatWWindow(200,100,400,250);
		WbmpRes bmp;
		bmp.strPath="res/tip.bmp";
		bmp.isTransParent = false;
		tip->SetBackGround(bmp);
		tip->SetText("密码错误");
		WScreen::getSingletonScreen()->AddDialog(tip);
	}
}


void ATMLogInDlg::OnCancelLick(void *pThis)
{
	ATMLogInDlg* pObj=(ATMLogInDlg*) pThis;
	if(pObj)
	{
		pObj->OnCancelLick();
	}
}

void ATMLogInDlg::OnCancelLick()
{
	m_pCardID->SetStrText("");
	m_pPwd->SetStrText("");
	
	m_pPwd->Domessage(MSG_MOUSE_LBUTTONDOWN,300,280);
	m_pCardID->Domessage(MSG_MOUSE_LBUTTONDOWN,300,200);

}

ATMLogInDlg::ATMLogInDlg(int posx,int posy,int width,int height)
:WDialog(posx,posy,width,height)
{
}
void ATMLogInDlg::OnInit()
{
	//卡号、密码标签
	m_pIDLab=WLabel::CreateWWindow(90, 150, 120, 60);
	m_pIDLab->SetCaption("账号");
	m_pPwdLab=WLabel::CreateWWindow(90, 240, 120, 60);
	m_pPwdLab->SetCaption("密码");
	this->AddControl(m_pIDLab);
	this->AddControl(m_pPwdLab);

	//卡号输入框、密码输入框
	m_pCardID=WTextBox::CreateWWindow(220, 150, 500, 60);
	WColor clr={255,255,255};
	m_pCardID->SetBackGroundColor(clr);

	m_pPwd=WTextBox::CreateWWindow(220, 240, 500, 60);
	m_pPwd->SetBackGroundColor(clr);
	m_pPwd->SetIsHide(true);

	this->AddControl(m_pCardID);
	this->AddControl(m_pPwd);


	//登录按钮
	m_pLoginBtn=WButton::CreateWWindow(230,350,227,75);
	WbmpRes bmp;
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = false;
	m_pLoginBtn->SetBackGroundNormal(bmp);
	
	
	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = false;
	m_pLoginBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = false;
	m_pLoginBtn->SetBackGroundPush(bmp);
	m_pLoginBtn->SetCaption("登录");
	m_pLoginBtn->SetClickCallBack(OnLoginClick,this);
	this->AddControl(m_pLoginBtn);
	
	//取消按钮
	m_pCancelBtn=WButton::CreateWWindow(480,350,227,75);
	bmp.strPath="res/btn_N.bmp";
    bmp.isTransParent = false;
	m_pCancelBtn->SetBackGroundNormal(bmp);
	
	
	bmp.strPath="res/btn_F.bmp";
    bmp.isTransParent = false;
	m_pCancelBtn->SetBackGroundFocus(bmp);

	bmp.strPath="res/btn_P.bmp";
    bmp.isTransParent = false;
	m_pCancelBtn->SetBackGroundPush(bmp);
	m_pCancelBtn->SetCaption("取消");
	m_pCancelBtn->SetClickCallBack(OnCancelLick,this);
	this->AddControl(m_pCancelBtn);

}