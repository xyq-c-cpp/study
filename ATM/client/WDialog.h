#ifndef _INC_WDIALOG_
#define _INC_WDIALOG_
#include"WWindow.h"
#include"WCommon.h"
#include"list"
#include"WBaseControl.h"

class WDialog:public WWindow
{
protected:
	WDialog(int posx, int posy, int width,int height);
public:
	virtual void Show();
	virtual void OnInit();
	
	void SetBackGround(WbmpRes bk);
	void SetText(string str);
	void AddControl(WBaseControl *pControl);
	static WDialog* CreatWWindow(int posx, int posy, int width,int height)
	{
    	WDialog* pDialog= new WDialog(posx,posy,width,height);
    	pDialog->OnInit();
	    return pDialog;
	}
	virtual bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam);
	bool IsShowCaret();
private:
	WbmpRes m_BackGround;
	string m_Text;
	list<WBaseControl *> m_ControlList;
};
#endif