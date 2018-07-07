#include "stdafx.h"
#include "IDropInterface.h"
#include "TargetData.h"
#include <ShlObj.h>
#include "DropWindow.h"
#include "SBrowserTab.h"
#include "core\shostwnd.h"
#include "DropTarget.h"

	void CDropSources::UpdateWindowLocation()
	{
		if (m_dropwindow)
		{
			CPoint curPos;
			GetCursorPos(&curPos);
			m_dropwindow->SetWindowPos(NULL, curPos.x- m_offset_pos.x, curPos.y- m_offset_pos.y, 0, 0, SWP_NOSIZE);			
		}
	}
	
	CDropSources::CDropSources(SBrowserTab *tab, SPrintWindow *printwnd, CPoint*pos, CDropWindow *dropwindow) :m_tabWnd(tab), m_dropwindow(dropwindow), m_offset_pos(*pos)
	{
		if (tab&&dropwindow)
		{
			tab->AddRef();
			m_tabrc = m_tabWnd->GetWindowRect();
			m_dropwindow->MoveWindow(0, 0, m_tabrc.Width(), m_tabrc.Height());
			m_dropwindow->CopyDropWindBkgndFormWindow(printwnd);
			m_printwnd = printwnd;
		}
	}
	CDropSources::~CDropSources(){
		if (m_tabWnd)
			m_tabWnd->Release();
	}
	HRESULT  CDropSources::QueryContinueDrag(
		/* [annotation][in] */
		_In_  BOOL fEscapePressed,
		/* [annotation][in] */
		_In_  DWORD grfKeyState)
	{
		m_dropwindow->CopyDropWindBkgndFormWindow(m_printwnd);
		UpdateWindowLocation();
		if ((grfKeyState & MK_LBUTTON) == 0||m_tabWnd->m_pHost!=NULL)
		return DRAGDROP_S_DROP;
		// continue with the drag-drop		
		return S_OK;
	}

	HRESULT  CDropSources::GiveFeedback(
		/* [annotation][in] */
		_In_  DWORD dwEffect)
	{		
		//不改变鼠标形状
		return S_OK;
	}

	HGLOBAL CDataObjecto::DupMem(HGLOBAL hMem)
	{
		// lock the source memory object
		DWORD   len = GlobalSize(hMem);
		PVOID   source = GlobalLock(hMem);
		// create a fixed "global" block - i.e. just
		// a regular lump of our process heap
		PVOID   dest = GlobalAlloc(GMEM_FIXED, len);
		memcpy(dest, source, len);
		GlobalUnlock(hMem);
		return dest;
	}
	HRESULT  CDataObjecto::GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
	{
		int idx;
		if ((idx = LookupFormatEtc(pFormatEtc)) == -1)
		{
			return DV_E_FORMATETC;
		}
		pMedium->tymed = m_pFormatEtc[idx].tymed;
		pMedium->pUnkForRelease = 0;
		switch (m_pFormatEtc[idx].tymed)
		{
		case TYMED_HGLOBAL:
			pMedium->hGlobal = DupMem(m_pStgMedium[idx].hGlobal);
			break;
		default:
			return DV_E_FORMATETC;

		}
		return S_OK;
	}

	HRESULT  CDataObjecto::GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
	{
		return DATA_E_FORMATETC;
	}

	HRESULT  CDataObjecto::QueryGetData(FORMATETC *pFormatEtc)
	{
		return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
	}

	HRESULT  CDataObjecto::GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
	{
		pFormatEtcOut->ptd = NULL;

		return E_NOTIMPL;
	}

	HRESULT  CDataObjecto::SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease)
	{
		return E_NOTIMPL;
	}

	HRESULT  CDataObjecto::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
	{
		if (NULL == ppEnumFormatEtc)
		{
			return E_INVALIDARG;
		}
		*ppEnumFormatEtc = NULL;
		HRESULT hr = E_NOTIMPL;

		if (dwDirection == DATADIR_GET)
		{			
			 hr=SHCreateStdEnumFmtEtc(m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc);
		}
		return hr;
	}

	HRESULT  CDataObjecto::DAdvise(FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}

	HRESULT  CDataObjecto::DUnadvise(DWORD      dwConnection)
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}

	HRESULT  CDataObjecto::EnumDAdvise(IEnumSTATDATA **ppEnumAdvise)
	{
		return OLE_E_ADVISENOTSUPPORTED;
	}
	//我只使用1；
	CDataObjecto::CDataObjecto(FORMATETC *fmt, STGMEDIUM *stgmed, int count)
	{		
		m_pFormatEtc = fmt;
		m_nNumFormats = count;
		m_pStgMedium = stgmed;
	}
	CDataObjecto::~CDataObjecto()
	{
		if (m_pFormatEtc) delete m_pFormatEtc;
		if (m_pStgMedium) delete m_pStgMedium;
	}
	
	int CDataObjecto::LookupFormatEtc(FORMATETC *pFormatEtc)
	{
		if (pFormatEtc->cfFormat==m_pFormatEtc->cfFormat
			&&pFormatEtc->tymed==m_pFormatEtc->tymed
			&&pFormatEtc->dwAspect==m_pFormatEtc->dwAspect)
			return 0;
		else
			return -1;
	}


	CDropTarget::CDropTarget(SBrowserTab *pBrowserCtrl) :m_pBrowserCtrl(pBrowserCtrl), tabWid(0)
	{
		if (pBrowserCtrl)
		{
			pBrowserCtrl->AddRef();
			tabWid = pBrowserCtrl->GetWindowRect().Width();
		}
	}
	CDropTarget::~CDropTarget()
	{
		if (m_pBrowserCtrl)
			m_pBrowserCtrl->Release();
	}
	 HRESULT  CDropTarget::DragEnter(
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect)
	{
		FORMATETC format =
		{
			CF_MAX+1, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL
		};
		STGMEDIUM medium;
		if (FAILED(pDataObj->GetData(&format, &medium)))
		{
			return S_FALSE;
		}

		TargetData * pTargetData = static_cast<TargetData *>(GlobalLock(medium.hGlobal));
		if (!pTargetData)
		{
			return S_FALSE;
		} 
		*pdwEffect = DROPEFFECT_LINK;

		bool bFront = tabWid == 0 ? true : (tabWid/2>pt.x?false:true);
		m_pBrowserCtrl->Insert(pTargetData->tabWnd,pTargetData->page, bFront);
		GlobalUnlock(medium.hGlobal);
		return S_OK;
	}

	 HRESULT  CDropTarget::DragOver(
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect)
	{
		*pdwEffect = DROPEFFECT_LINK;
		return S_OK;
	}

	 HRESULT  CDropTarget::DragLeave(void)
	{		
		/*if (m_pBrowserCtrlAdd==NULL)
		{
			return S_FALSE;
		}
		m_pBrowserCtrlAdd->ReMoveMe();*/
		return S_OK;
	}
	 HRESULT  CDropTarget::Drop(
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect)
	{				
		*pdwEffect = DROPEFFECT_LINK;
		return S_OK;
	}

	CDropHelper::CDropHelper() {}
	HRESULT CDropHelper::CreateDropSource(SBrowserTab* tab, SPrintWindow *printwnd, CDropWindow *dropwindow, CPoint *pos, IDropSource **ppDropSource)
	{
		if (ppDropSource == 0)
			return E_INVALIDARG;
		*ppDropSource = new CDropSources(tab, printwnd, pos, dropwindow);
		return (*ppDropSource) ? S_OK : E_OUTOFMEMORY;
	}
	HRESULT CDropHelper::CreateDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, IDataObject **ppDataObject)
	{
		if (ppDataObject == 0)
			return E_INVALIDARG;
		*ppDataObject = new CDataObjecto(fmtetc, stgmeds, count);
		return (*ppDataObject) ? S_OK : E_OUTOFMEMORY;
	}
	void CDropHelper::UpdateWindowLocation(CDropWindow *dropwindow, int offset_x, int offset_y)
	{
		if (dropwindow)
		{
			CPoint curPos;
			GetCursorPos(&curPos);
			dropwindow->SetWindowPos(NULL, curPos.x- offset_x, curPos.y- offset_y, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
		}
	}
	void CDropHelper::StartDrop(TargetData &data)
	{
		IDataObject *pDataObject;
		IDropSource *pDropSource;
		DWORD        dwEffect= DROPEFFECT_NONE;
		DWORD        dwResult;

		{
			FORMATETC fmtetc = { CF_MAX + 1, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM stgmed = { TYMED_HGLOBAL,{ 0 }, 0 };
			//stgmed.hGlobal = CopySelection(hwndEdit);
			// Create IDataObject and IDropSource COM objects
			stgmed.hGlobal = ::GlobalAlloc(GMEM_FIXED, sizeof(TargetData)); //得到handle
			TargetData *p = (TargetData *)::GlobalLock(stgmed.hGlobal); //从handle得到指针
			memcpy(p, &data, sizeof(TargetData));
			::GlobalUnlock(stgmed.hGlobal);
			//这个窗口有问题
			CDropWindow DrapWnd;
			DrapWnd.Create(NULL);
			//
			DrapWnd.CopyDropWindBkgndFormWindow(data.printwnd);
			DrapWnd.SendMessage(WM_INITDIALOG, (WPARAM)DrapWnd.m_hWnd);
			CreateDropSource(data.tabWnd, data.printwnd, &DrapWnd, &data.Pos, &pDropSource);			
			CreateDataObject(&fmtetc, &stgmed, 1, &pDataObject);
			UpdateWindowLocation(&DrapWnd, data.Pos.x, data.Pos.y);
			
			dwResult = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_LINK, &dwEffect);
			DrapWnd.close();
			::GlobalFree(p);
			pDataObject->Release();
			pDropSource->Release();
		}
		DWORD *p0 = &dwEffect;
		
		if (dwResult == DRAGDROP_S_DROP)
		{	
			//创建一个新窗口把它插入进去
			if (dwEffect== DROPEFFECT_NONE)
			{
				IHostInterface *ihostwnd=data.hostfactory->NewWindow();
				bool H = ihostwnd->CreateHostWnd();
				ihostwnd->SetAutoNewCreateTab(false);
				ihostwnd->InitDialog();
				data.tabWnd->ReleaseCapture();
				ihostwnd->InsertTab(data.tabWnd, data.page);
				CPoint curPos;
				GetCursorPos(&curPos);
				ihostwnd->SetPos(curPos);
			}
			//合并多tab
			else if (DROPEFFECT_LINK == dwEffect)
			{
				data.tabWnd->ReleaseOldCapture();
			}			
		}		
	}
