#include "stdafx.h"

#include "SBrowserTab.h"
#include <control/SCmnCtrl.h>
#include "DropTarget.h"
#include "TargetData.h"

namespace SOUI
{
	const wchar_t KXmlTabStyle[] = L"tabStyle";
	const wchar_t KXmlCloseBtnStyle[] = L"closeBtnStyle";
	const wchar_t KXmlNewBtnStyle[] = L"newBtnStyle";
	const wchar_t KXmlTitleStyle[] = L"titleStyle";
	//统一命名，不用再XML指定名称
	const wchar_t Name_Btn_Close[] = L"closeBtn";
	const wchar_t Name_Logo[] = L"Logo";
	const wchar_t Name_Title_Lable[] = L"Title";
	const wchar_t Name_Loging[] = L"loading";


	SPrintWindow::SPrintWindow(SWindow *pHostCtrl) :m_pHostCtrl(pHostCtrl)
	{
		SASSERT(pHostCtrl);
		CRect hostRc = pHostCtrl->GetWindowRect();
		pHostCtrl->InsertChild(this);
		Move(hostRc);
		GETRENDERFACTORY->CreateRenderTarget(&m_memRT, hostRc.Width(), hostRc.Height());
		CPoint pt;
		pt -= hostRc.TopLeft();
		m_memRT->SetViewportOrg(pt);
		PaintBackground(m_memRT, &hostRc);
		m_memRT->SetViewportOrg(CPoint());
		SetVisible(FALSE, TRUE);
	}
	//更新
	void SPrintWindow::Update()
	{
		SASSERT(m_pHostCtrl);
		CRect hostRc = m_pHostCtrl->GetWindowRect();
		m_memRT = NULL;
		Move(hostRc);
		BringWindowToTop();
		GETRENDERFACTORY->CreateRenderTarget(&m_memRT, hostRc.Width(), hostRc.Height());
		CPoint pt;
		pt -= hostRc.TopLeft();
		m_memRT->SetViewportOrg(pt);
		PaintBackground(m_memRT, &hostRc);
		m_memRT->SetViewportOrg(CPoint());
		SetVisible(FALSE, TRUE);
	}
	void SPrintWindow::Print(IRenderTarget *pRT, CRect &rcWindow)
	{
		pRT->BitBlt(rcWindow, m_memRT, 0, 0, SRCCOPY);
	}

	SBrowserTab::SBrowserTab(SBrowserTabCtrl* pHost)
		:m_bAllowClose(TRUE)
		, m_pHost(pHost)
		, m_iOrder(-1)
		, m_iTabIndex(-1)
		, m_bLoading(true)
		, m_bCloseHostWnd(false)
	{
		m_iTabId = GetTickCount();
		m_bClipClient = TRUE;
	}

	void SBrowserTab::MoveTo(const CRect & rcEnd)
	{
		m_rcBegin = GetWindowRect();
		m_rcEnd = rcEnd;
		Stop();
		Start(200);
	}
	
	void SBrowserTab::CloseMe()
	{
		if (m_pHost)
		{
			m_pHost->CloseTab(m_iOrder);
		}
	}

	void SBrowserTab::CloseOther()
	{
		if (m_pHost)
		{
			m_pHost->CloseTabExcept(m_iOrder);
		}
	}

	void SBrowserTab::ClosePre()
	{
		if (m_pHost)
		{
			m_pHost->ClosePre(m_iOrder);
		}
	}
	void SBrowserTab::CloseNext()
	{
		if (m_pHost)
		{
			m_pHost->CloseNext(m_iOrder);
		}
	}
	void SBrowserTab::CanClosePre()
	{
		if (m_pHost)
		{
			m_pHost->CanClosePre(m_iOrder);
		}
	}
	void SBrowserTab::CanCloseNext()
	{
		if (m_pHost)
		{
			m_pHost->CanCloseNext(m_iOrder);
		}
	}

	void SBrowserTab::SegregateMe()
	{
		STabPage *Page = m_pHost->GetPage(m_iTabId);
		IHostInterface *ihostwnd = m_pHost->GetHostFactory()->NewWindow();
		ihostwnd->CreateHostWnd();
		ihostwnd->SetAutoNewCreateTab(false);
		ihostwnd->InitDialog();
		RemoveMe();
		ihostwnd->InsertTab(this, Page);
		CPoint curPos;
		GetCursorPos(&curPos);
		ihostwnd->SetPos(curPos);
		TryCloseHostWnd();
	}
	void SBrowserTab::RemoveMe()
	{		
		if (m_pHost == 0)
			return;
		GetEventSet()->unsubscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnTabClick, m_pHost));
		//closebtn
		SWindow *closbtn = FindChildByName(Name_Btn_Close);
		if (closbtn)
			closbtn->GetEventSet()->unsubscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnBtnCloseTabClick, m_pHost));

		m_pHost->RemoveTab(this, false);
		m_bCloseHostWnd = false;
		if (m_pHost->GetTabCount() == 0)
		{
			m_bCloseHostWnd = true;
			m_hHostWnd = GetContainer()->GetHostHwnd();
			ShowWindow(m_hHostWnd, SW_HIDE);
		}
		RevokeDragDrop();
		m_pHost = NULL;
	}

	void SBrowserTab::TryCloseHostWnd()
	{
		if (m_bCloseHostWnd&&IsWindow(m_hHostWnd))
		{
			PostMessage(m_hHostWnd, WM_CLOSE, 0, 0);
		}
	}
	//插入
	void SBrowserTab::Insert(SBrowserTab *tabWnd, STabPage *tabPage, bool bFront = true)
	{
		if (tabWnd->m_pHost != NULL)
			return;
		int insertPos = bFront ? m_iOrder : m_iOrder + 0;

		if (tabWnd&&m_pHost)
		{
			m_pHost->InsertTabEx(tabWnd, tabPage, insertPos);
		}
	}

	void SBrowserTab::OnAnimatorState(int percent)
	{
		CRect rcTemp;
		rcTemp.left = m_rcBegin.left + (m_rcEnd.left - m_rcBegin.left)*percent / 100;
		rcTemp.top = m_rcBegin.top + (m_rcEnd.top - m_rcBegin.top)*percent / 100;
		rcTemp.right = m_rcBegin.right + (m_rcEnd.right - m_rcBegin.right)*percent / 100;
		rcTemp.bottom = m_rcBegin.bottom + (m_rcEnd.bottom - m_rcBegin.bottom)*percent / 100;
		Move(rcTemp);
	}

	void SBrowserTab::OnMouseMove(UINT nFlags, CPoint pt)
	{
		SWindow *parent = GetParent();
		if (parent)
			m_ParentWinrc = parent->GetWindowRect();
		//此处有两种情况，拖动在SBrowserTabCtrl 还可能拖出去了。
		if (m_ParentWinrc.PtInRect(pt))
		{
			if ((nFlags & MK_LBUTTON) && IsDragable())
			{
				CRect rcWnd = GetWindowRect();
				rcWnd.OffsetRect(pt.x - m_ptDrag.x, 0);
				Move(rcWnd);
				m_ptDrag = pt;
				m_pHost->ChangeTabPos(this, pt);
				m_bDrag = true;
			}
		}
		else
		{
			if ((nFlags & MK_LBUTTON) && IsDragable())
			{
				//if (m_bDrag == true)
				{
					//从tabctrl移除但不销毁，因为它还要使用
					//SPrintWindow *pPrintWindow = new SPrintWindow(this);
					TargetData data;
					data.tabWnd = this;
					data.printwnd = m_pPrintWindow;
					CRect rcWnd = GetWindowRect();
					data.Pos.SetPoint(m_ptDes.x, m_ptDes.y);
					data.page = m_pHost->GetPage(m_iTabId);
					data.hostfactory = m_pHost->GetHostFactory();
					RemoveMe();
					CDropHelper::StartDrop(data);
				}
			}
		}
		//m_pHost->UpdateChildrenPosition();
	}

	void SBrowserTab::OnLButtonUp(UINT nFlags, CPoint pt)
	{
		SWindow::OnLButtonUp(nFlags, pt);
		if (m_bDrag)
			m_pHost->UpdateChildrenPosition();
	}

	void SBrowserTab::OnMouseLeave()
	{
		__super::OnMouseLeave();
		if (m_pHost)
			m_pHost->UpdateChildrenPosition();
	}

	void SBrowserTab::ReleaseOldCapture()
	{
		if (IsWindow(m_pcontainer->GetHostHwnd()))
		{
			if (m_pcontainer != GetContainer())
			{
				m_pcontainer->OnReleaseSwndCapture();
			}
		}
	}

	void SBrowserTab::OnLButtonDown(UINT nFlags, CPoint pt)
	{
		m_pcontainer = GetContainer();
		SWindow::OnLButtonDown(nFlags, pt);
		m_pPrintWindow->Update();
		BringWindowToTop();
		CRect rcWnd = GetWindowRect();
		m_ptDes.x = pt.x - rcWnd.left;
		m_ptDes.y = pt.y - rcWnd.top;
		m_ptDrag = pt;
		m_bDrag = false;
	}

	HRESULT SBrowserTab::SetTitle(SStringT szVaule, BOOL bLoading)
	{
		SStatic *pTitle = FindChildByName2<SStatic>(L"title");
		if (pTitle)
		{
			pTitle->SetWindowText(szVaule);
		}
		return S_OK;
	}

	int SBrowserTab::OnCreate(LPVOID)
	{
		int nRet = __super::OnCreate(NULL);
		if (m_iOrder >= 0)
			RegisterDragDrop();
		m_pPrintWindow = new SPrintWindow(this);
		return nRet;
	}
	BOOL SBrowserTab::RegisterDragDrop()
	{
		return GetContainer()->RegisterDragDrop(GetSwnd(), new CDropTarget(this));
	}
	BOOL SBrowserTab::RevokeDragDrop()
	{
		return GetContainer()->RevokeDragDrop(GetSwnd());
	}
	void SBrowserTab::OnDestroy()
	{
		RevokeDragDrop();
		__super::OnDestroy();
	}

	void SBrowserTab::OnSize(UINT nType, CSize size)
	{
		SWindow *parent = GetParent();
		if (parent)
			m_ParentWinrc = parent->GetWindowRect();
	}

	void SBrowserTab::SetTitle(LPCTSTR szTitle, LPCTSTR szTip)
	{
		SStatic *pTitle = FindChildByName2<SStatic>(Name_Title_Lable);
		if (pTitle)
		{
			pTitle->SetWindowText(szTitle);
			if (szTip == NULL)
			{
				SetAttribute(L"tip", S_CT2W(szTitle));
			}
			else
			{
				SetAttribute(L"tip", S_CT2W(szTip));
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// SBrowserTabCtrl
	SBrowserTabCtrl::SBrowserTabCtrl(void)
		:m_pSelTab(NULL)
		, m_tabAlign(TDIR_HORZ)
		, m_nDesiredSize(200)
		, m_bEnableDrag(TRUE)
		, m_pBtnNew(NULL)
		, m_pRelTabCtrl(NULL)
		, m_HostFactory(NULL)
		, m_pCacheTab(NULL)
	{
		m_bClipClient = TRUE;
		m_evtSet.addEvent(EVENTID(EventBrowserTabNew));
		m_evtSet.addEvent(EVENTID(EventBrowserTabClose));
		m_evtSet.addEvent(EVENTID(EventBrowserTabSelChanged));
		m_evtSet.addEvent(EVENTID(EventBrowserTabGetIcon));
	}

	SBrowserTabCtrl::~SBrowserTabCtrl(void)
	{
	}

	void SBrowserTabCtrl::SetTabCtrl(SRelTabCtrl* tabctrl)
	{
		m_pRelTabCtrl = tabctrl;
	}

	STabPage* SBrowserTabCtrl::GetPage(int id)
	{
		if (m_pRelTabCtrl != NULL)
		{
			return m_pRelTabCtrl->GetItem(m_pRelTabCtrl->GetIndexById(id));
		}
		return NULL;
	}

	void SBrowserTabCtrl::RmovePage(int id)
	{
		if (m_pRelTabCtrl != NULL)
		{
			m_pRelTabCtrl->RemoveItemEx(GetPage(id));
		}
	}

	int SBrowserTabCtrl::ChangeTabPos(SBrowserTab* pCurMove, CPoint ptCur)
	{
		for (int i = 0; i<(int)m_lstTabOrder.GetCount(); i++)
		{
			if (m_lstTabOrder[i] == pCurMove)
			{
				continue;
			}
			CRect rcWnd = m_lstTabOrder[i]->GetWindowRect();
			CPoint ptCenter = rcWnd.CenterPoint();

			if ((m_tabAlign == TDIR_HORZ && ptCenter.x <= ptCur.x && rcWnd.right >= ptCur.x)
				|| (m_tabAlign == TDIR_VERT && ptCenter.y <= ptCur.y && rcWnd.bottom >= ptCur.y))
			{
				if (m_tabAlign == TDIR_HORZ)
				{
					if (pCurMove->m_iOrder > m_lstTabOrder[i]->m_iOrder)
					{
						rcWnd.OffsetRect(rcWnd.Width(), 0);
					}
					else
					{
						rcWnd.OffsetRect(-rcWnd.Width(), 0);
					}
				}
				else
				{
					if (pCurMove->m_iOrder > m_lstTabOrder[i]->m_iOrder)
					{
						rcWnd.OffsetRect(0, rcWnd.Height());
					}
					else
					{
						rcWnd.OffsetRect(0, -rcWnd.Height());
					}
				}
				int order = pCurMove->m_iOrder;
				pCurMove->m_iOrder = m_lstTabOrder[i]->m_iOrder;
				m_lstTabOrder[i]->m_iOrder = order;
				m_lstTabOrder[i]->Move(rcWnd);
				SBrowserTab* pTemp = m_lstTabOrder[i];
				m_lstTabOrder[pCurMove->m_iOrder] = pCurMove;
				m_lstTabOrder[pTemp->m_iOrder] = pTemp;
			}
		}
		return 1;
	}

	BOOL SBrowserTabCtrl::CreateChildren(pugi::xml_node xmlNode)
	{
		//m_xmlStyle.append_copy(xmlNode);
		pugi::xml_node xmlTabStyle = xmlNode.child(KXmlTabStyle);

		if (xmlTabStyle)
		{
			m_xmlStyle.append_copy(xmlTabStyle);
		}		
		
		//title
		pugi::xml_node xmlTitle = xmlNode.child(KXmlTitleStyle);
		if (xmlTitle)
		{
			m_xmlStyle.append_copy(xmlTitle);
		}
		pugi::xml_node xmlCloseBtn = xmlNode.child(KXmlCloseBtnStyle);
		if (xmlCloseBtn)
		{
			m_xmlStyle.append_copy(xmlCloseBtn);
		}	


		pugi::xml_node xmlTabs = xmlNode.child(L"tabs");//所有tab都必须在tabs标签内

		for (pugi::xml_node xmlChild = xmlTabs.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
		{
			if (wcscmp(xmlChild.name(), SBrowserTab::GetClassName()) != 0)
				continue;
			SBrowserTab * pTab = new SBrowserTab(this);
			SASSERT(pTab);
			pTab->m_iOrder = pTab->m_iTabIndex = m_lstTabOrder.GetCount();
			m_lstTabOrder.Add(pTab);
			InsertChild(pTab);
			if (xmlTabStyle)
				pTab->InitFromXml(xmlTabStyle);
			
			//title
			pugi::xml_node xmlTitle = m_xmlStyle.child(KXmlTitleStyle);
			if (xmlTitle)
			{
				SWindow *pTitle = SApplication::getSingleton().CreateWindowByName(SStatic::GetClassName());
				pTab->InsertChild(pTitle);
				pTitle->InitFromXml(xmlTitle);
			}
			pugi::xml_node xmlCloseBtn = m_xmlStyle.child(KXmlCloseBtnStyle);
			if (xmlCloseBtn)
			{
				if (!pTab->m_bAllowClose) continue;
				SWindow *pBtn = SApplication::getSingleton().CreateWindowByName(SImageButton::GetClassName());
				pTab->InsertChild(pBtn);
				pBtn->InitFromXml(xmlCloseBtn);
				pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnBtnCloseTabClick, this));
			}
			pTab->InitFromXml(xmlChild);

			//pTab->SetTitle(xmlChild.value(),xmlChild.attribute(L"tip").value());
			pTab->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnTabClick, this));
		}

		pugi::xml_node xmlNewBtn = xmlNode.child(KXmlNewBtnStyle);
		if (xmlNewBtn)
		{
			m_pBtnNew = new SBrowserTab(this);
			InsertChild(m_pBtnNew);
			m_pBtnNew->m_iOrder = -1;
			m_pBtnNew->InitFromXml(xmlNewBtn);
			m_pBtnNew->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnBtnNewClick, this));
		}
		return TRUE;
	}

	void SBrowserTabCtrl::UpdateChildrenPosition()
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		CSize szBtnNew;
		if (m_pBtnNew) szBtnNew = m_pBtnNew->GetDesiredSize(&rcClient);
		CRect rcTab = rcClient;
		if (m_tabAlign == TDIR_HORZ)
		{
			int nTabWid = 0;
			if (rcClient.Width() - szBtnNew.cx > (int)m_lstTabOrder.GetCount()*m_nDesiredSize)
			{
				nTabWid = m_nDesiredSize;
			}
			else
			{
				nTabWid = (rcClient.Width() - szBtnNew.cx) / m_lstTabOrder.GetCount();
			}
			rcTab.right = rcTab.left + nTabWid;
			for (UINT i = 0; i < m_lstTabOrder.GetCount(); i++)
			{
				m_lstTabOrder[i]->MoveTo(rcTab);
				rcTab.OffsetRect(nTabWid, 0);
			}
			rcTab.left++;
			if (m_pBtnNew)
			{
				CRect rcNewBtn = CRect(rcTab.TopLeft(), szBtnNew);
				m_pBtnNew->MoveTo(rcNewBtn);
			}

		}
		else
		{
			int nTabHei = 0;
			if (rcClient.Height() - szBtnNew.cy > (int)m_lstTabOrder.GetCount()*m_nDesiredSize)
			{
				nTabHei = m_nDesiredSize;
			}
			else
			{
				nTabHei = (rcClient.Height() - szBtnNew.cx) / m_lstTabOrder.GetCount();
			}
			rcTab.bottom = rcTab.top + nTabHei;
			for (UINT i = 0; i < m_lstTabOrder.GetCount(); i++)
			{
				m_lstTabOrder[i]->MoveTo(rcTab);
				rcTab.OffsetRect(0, nTabHei);
			}
		}
		if (m_pBtnNew)
		{
			CRect rcNewBtn = CRect(rcTab.TopLeft(), szBtnNew);
			m_pBtnNew->MoveTo(rcNewBtn);
		}

	}

	bool SBrowserTabCtrl::OnBtnNewClick(EventArgs *pEvt)
	{
		InsertTab(L"新建选项卡", L"https://cn.bing.com");
		return true;
	}

	BOOL SBrowserTabCtrl::RemoveTab(int iTab)
	{
		if (iTab < 0 || iTab >= (int)m_lstTabOrder.GetCount()) return FALSE;
		return RemoveTab(m_lstTabOrder[iTab]);
	}

	void SBrowserTabCtrl::CloseTabExcept(int tabid)
	{
		if (tabid < 0 || tabid >= (int)m_lstTabOrder.GetCount()) return;
		int count = m_lstTabOrder.GetCount();
		for (int id = m_lstTabOrder.GetCount() - 1; id >= 0; id--)
		{
			if (id != tabid)
				CloseTab(id);
		}
	}

	void SBrowserTabCtrl::CloseTab(int tabid)
	{
		if (tabid < 0 || tabid >= (int)m_lstTabOrder.GetCount()) return;
		EventBrowserTabClose evt(this);
		evt.pCloseTab = m_lstTabOrder[tabid];
		evt.iCloseTab = m_lstTabOrder[tabid]->m_iOrder;
		evt.iCloseTabId = m_lstTabOrder[tabid]->m_iTabId;
		FireEvent(evt);
		RemoveTab(m_lstTabOrder[tabid]);
	}

	void SBrowserTabCtrl::CloseAllTab()
	{
		for (int tabid = m_lstTabOrder.GetCount() - 1; tabid >= 0; tabid--)
		{
			EventBrowserTabClose evt(this);
			evt.pCloseTab = m_lstTabOrder[tabid];
			evt.iCloseTab = m_lstTabOrder[tabid]->m_iOrder;
			evt.iCloseTabId = m_lstTabOrder[tabid]->m_iTabId;
			FireEvent(evt);
			RemoveTab(m_lstTabOrder[tabid]);
		}
	}

	void SBrowserTabCtrl::ClosePre(int tabid)
	{
		CloseTab(--tabid);
	}
	void SBrowserTabCtrl::CloseNext(int tabid)
	{
		CloseTab(++tabid);
	}
	bool SBrowserTabCtrl::CanClosePre(int tabid)
	{
		--tabid;
		if (tabid < 0 || tabid >= (int)m_lstTabOrder.GetCount())
			return false;
		return true;
	}
	bool SBrowserTabCtrl::CanCloseNext(int tabid)
	{
		++tabid;
		if (tabid < 0 || tabid >= (int)m_lstTabOrder.GetCount())
			return false;
		return true;
	}
	BOOL SBrowserTabCtrl::RemoveTab(SBrowserTab *pTab, bool bRelease)
	{
		void *me = this;
		SASSERT(pTab);
		int iOrder = pTab->m_iOrder;
		SASSERT(iOrder != -1);

		m_lstTabOrder.RemoveAt(iOrder);
		if (pTab == m_pSelTab)
		{
			m_pSelTab = NULL;
			if (iOrder - 1 >= 0)
				SetCurSel(iOrder - 1);
			else
			{
				if (m_lstTabOrder.GetCount() > 0)
					SetCurSel(0);
			}
		}
		if (bRelease)
			DestroyChild(pTab);
		else
		{
			pTab->InvalidateRect(NULL);
			m_pCacheTab = pTab;
			RemoveChild(pTab);
		}
		UpdateChildrenPosition();
		for (int i = 0; i < (int)m_lstTabOrder.GetCount(); i++)
		{
			if (m_lstTabOrder[i]->m_iOrder > iOrder)
				m_lstTabOrder[i]->m_iOrder--;
		}
		return TRUE;
	}

	SWND SBrowserTabCtrl::SwndFromPoint(CPoint ptHitTest, BOOL bOnlyText)
	{
		if (ptHitTest.x > m_pBtnNew->GetWindowRect().right)
			return NULL;
		return __super::SwndFromPoint(ptHitTest, bOnlyText);
	}

	bool SBrowserTabCtrl::OnBtnCloseTabClick(EventArgs *pEvt)
	{
		SWindow *pBtn = (SWindow*)pEvt->sender;
		SBrowserTab *pTab = (SBrowserTab*)pBtn->GetParent();

		EventBrowserTabClose evt(this);
		evt.pCloseTab = pTab;
		evt.iCloseTab = pTab->m_iOrder;
		evt.iCloseTabId = pTab->m_iTabId;
		FireEvent(evt);
		RemoveTab(pTab);
		if (m_lstTabOrder.GetCount() == 0)
		{
			PostMessage(GetContainer()->GetHostHwnd(), WM_CLOSE, 0, 0);
		}
		return true;
	}

	bool SBrowserTabCtrl::OnTabClick(EventArgs *pEvt)
	{
		SBrowserTab *pTab = (SBrowserTab*)pEvt->sender;
		SetCurSel(pTab->m_iOrder);
		return true;
	}

	BOOL SBrowserTabCtrl::InsertTabEx(SBrowserTab *pNewTab, STabPage *tabPage, int iPos)
	{
		SASSERT(pNewTab);
		//pNewTab->KillFocus();
		pNewTab->ModifyState(0, WndState_Normal, TRUE);
		InsertChild(pNewTab);
		pNewTab->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnTabClick, this));
		SWindow *closbtn = pNewTab->FindChildByName(Name_Btn_Close);
		if (closbtn)
			closbtn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnBtnCloseTabClick, this));
		//SBrowserTabCtrl *oldHost = pNewTab->m_pHost;
		pNewTab->SetHost(this);

		if (iPos < 0)
			iPos = m_lstTabOrder.GetCount();
		pNewTab->m_iOrder = iPos;
		pNewTab->m_iTabIndex = m_lstTabOrder.GetCount();
		for (int i = 0; i < (int)m_lstTabOrder.GetCount(); i++)
		{
			if (m_lstTabOrder[i]->m_iOrder >= iPos)
				m_lstTabOrder[i]->m_iOrder++;
		}
		m_lstTabOrder.InsertAt(iPos, pNewTab);
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcLeft;
		if (iPos > 0)
		{
			m_lstTabOrder[iPos - 1]->GetWindowRect(&rcLeft);
		}
		else
		{
			rcLeft = rcClient;
			rcLeft.right = rcLeft.left;
		}
		//插入到初始位置
		CRect rcInit = rcLeft;
		rcInit.left = rcInit.right;
		rcInit.right = rcInit.left + m_nDesiredSize;
		pNewTab->Move(rcInit);
		
		pNewTab->RegisterDragDrop();

		UpdateChildrenPosition();
		if (tabPage)
		{

			SRelTabCtrl *oldtabctrl = (SRelTabCtrl*)tabPage->GetParent();
			if (oldtabctrl == m_pRelTabCtrl)
			{//没有移动
				SetCurSel(iPos);
				return TRUE;
			}			
			if (oldtabctrl)
			{
				oldtabctrl->RemoveItemEx(tabPage);
			}
			m_pRelTabCtrl->InsertItemEx(tabPage);
		}
		//试一下原来的窗口是不是要关
		pNewTab->TryCloseHostWnd();
		SetCurSel(iPos);
		return TRUE;
	}

	BOOL SBrowserTabCtrl::InsertTab(LPCTSTR pszTitle, void * pData, int iPos)
	{
		SBrowserTab *pNewTab = new SBrowserTab(this);
		SASSERT(pNewTab);
		InsertChild(pNewTab);

		if (iPos < 0) iPos = m_lstTabOrder.GetCount();
		pNewTab->m_iOrder = iPos;
		pNewTab->m_iTabIndex = m_lstTabOrder.GetCount();

		for (int i = 0; i < (int)m_lstTabOrder.GetCount(); i++)
		{
			if (m_lstTabOrder[i]->m_iOrder >= iPos)
				m_lstTabOrder[i]->m_iOrder++;
		}
		pugi::xml_node xmlTabStyle = m_xmlStyle.child(KXmlTabStyle);
		if (xmlTabStyle)
			pNewTab->InitFromXml(xmlTabStyle);
		pNewTab->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnTabClick, this));
		m_lstTabOrder.InsertAt(iPos, pNewTab);

		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcLeft;
		if (iPos > 0)
		{
			m_lstTabOrder[iPos - 1]->GetWindowRect(&rcLeft);
		}
		else
		{
			rcLeft = rcClient;
			rcLeft.right = rcLeft.left;
		}
		
		pugi::xml_node xmlTitle = m_xmlStyle.child(KXmlTitleStyle);
		if (xmlTitle)
		{
			SWindow *pTitle = SApplication::getSingleton().CreateWindowByName(SStatic::GetClassName());
			pNewTab->InsertChild(pTitle);
			pTitle->InitFromXml(xmlTitle);
			pTitle->SetName(Name_Title_Lable);
			if (pszTitle)
				pNewTab->SetTitle(pszTitle);
		}
		pugi::xml_node xmlCloseBtn = m_xmlStyle.child(KXmlCloseBtnStyle);
		if (xmlCloseBtn && pNewTab->m_bAllowClose)
		{
			SWindow *pBtn = SApplication::getSingleton().CreateWindowByName(SImageButton::GetClassName());
			pNewTab->InsertChild(pBtn);
			pBtn->InitFromXml(xmlCloseBtn);
			pBtn->SetName(Name_Btn_Close);
			pBtn->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&SBrowserTabCtrl::OnBtnCloseTabClick, this));
		}

		//插入到初始位置
		CRect rcInit = rcLeft;
		rcInit.left = rcInit.right;
		rcInit.right = rcInit.left + m_nDesiredSize;
		pNewTab->Move(rcInit);

		UpdateChildrenPosition();

		EventBrowserTabNew evt(this);
		evt.pNewTab = pNewTab;
		evt.iNewTab = iPos;
		evt.pszTitle = pszTitle;
		evt.pData = pData;
		FireEvent(evt);
		SetCurSel(iPos);
		return TRUE;
	}

	void SBrowserTabCtrl::OnNextFrame()
	{
		for (UINT i = 0; i < m_lstTabOrder.GetCount(); i++)
		{
			m_lstTabOrder[i]->Update();
		}
		if (m_pBtnNew) m_pBtnNew->Update();
	}

	int SBrowserTabCtrl::OnCreate(LPVOID)
	{
		int nRet = __super::OnCreate(NULL);
		if (nRet == 0)
			GetContainer()->RegisterTimelineHandler(this);

		return nRet;
	}

	void SBrowserTabCtrl::OnDestroy()
	{
		GetContainer()->UnregisterTimelineHandler(this);
		__super::OnDestroy();
	}

	void SBrowserTabCtrl::SetCurSel(int iTab, bool bSendNotify)
	{
		SBrowserTabCtrl *abc = this;
		if (iTab >= (int)m_lstTabOrder.GetCount())
			return;
		int iCurSel = m_pSelTab ? m_pSelTab->m_iOrder : -1;
		if (iTab != iCurSel)
		{
			int oldSel = iCurSel;
			if (m_pSelTab)
			{
				m_pSelTab->ModifyState(0, WndState_Check, TRUE);
				m_pSelTab = NULL;
			}
			if (iTab != -1)
			{
				m_pSelTab = m_lstTabOrder[iTab];
				m_pSelTab->ModifyState(WndState_Check, 0, TRUE);
			}

			if (bSendNotify)
			{
				EventBrowserTabSelChanged evt(this);
				evt.iOldSel = oldSel;
				evt.iNewSel = iTab;

				FireEvent(evt);
			}
		}
	}

	int SBrowserTabCtrl::GetCurSel() const
	{
		return m_pSelTab ? m_pSelTab->m_iOrder : -1;
	}

	int SBrowserTabCtrl::GetSelTabId() const
	{
		return m_pSelTab ? m_pSelTab->m_iTabId : -1;
	}

	int SBrowserTabCtrl::GetTabIdByIndex(int iTabIndex) const
	{
		if (iTabIndex < (int)m_lstTabOrder.GetCount()){
			return m_lstTabOrder[iTabIndex]->m_iTabId;
		}
		return 0;
	}

	int SBrowserTabCtrl::GetTabIndexById(int iTabId) const
	{
		for (int i = 0; i < (int)m_lstTabOrder.GetCount(); i++)
		{
			if (m_lstTabOrder[i]->m_iTabId == iTabId)
				return i;
		}

		return 0;
	}

	int SBrowserTabCtrl::GetTabCount()const
	{
		return m_lstTabOrder.GetCount();
	}
	int SBrowserTabCtrl::GetTabIndex(int iTab) const
	{
		SASSERT(iTab >= 0 && iTab <= m_lstTabOrder.GetCount() - 1);
		return m_lstTabOrder[iTab]->m_iTabIndex;
	}

	int SBrowserTabCtrl::GetTabOrder(int iTabIndex) const
	{
		for (int i = 0; i < (int)m_lstTabOrder.GetCount(); i++)
		{
			if (m_lstTabOrder[i]->m_iTabIndex == iTabIndex) return i;
		}
		return -1;
	}

	BOOL SBrowserTabCtrl::UpdateTabTitle(int iTabId, LPCTSTR lpszTitle)
	{
		SBrowserTab *pSelTab = NULL;
		for (int i = 0; i < (int)m_lstTabOrder.GetCount(); i++)
		{
			if (m_lstTabOrder[i]->m_iTabId == iTabId)
				pSelTab = m_lstTabOrder[i];
		}
		if (pSelTab == NULL&&m_pSelTab != NULL)
			if (m_pCacheTab->m_iTabId == iTabId)
				pSelTab = m_pCacheTab;

		if (pSelTab == NULL)
			return FALSE;
		if (lpszTitle)
		{
			pSelTab->SetTitle(lpszTitle);
			pSelTab->SetAttribute(L"tip", S_CT2W(lpszTitle));
		}
		return TRUE;
	}
	
	void SBrowserTabCtrl::RegisterHost(SHostWindowFactory & wndFactory)
	{
		m_HostFactory = wndFactory.Clone();
	}
	SHostWindowFactory *SBrowserTabCtrl::GetHostFactory()
	{
		return m_HostFactory;
	}
}