#ifndef _INC_WLOGINFAILTIP
#define _INC_WLOGINFAILTIP
#include "WDialog.h"
#include "WButton.h"
#include "WLabel.h"


class WLoginFailTip:public WDialog
{
protected:
    WLoginFailTip(int posx,int posy,int width,int length);
public:	
	static WLoginFailTip* CreatWWindow(int posx, int posy, int width,int height)
	{
	   WLoginFailTip *pWindow=new WLoginFailTip(posx,posy,width,height);
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

