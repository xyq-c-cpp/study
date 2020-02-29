#ifndef _INC_WBUTTON_
#define _INC_WBUTTON_
#include"WBaseControl.h"
#include "WCommon.h"
#include<string>
typedef void (*pClickCallBack)(void* pThis);//定义一个类型,无参数无返回值的函数指针,可改为含参数的，()中可以设定相应需要传入的参数
//无法指定用于哪个类，则设置为void*

class WButton:public WBaseControl
{
protected:
	WButton(int posx,int posy,int width,int height);
public:
	
static WButton* CreateWWindow(int posx, int posy, int width,int height)
{
	WButton* pButton= new WButton(posx,posy,width,height);
	pButton->OnInit();
	return pButton;
}
	virtual void Show();
	virtual void OnInit();
	virtual bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam);
	
	WbmpRes GetBackGroundNormal();
	WbmpRes GetBackGroundFocus();
	WbmpRes GetBackGroundPush();
	void SetBackGroundNormal(WbmpRes bkRes);
	void SetBackGroundFocus(WbmpRes bkRes);
	void SetBackGroundPush(WbmpRes bkRes);
	void SetCaption(string strCaption);
	void SetClickCallBack(pClickCallBack pFunc,void* pCallerObj);
	//void SetClickCallBack(pClickCallBack pFunc);
private:
	WbmpRes m_BackGroundNormal;
	WbmpRes m_BackGroundFocus;
	WbmpRes m_BackGroundPush;

	EBUTTONSTATUS m_btnStatus;
	std::string m_strCaption;
	//void (*pClickCallBack)();
	pClickCallBack m_pOnClick;
	void* m_pCallerObj;
};
#endif