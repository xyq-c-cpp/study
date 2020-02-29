#ifndef _INC_ATMMENUDLG
#define _INC_ATMMENUDLG
//#include "ATMMenuDlg.h"
#include "WDialog.h"
#include "WButton.h"


class ATMMenuDlg:public WDialog
{
protected:
    ATMMenuDlg(int posx,int posy,int width,int length);
public:	
    static ATMMenuDlg* CreatWWindow(int posx, int posy, int width,int height)
	{
	   ATMMenuDlg *pATMLoginDlg=new ATMMenuDlg(posx,posy,width,height);
       pATMLoginDlg->OnInit();
	   return pATMLoginDlg;
	}
	virtual void OnInit();
	void OnExitClick();
	static void OnExitClick(void *pThis);
	void OnQueryClick();
	static void OnQueryClick(void *pThis);
	void OnSaveClick();
	static void OnSaveClick(void *pThis);
	void OnDrawClick();
	static void OnDrawClick(void *pThis);
	void OnTransferClick();
	static void OnTransferClick(void *pThis);
	friend class WWindow;
private:
	WButton* m_pQueryBtn;
	WButton* m_pExitBtn;
	WButton* m_pDrawBtn;
	WButton* m_pSaveBtn;
	WButton* m_pTransfer;
};
#endif