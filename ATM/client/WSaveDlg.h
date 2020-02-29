#ifndef _INC_WSAVEDLG
#define _INC_WSAVEDLG
#include "WDialog.h"
#include "WButton.h"
#include "WTextBox.h"
#include "WLabel.h"





class WSaveDlg:public WDialog
{
protected:
    WSaveDlg(int posx,int posy,int width,int length);
public:	
	static WSaveDlg* CreatWWindow(int posx, int posy, int width,int height)
	{
	   WSaveDlg *pWindow=new WSaveDlg(posx,posy,width,height);
       pWindow->OnInit();
	   return pWindow;
	}

	virtual void OnInit();
	static void OnSaveClick(void *pThis);
	void OnSaveClick();
	static void OnReturnClick(void *pThis);
	void OnReturnClick();
	friend class WWindow;
private:

	WButton* m_pSaveBtn;
	WButton* m_pReturnBtn;
	WTextBox* m_pMoney;
	WLabel* m_pLabel;
	
	//一个lable“输入存入金额:”
	
};

#endif

