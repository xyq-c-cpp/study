#ifndef _INC_WSCREEN
#define _INC_WSCREEN
#include<list>
using namespace std;
#include"WDialog.h"

class WScreen
{
private:
	WScreen();
public:

	static WScreen* getSingletonScreen();
	void draw();
	void AddDialog(WDialog* pdlg);
	void DeleteDialog();
	bool Domessage(EMESSAGETYPE msgType,unsigned int wParam,long lParam);
	

private:
	list<WDialog*> m_DlgList;
};
#endif