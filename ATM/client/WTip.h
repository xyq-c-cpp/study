#ifndef _INC_WTIP
#define _INC_WTIP
#include "WDialog.h"
#include "WButton.h"
#include "WLabel.h"


class WTip:public WDialog
{
protected:
    WTip(int posx,int posy,int width,int length);
public:	
	static WTip* CreatWWindow(int posx, int posy, int width,int height)
	{
	   WTip *pWindow=new WTip(posx,posy,width,height);
       pWindow->OnInit();
	   return pWindow;
	}

	virtual void OnInit();
	//void Show();
	static void OnYesClick(void *pThis);
	void OnYesClick();
	//static void OnNoClick(void *pThis);
	//void OnNoClick();
	friend class WWindow;
private:
	WButton* m_pYesBtn;
	//WButton* m_pNoBtn;
	//WLabel* m_pLabel;
	WbmpRes m_BackGround;
	//list<WBaseControl *> m_ControlList;
};

#endif

