#ifndef _INC_WDRAWMONEY
#define _INC_WDRAWMONEY
#include "WDialog.h"
#include "WButton.h"
#include "WTextBox.h"
#include "WLabel.h"

class WDrawMoneyDlg:public WDialog
{
protected:
    WDrawMoneyDlg(int posx,int posy,int width,int length);
public:	
	static WDrawMoneyDlg* CreatWWindow(int posx, int posy, int width,int height)
	{
	   WDrawMoneyDlg *pWindow=new WDrawMoneyDlg(posx,posy,width,height);
       pWindow->OnInit();
	   return pWindow;
	}

	virtual void OnInit();
	static void OnDrawClick(void *pThis);
	void OnDrawClick();
	static void OnReturnClick(void *pThis);
	void OnReturnClick();
	friend class WWindow;
private:
	WButton* m_pReturnBtn;
	WButton* m_pDrawBtn;
	WTextBox* m_pMoney;
	WLabel* m_pLabel;
	
};

#endif

