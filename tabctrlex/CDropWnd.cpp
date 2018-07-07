#include"stdafx.h"
#include "CDropWnd.h"


void CDropWnd::OnPaint(IRenderTarget *pRT)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	pRT->BitBlt(rcWindow, m_memRT, 0, 0, SRCCOPY);
}
void CDropWnd::CopyDropWindBkgndFormWindow(SPrintWindow *FormWindow)
{
	if (m_memRT)
		m_memRT = NULL;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	GETRENDERFACTORY->CreateRenderTarget(&m_memRT, rcWindow.Width(), rcWindow.Height());
	m_memRT->ClearRect(rcWindow, RGBA(100, 100, 100, 255));
	FormWindow->Print(m_memRT, rcWindow);
}