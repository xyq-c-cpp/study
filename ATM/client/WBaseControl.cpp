#include"WBaseControl.h"
#include"stdlib.h"
#include"WDialog.h"
#include"WDraw.h"
WBaseControl::WBaseControl(int posx,int posy,int width,int height)
:WWindow(posx,posy,width,height)
{
	m_font.width = 30;
	m_font.height = 60;
	m_font.sName = "ºÚÌå";
	m_pFatherWindow = NULL;
}

void WBaseControl::SetFatherWindow(WDialog* pFather)
{
	m_pFatherWindow = pFather;
}

int WBaseControl::GetAbsLeftTopX()
{
	if(m_pFatherWindow!=NULL)
   {
	   return m_pFatherWindow->GetLeftTopX()+m_LeftTopX;
	 //  WBaseControl::m_pFatherWindow->GetLeftTopX()+m_LeftTopX;
	}	
   else
   {
	   return m_LeftTopX;
   }
}
int WBaseControl::GetAbsLeftTopY()
{
   if(m_pFatherWindow!=NULL)
   {
	   return m_pFatherWindow->GetLeftTopY()+m_LeftTopY;
   }
   else
   {
	   return m_LeftTopY;
   }
}
bool WBaseControl::IsInWindow(int posx,int posy)
{
	if(posx>=GetAbsLeftTopX()&&posx<=GetAbsLeftTopX()+m_Width&&
		posy>=GetAbsLeftTopY()&&posy<=GetAbsLeftTopY()+m_Height)
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool WBaseControl::IsShowCaret()
{
	return false;
}