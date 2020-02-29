#include"WApplication.h"
#include"windows.h"
#include"WDialog.h"
#include"WScreen.h"
#include"WButton.h"
#include"ATMLogInDlg.h"
#include <wingdi.h>
#include "WCommon.h"
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"ws2_32.lib")

#include "stdafx.h"
#include <iostream>
//#include <winsock2.h>
SOCKET sockCli;
char CardId[20];
//extern SOCKET sockCli;                           //创建套接字sockCli
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{	
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2),&wsaData))
	{
		cout<<"Winsock不能被初始化!"<<endl;
		WSACleanup();
		return 0;
	}
	
	//extern SOCKET sockCli;                           //创建套接字sockCli
	sockCli=socket(AF_INET,SOCK_STREAM,0);

    SOCKADDR_IN addrSer;                      //客户端只要创建一个套接字地址
	addrSer.sin_family=AF_INET;
	addrSer.sin_port=htons(5566);
	addrSer.sin_addr.S_un.S_addr=inet_addr("172.16.8.17"); //服务器IP  10.1.38.10

	int res=connect(sockCli,(SOCKADDR*)&addrSer,sizeof(SOCKADDR));
	
    WApplication app;
	app.InitUI();
    //模板T：ATMLogInDlg* plg = WWindow::CreatWWindow<ATMLogInDlg>(0,0,800,480);
    ATMLogInDlg* plg = ATMLogInDlg::CreatWWindow(0,0,800,480);
	WbmpRes bmp;

	bmp.strPath="res/test.bmp";
	bmp.isTransParent = false;
	plg->SetBackGround(bmp);
	WScreen::getSingletonScreen()->AddDialog(plg);
	app.ShowSysWindow();

	//char sendbuf[256],recvbuf[256];

	
	//WSACleanup();
	return 0;

}

bool send0(SOCKET sockCli,char s[])
{
	char recvbuf[256];
	send(sockCli,s,strlen(s)+1,0);
	recv(sockCli,recvbuf,256,0);
	if(recvbuf[0]=='1')
	return true;
	else return false;
}

char* send1(SOCKET sockCli,char s[])
{
	static char recvbuf[256];
	send(sockCli,s,strlen(s)+1,0);
	recv(sockCli,recvbuf,256,0);
	static char *p=recvbuf;
	return p;
}
