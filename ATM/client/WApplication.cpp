#include"WApplication.h"
#include"windows.h"
#include"WScreen.h"
#include"WDialog.h"
// 全局变量: 
HINSTANCE hInst; 
TCHAR* szTitle = "";                  // 标题栏文本
TCHAR* szWindowClass = "WindowClass";            // 主窗口类名
HWND g_hWnd = NULL; //主窗口句柄
int g_UIx = 0;
int g_UIy = 0;
int g_UIWidth = 800;
int g_UIHeight = 480;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	EMESSAGETYPE msgType;
	unsigned int wWParam=0;
	long lWParam=0;

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		 //TODO: 在此处添加使用 hdc 的任何绘图代码...
		msgType=MSG_PAINT;
		WScreen::getSingletonScreen()->draw();
	    EndPaint(hWnd, &ps);
	}
		break;
	case WM_MOUSEMOVE:
		msgType=MSG_MOUSE_MOVE;
		wWParam=LOWORD(lParam);
		lWParam=HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
		msgType=MSG_MOUSE_LBUTTONDOWN;
		wWParam=LOWORD(lParam);
		lWParam=HIWORD(lParam);
		break;
	case WM_LBUTTONUP:
        msgType=MSG_MOUSE_LBUTTONUP;
		wWParam=LOWORD(lParam);
		lWParam=HIWORD(lParam);
		break;
	case WM_CHAR:
		msgType=MSG_KEY_CHAR;
		wWParam=wParam;
		break;
	default:
		//LMessage msg;
		//msg.msgtype = LMSG_UNKOWN;
		//getCanvasSingleton()->DoMessage(msg);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	WScreen::getSingletonScreen()->Domessage(msgType,wWParam,lWParam);
	return 0;
}
void WApplication::RegisterWindowClass()
{
	hInst = ::GetModuleHandle(NULL);
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= LoadIcon(hInst, NULL);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, NULL);

	RegisterClassEx(&wcex);
}
void WApplication::CreateSysWindow()
{
	g_hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP,
		g_UIx, g_UIy, g_UIWidth, g_UIHeight, NULL, NULL, hInst, NULL);
}


void WApplication::EnterMessageLoop()
{
	MSG msg;

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void WApplication::ShowSysWindow()
{
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
	EnterMessageLoop();
}

void WApplication::InitUI()
{
	RegisterWindowClass();
	CreateSysWindow();
}