#pragma once

#include <core/Swnd.h>
#include <control/Stabctrl.h>
#include <core/shostwnd.h>
#include "SAnimator.h"
#include "SRelTabCtrl.h"

namespace SOUI
{

#define EVT_WEBTAB_BEGIN (EVT_EXTERNAL_BEGIN + 400)
#define EVT_WEBTAB_NEW           (EVT_WEBTAB_BEGIN + 0)
#define EVT_WEBTAB_CLOSE         (EVT_WEBTAB_BEGIN + 1)
#define EVT_WEBTAB_SELCHANGED    (EVT_WEBTAB_BEGIN + 2)
#define EVT_WEBTAB_GETICON   (EVT_WEBTAB_BEGIN + 3)//这个图标暂不用，因为可以和EventBrowserTabNew共用
	class EventBrowserTabNew : public TplEventArgs < EventBrowserTabNew >
	{
		SOUI_CLASS_NAME(EventBrowserTabNew, L"on_webtab_new")
	public:
		EventBrowserTabNew(SWindow *pSender) :TplEventArgs<EventBrowserTabNew>(pSender)
		{

		}
		enum{ EventID = EVT_WEBTAB_NEW };
		SWindow * pNewTab;
		LPCTSTR		pszTitle;
		void *		pData;
		int       iNewTab;
	};
	class EventBrowserTabGetIcon : public TplEventArgs < EventBrowserTabGetIcon >
	{
		SOUI_CLASS_NAME(EventBrowserTabGetIcon, L"on_webtab_getico")
	public:
		EventBrowserTabGetIcon(SWindow *pSender) :TplEventArgs<EventBrowserTabGetIcon>(pSender)
		{

		}
		enum {
			EventID = EVT_WEBTAB_GETICON
		};
		SWindow * pTab;
	};

	class EventBrowserTabClose : public TplEventArgs < EventBrowserTabClose >
	{
		SOUI_CLASS_NAME(EventBrowserTabClose, L"on_webtab_close")
	public:
		EventBrowserTabClose(SWindow *pSender) :TplEventArgs<EventBrowserTabClose>(pSender)//, bClose(true)
		{

		}
		enum{ EventID = EVT_WEBTAB_CLOSE };

		SWindow * pCloseTab;
		int       iCloseTab;
		int       iCloseTabId;
		//bool	bClose;
	};

	class EventBrowserTabSelChanged : public TplEventArgs < EventBrowserTabSelChanged >
	{
		SOUI_CLASS_NAME(EventBrowserTabSelChanged, L"on_webtab_sel_changed")
	public:
		EventBrowserTabSelChanged(SWindow *pSender) :TplEventArgs<EventBrowserTabSelChanged>(pSender)
		{

		}
		enum{ EventID = EVT_WEBTAB_SELCHANGED };

		int         iOldSel;
		int         iNewSel;
	};

	class SBrowserTab;

	struct IHostInterface
	{
		virtual void SetAutoNewCreateTab(bool)=0;
		virtual void InsertTab(SBrowserTab * tab, STabPage * page) = 0;
		virtual bool CreateHostWnd() = 0;
		virtual void InitDialog() = 0;
		virtual void SetPos(CPoint &) = 0;
	};

	class SHostWindowFactory
	{
	public:
		virtual ~SHostWindowFactory() {}
		virtual IHostInterface* NewWindow() = 0;
		virtual SHostWindowFactory* Clone() const = 0;
	};

	template <typename T>
	class SHostFactory :public SHostWindowFactory
	{
	public:
		//! Default constructor.
		SHostFactory()
		{}
		// Implement WindowFactory interface
		virtual IHostInterface* NewWindow()
		{
			return new T;
		}

		virtual SHostWindowFactory* Clone() const
		{
			return new SHostFactory();
		}
	};

	class SBrowserTabCtrl : public SWindow, public ITimelineHandler
	{
		SOUI_CLASS_NAME(SBrowserTabCtrl, L"BrowserTabCtrl")
			friend class SBrowserTab;
	public:
		enum TABDIR{
			TDIR_HORZ,
			TDIR_VERT,
		};		
		void RegisterHost(SHostWindowFactory & wndFactory);
		SHostWindowFactory *GetHostFactory();
		SBrowserTabCtrl(void);
		~SBrowserTabCtrl(void);
		void SetTabCtrl(SRelTabCtrl* tab);
		STabPage* SBrowserTabCtrl::GetPage(int id);
		void RmovePage(int);
		BOOL InsertTab(LPCTSTR pszTitle, void * pData, int iPos = -1);
		BOOL InsertTabEx(SBrowserTab * pNewTab, STabPage *tabPage, int iPos = -1);
		BOOL RemoveTab(int iTab);
		BOOL RemoveTab(SBrowserTab *pTab, bool bRelease = true);
		void SetCurSel(int iTab, bool bSendNotify = true);
		int GetCurSel() const;
		int GetSelTabId() const;
		//void RegisterDragDrop();
		void CloseTab(int tabid);
		void CloseTabExcept(int tabid);
		void CloseAllTab();
		void ClosePre(int tabid);
		void CloseNext(int tabid);
		bool CanClosePre(int tabid);
		bool CanCloseNext(int tabid);
		int GetTabIdByIndex(int iTabIndex) const;
		int GetTabIndexById(int iTabId) const;
		int GetTabIndex(int iTab) const;
		int GetTabOrder(int iTabIndex) const;
		int GetTabCount()const;
		//更新选项卡标题
		BOOL UpdateTabTitle(int iTabId, LPCTSTR lpszTitle);
	protected:
		int ChangeTabPos(SBrowserTab* pCurMove, CPoint ptCur);
		virtual SWND SwndFromPoint(CPoint ptHitTest, BOOL bOnlyText);
		virtual BOOL CreateChildren(pugi::xml_node xmlNode);
		virtual void UpdateChildrenPosition();
		virtual void OnNextFrame();
		bool OnBtnNewClick(EventArgs *pEvt);
		bool OnBtnCloseTabClick(EventArgs *pEvt);
		bool OnTabClick(EventArgs *pEvt);
		int OnCreate(LPVOID);
		void OnDestroy();

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
			MSG_WM_DESTROY(OnDestroy)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"tabDesiredSize", m_nDesiredSize, FALSE)
			ATTR_INT(L"enableDrag", m_bEnableDrag, FALSE)
			ATTR_ENUM_BEGIN(L"tabAlign", TABDIR, FALSE)
			ATTR_ENUM_VALUE(L"vertical", TDIR_VERT)
			ATTR_ENUM_VALUE(L"horizontal", TDIR_HORZ)
			ATTR_ENUM_END(m_tabAlign)
		SOUI_ATTRS_END()

		int     m_nDesiredSize;
		TABDIR  m_tabAlign;
		BOOL    m_bEnableDrag;

		SArray<SBrowserTab*> m_lstTabOrder;
		SBrowserTab*		m_pCacheTab;
		SBrowserTab *        m_pBtnNew;
		SBrowserTab *        m_pSelTab;
		//真实的tab
		SRelTabCtrl*	m_pRelTabCtrl;
		SScrollView*	m_pTabContainer;

		pugi::xml_document  m_xmlStyle;
		SHostWindowFactory *m_HostFactory;
		int    m_nTabCount;		
	};
	class SPrintWindow : public SWindow
	{
		SOUI_CLASS_NAME(SPrintWindow, L"SPrintWindow")
	public:
		SPrintWindow(SWindow *pHostCtrl);
		//更新
		void Update();
		void Print(IRenderTarget *pRT, CRect &rcWindow);
	protected:
		CAutoRefPtr<IRenderTarget> m_memRT;
		SWindow *                 m_pHostCtrl;
	};
	//////////////////////////////////////////////////////////////////////////
	// SBrowserTab
	class SBrowserTab : public SWindow, public SAnimator
	{
		SOUI_CLASS_NAME(SBrowserTab, L"BrowserTab")
			friend class SBrowserTabCtrl;
	public:
		SBrowserTab(SBrowserTabCtrl* pHost);
		void MoveTo(const CRect & rcEnd);
		BOOL IsDragable() { return m_iOrder != -1 && m_pHost->m_bEnableDrag; }
		
		int GetTabID()
		{
			return m_iTabId;
		}
		//void Insert(SWND tab);
		void RemoveMe();
		void CloseMe();
		void CloseOther();
		void ClosePre();
		void CloseNext();
		void CanClosePre();
		void CanCloseNext();
		void SegregateMe();
		void TryCloseHostWnd();
		void Insert(SBrowserTab * tabWnd, STabPage *, bool bFront);
		//void Insert(SBrowserTab * tab);
		void SetHost(SBrowserTabCtrl *pNewHost)
		{
			m_pHost = pNewHost;
		}
		void SetTitle(LPCTSTR szTitle, LPCTSTR szTip = NULL);
		HRESULT SetTitle(SStringT szVaule, BOOL bLoading);
		int OnCreate(LPVOID);
		BOOL RegisterDragDrop();
		BOOL RevokeDragDrop();
		void OnDestroy();
		void ReleaseOldCapture();
		void OnSize(UINT nType, CSize size);
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"allowClose", m_bAllowClose, TRUE)
			ATTR_CUSTOM(L"title", SetTitle);
		SOUI_ATTRS_END()

			SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
			MSG_WM_DESTROY(OnDestroy)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			MSG_WM_SIZE(OnSize)
			SOUI_MSG_MAP_END()

	protected:
		virtual void OnAnimatorState(int percent);
		virtual void OnFinalRelease() { delete this; }
		virtual void OnStateChanged(DWORD dwOldState, DWORD dwNewState)
		{
			__super::OnStateChanged(dwOldState, dwNewState);
			SImageButton *closeBtn = FindChildByName2<SImageButton>(L"close");
			if (closeBtn)
			{
				if ((dwNewState != 0) && !closeBtn->IsVisible())
					closeBtn->SetVisible(TRUE, TRUE);
				else if ((dwNewState == 0) && closeBtn->IsVisible())
					closeBtn->SetVisible(FALSE, TRUE);
			}
		}
		void OnMouseMove(UINT nFlags, CPoint pt);
		void OnLButtonUp(UINT nFlags, CPoint pt);
		void OnMouseLeave();
		void OnLButtonDown(UINT nFlags, CPoint pt);

		CRect m_rcBegin, m_rcEnd;
		BOOL    m_bAllowClose;
		CPoint  m_ptDrag, m_ptDes;
		int     m_iOrder;
		int     m_iTabIndex;
		int		m_iTabId;
		bool    m_bDrag;
		ISwndContainer *m_pcontainer;
		SPrintWindow *m_pPrintWindow;
		//标明是否是加载状态
		bool m_bLoading;
		bool m_bCloseHostWnd;
		HWND m_hHostWnd;
	public:
		SBrowserTabCtrl* m_pHost;
	private:
		CRect m_ParentWinrc;
	};
}
	
