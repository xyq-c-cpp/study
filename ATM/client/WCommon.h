#ifndef _INC_WCOMMON_
#define _INC_WCOMMON_

#include<string>
#include"windows.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;


extern SOCKET sockCli ;                        //´´½¨Ì×½Ó×ÖsockCli
extern char CardId[20];
bool send0(SOCKET sockCli,char s[]);
char* send1(SOCKET sockCli,char s[]);
bool send3(SOCKET sockCli,char s[]);

struct WColor{
unsigned char r;
unsigned char g;
unsigned char b; 
};

struct WbmpRes{
string strPath;
bool isTransParent;
WColor keyColor;
};

struct WFont
{
	int width;
	int height;
	string sName;
};

enum EBUTTONSTATUS
{
	BTN_NORMAL,
	BTN_FOCUS,
	BTN_PUSH

};
enum EMESSAGETYPE
{
	//paint message
	MSG_PAINT,
	//mouse message start
	MSG_MOUSE_START,
		MSG_MOUSE_MOVE,
		MSG_MOUSE_LBUTTONDOWN,
		MSG_MOUSE_LBUTTONUP,
	MSG_MOUSE_END,
	//mouse message end

	MSG_KEY_CHAR
};

class BankCard
{
private:
	string id;
	int psw;
public:
	BankCard()
	{
		id="unknow";
		psw=000000;
	}
	void SetId(string _id)
	{
		id=_id;
	}
	void SetPsw(int _psw)
	{
		psw=_psw;
	}
};


#endif