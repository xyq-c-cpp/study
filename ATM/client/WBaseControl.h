#ifndef _INC_WBASECONTROL_
#define _INC_WBASECONTROL_

#include"WWindow.h"
#include "WCommon.h"

class WDialog;

class WBaseControl:public WWindow
{
protected:
    WBaseControl(int posx,int posy,int width,int height);

protected:
	WFont m_font;

public:
	virtual bool IsShowCaret();
	void SetFatherWindow(WDialog *pFather);
	//È¡¾ø¶Ô×ø±ê
	int GetAbsLeftTopX();
	int GetAbsLeftTopY();
	void SetBackGround();
//	bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam);
	
	bool IsInWindow(int posx,int posy);

private:
	WDialog* m_pFatherWindow;
};
#endif