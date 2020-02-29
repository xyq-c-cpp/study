#ifndef INC_WAPPLICATION
#define INC_WAPPLICATION
class WApplication
{
public:
	void InitUI();
	void ShowSysWindow();
private:
	void RegisterWindowClass();
	void CreateSysWindow();
	void EnterMessageLoop();
};

#endif