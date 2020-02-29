#ifndef _INC_WLABEL_
#define _INC_WLABEL_
#include"WBaseControl.h"
#include "WCommon.h"
#include<string>
//typedef void (*pClickCallBack)();//定义一个类型

class WLabel:public WBaseControl
{
protected:
	WLabel(int posx,int posy,int width,int height);
public:
	
static WLabel* CreateWWindow(int posx, int posy, int width,int height)
{
	WLabel* pLabel= new WLabel(posx,posy,width,height);
	pLabel->OnInit();
	return pLabel;
}
	virtual void Show();
	virtual void OnInit();
	void SetCaption(string strCaption);
	virtual bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam);
	/*
	WbmpRes GetBackGroundFocus();
	WbmpRes GetBackGroundPush();
	void SetBackGroundNormal(WbmpRes bkRes);
	void SetBackGroundFocus(WbmpRes bkRes);
	void SetBackGroundPush(WbmpRes bkRes);
	void SetClickCallBack(pClickCallBack pFunc);
	virtual bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam);
	*/
	
	
private:
	WbmpRes m_BackGroundNormal;
	/*
	WbmpRes m_BackGroundFocus;
	WbmpRes m_BackGroundPush;

	EBUTTONSTATUS m_btnStatus;
	*/
	std::string m_strCaption;
	//pClickCallBack m_pOnClick;
};
#endif