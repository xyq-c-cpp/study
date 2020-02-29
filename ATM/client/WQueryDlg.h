#ifndef _INC_WQUERYDLG
#define _INC_WQUERYDLG
#include "WDialog.h"
#include "WButton.h"
#include "WLabel.h"

class WQueryDlg:public WDialog
{
protected:
    WQueryDlg(int posx,int posy,int width,int length);
public:	
	static WQueryDlg* CreatWWindow(int posx, int posy, int width,int height)
	{
	   WQueryDlg *pWindow=new WQueryDlg(posx,posy,width,height);
       pWindow->OnInit();
	   return pWindow;
	}

	virtual void OnInit();
	static void OnReturnClick(void *pThis);
	void OnReturnClick();
	friend class WWindow;
private:
	WButton* m_pReturnBtn;
	WLabel* m_pLabel;
	WLabel* m_pMoney;
	
};

#endif

