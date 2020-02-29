#ifndef _INC_WRETURNTIP
#define _INC_WRETURNTIP
#include "WDialog.h"
#include "WButton.h"
#include "WLabel.h"
#include"list"

class WReturnTip:public WDialog
{
protected:
    WReturnTip(int posx,int posy,int width,int length);
public:	
	static WReturnTip* CreatWWindow(int posx, int posy, int width,int height)
	{
	   WReturnTip *pWindow=new WReturnTip(posx,posy,width,height);
       pWindow->OnInit();
	   return pWindow;
	}

	virtual void OnInit();
	//virtual void Show();
	//void AddControl(WBaseControl *pControl);
	
	//void SetBackGround(WbmpRes bk);
	//void SetText(string str);
	static void OnYesClick(void *pThis);
	void OnYesClick();
	static void OnNoClick(void *pThis);
	void OnNoClick();
	friend class WWindow;
private:
	WButton* m_pYesBtn;
	WButton* m_pNoBtn;
	WLabel* m_pLabel;
	WbmpRes m_BackGround;
	//list<WBaseControl *> m_ControlList;
};

#endif

