#include "stdafx.h"
#include "SEditEx.h"

namespace SOUI
{
	SEditEx::SEditEx()
	{
		GetEventSet()->addEvent(EVENTID(EventEditKeyReturn));
	}

	void SEditEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (nChar == VK_RETURN)
		{
			EventEditKeyReturn evt(this);
			FireEvent(evt);
			return;
		}
		return SRichEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}