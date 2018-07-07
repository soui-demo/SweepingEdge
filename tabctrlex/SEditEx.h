#pragma once
#include <control/SCmnCtrl.h>

namespace SOUI
{
#define EVT_EDIT_KEYRETURN (EVT_EXTERNAL_BEGIN + 600)

	class EventEditKeyReturn : public TplEventArgs<EventEditKeyReturn>
	{
		SOUI_CLASS_NAME(EventEditKeyReturn, L"on_edit_return")
	public:
		EventEditKeyReturn(SObject *pSender) :TplEventArgs<EventEditKeyReturn>(pSender){}
		enum { EventID = EVT_EDIT_KEYRETURN };
	};


	class SEditEx
		:public SEdit
	{
		SOUI_CLASS_NAME(SEditEx, L"editex")
	public:
		SEditEx();

	protected:

		void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_KEYDOWN(OnKeyDown)
		SOUI_MSG_MAP_END()
	};
}