#ifndef _INC_WWINDOW
#define _INC_WWINDOW

#include"WCommon.h"
//Ä£°å²ÎÊý:ATMLogInDlg,Wdialog
/*template<class T>
T* CreatWWindow(int posx, int posy, int width,int height)
{
	T* pWindow= new T(posx,posy,width,height);
	pWindow->OnInit();
	return pWindow;
}*/
class WWindow{
protected:
	WWindow(int posx, int posy, int width,int height);
	~WWindow();
public:
	int GetLeftTopX();
	int GetLeftTopY();
	void SetTopX(int m_LeftTopX);
	void Refresh();
	virtual void OnInit();
	virtual void Show()=0;
	virtual bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam)=0;
	
	
protected:
	int m_LeftTopX;
	int m_LeftTopY;
	int m_Width;
	int m_Height;
};
#endif