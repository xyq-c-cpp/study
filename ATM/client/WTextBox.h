#ifndef _INC_WTEXTBOX
#define _INC_WTEXTBOX
#include "WBaseControl.h"
#include"WCommon.h"
#include<string>
using namespace std;

class WTextBox:public WBaseControl
{
protected:
	WTextBox(int posx,int posy,int width,int height);
public:
	
	static WTextBox* CreateWWindow(int posx, int posy, int width,int height)
	{
	WTextBox* pWindow= new WTextBox(posx,posy,width,height);
	pWindow->OnInit();
	return pWindow;
	}
	virtual void Show();
	virtual bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam);
	virtual bool IsShowCaret();
	void SetBackGroundColor(WColor clr);
	void SetStrText(string str);
	string GetText();
	void SetIsHide(bool ishide);
	void SetCaret();

private:
	string m_strText;
	WColor m_BackGroundColor;
	bool m_IsFocus;
	int m_iNextInsertPos;
	bool IsHide;
	bool IsEdit;
};
#endif