#ifndef _INC_ATNLOGINDLG
#define _INC_ATNLOGINDLG

#include"WDialog.h"
#include"WButton.h"
#include"WTextBox.h"
#include "WLabel.h"

class ATMLogInDlg:public WDialog
{
protected:
    ATMLogInDlg(int posx,int posy,int width,int length);
public:	
    static ATMLogInDlg* CreatWWindow(int posx, int posy, int width,int height)
	{
	   ATMLogInDlg *pWindow=new ATMLogInDlg(posx,posy,width,height);
       pWindow->OnInit();
	   return pWindow;
	}
	virtual void OnInit();
	static void OnLoginClick(void *pThis);
	void OnLoginClick();
	static void OnCancelLick(void *pThis);
	void OnCancelLick();
	friend class WWindow;
	
	//void OnLoginClick();
private:
	WButton* m_pLoginBtn;
	WButton* m_pCancelBtn;
	WTextBox* m_pCardID;
	WTextBox* m_pPwd;
	WLabel* m_pIDLab;
	WLabel* m_pPwdLab;
	
};
#endif