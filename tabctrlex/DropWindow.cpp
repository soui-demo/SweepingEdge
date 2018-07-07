#include "stdafx.h"
#include "DropWindow.h"

//需要一个鼠标穿透的窗口
int CDropWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//lpCreateStruct->dwExStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
	SetMsgHandled(FALSE);
	return 0;
}

BOOL CDropWindow::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	m_bLayoutInited = TRUE;
	//DWORD dwExStyle= GetWindowLong(GWL_EXSTYLE);
	//SetWindowLong(GWL_EXSTYLE, dwExStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	ModifyStyleEx(0,  WS_EX_TRANSPARENT);
	m_dummyWnd.ModifyStyleEx(0, WS_EX_TRANSPARENT);
	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);	
	
	return 0;
}

CDropWindow::CDropWindow() : SHostWnd(_T("LAYOUT:XML_MAINDROPS"))
{	
	m_bLayoutInited = FALSE;
}


CDropWindow::~CDropWindow()
{
}
