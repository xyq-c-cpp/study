#ifndef _INC_WTRANSFERDLG
#define _INC_WTRANSFERDLG
#include "WDialog.h"
#include "WButton.h"
#include "WTextBox.h"
#include "WLabel.h"


class WTransfer:public WDialog
{
protected:
    WTransfer(int posx,int posy,int width,int length);
public:	
	static WTransfer* CreatWWindow(int posx, int posy, int width,int height)
	{
	   WTransfer *pWindow=new WTransfer(posx,posy,width,height);
       pWindow->OnInit();
	   return pWindow;
	}

	virtual void OnInit();
	static void OnTransferClick(void *pThis);
	void OnTransferClick();
	static void OnReturnClick(void *pThis);
	void OnReturnClick();
	friend class WWindow;
private:

	WButton* m_pTransBtn;
	WButton* m_pReturnBtn;
	WTextBox* m_pMoneyBox;
	WTextBox* m_pIDBox;
	WLabel* m_pMoneyLabel;
	WLabel* m_pIDLabel;
	
	//一个lable“输入存入金额:”
	
};

#endif

