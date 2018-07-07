#include "stdafx.h"
#include "SRelTabCtrl.h"

void SRelTabCtrl::InsertItemEx(STabPage *pPage, int iInsert)
{
	InsertChild(pPage);
	if (iInsert == -1)
		iInsert = m_lstPages.GetCount();
	m_lstPages.InsertAt(iInsert, pPage);

	CRect rcPage = GetChildrenLayoutRect();
	pPage->Move(&rcPage);
	pPage->SetVisible(FALSE, FALSE);
	if (m_nCurrentPage >= iInsert)  m_nCurrentPage++;
	
	if (m_nCurrentPage == -1) SetCurSel(iInsert);
}
void SRelTabCtrl::RemoveItemEx(STabPage *pPage)
{
	for (int i = 0; i < m_lstPages.GetCount(); i++)
	{
		if (pPage == m_lstPages[i])
		{
	       if (m_nCurrentPage > i)
				--m_nCurrentPage;
			RemoveChild(m_lstPages[i]);
			m_lstPages.RemoveAt(i);
		}
	}

}

int SRelTabCtrl::GetIndexById(int id)
{
	for (int i = GetItemCount() - 1; i >= 0; i--)
	{
		STabPage * pTab = GetItem(i);
		if (pTab->GetID() == id)
			return i;
	}

	return 0;
}


BOOL SRelTabCtrl::SetCurSelById(int id)
{
	int nIndex = GetIndexById(id);

	return SetCurSel(nIndex);
}


BOOL SRelTabCtrl::SetPageId(int nIndex, int id)
{
	if (nIndex < 0 || nIndex>(int)GetItemCount() - 1 || (m_nCurrentPage == nIndex))
		return FALSE;

	STabPage *pTab = GetItem(nIndex);

	if (pTab)
		pTab->SetID(id);

	return TRUE;
}