#pragma once

class SRelTabCtrl : public STabCtrl
{
	SOUI_CLASS_NAME(SRelTabCtrl, L"RelTabCtrl")
public:
	void InsertItemEx(STabPage *pPage, int iInsert = -1);
	void RemoveItemEx(STabPage *pPage);

	int GetIndexById(int id);
	BOOL SetCurSelById(int id);
	BOOL SetPageId(int nIndex, int id);
};