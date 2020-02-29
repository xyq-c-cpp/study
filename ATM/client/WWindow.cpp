#include"WWindow.h"
#include"WScreen.h"

WWindow::WWindow(int posx, int posy, int width,int height)
{
	m_LeftTopX = posx;
	m_LeftTopY = posy;
	m_Width = width;
	m_Height = height;
}

WWindow::~WWindow()
{
}
int WWindow::GetLeftTopX()
{
	return m_LeftTopX;
}
int WWindow::GetLeftTopY()
{
	return m_LeftTopY;
}
void WWindow::OnInit()
{
}
void WWindow::Refresh()
{
	WScreen::getSingletonScreen()->draw();
}
