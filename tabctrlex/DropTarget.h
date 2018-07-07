#pragma once
#include "SBrowserTab.h"
#include "TargetData.h"
#include "DropWindow.h"
#include "IDropInterface.h"
class CDropSources :public CIDropSource
{
	SBrowserTab *m_tabWnd;
	CRect m_tabrc;
	void UpdateWindowLocation();
	CDropWindow *m_dropwindow;
	CPoint m_offset_pos;
	SPrintWindow *m_printwnd;
public:
	CDropSources(SBrowserTab *tab, SPrintWindow *printwnd, CPoint*pos, CDropWindow *dropwindow);
	~CDropSources();
public:
	virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(
		/* [annotation][in] */
		_In_  BOOL fEscapePressed,
		/* [annotation][in] */
		_In_  DWORD grfKeyState);

	virtual HRESULT STDMETHODCALLTYPE GiveFeedback(
		/* [annotation][in] */
		_In_  DWORD dwEffect);
private:
};

class CDataObjecto : public CIDataObject
{
	HGLOBAL DupMem(HGLOBAL hMem);
public:
	HRESULT STDMETHODCALLTYPE GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium);

	HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pMedium);

	HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC *pFormatEtc);

	HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut);

	HRESULT STDMETHODCALLTYPE SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease);
	HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc);

	HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);

	HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection);

	HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA **ppEnumAdvise);
	//我只使用1；
	CDataObjecto(FORMATETC *fmt, STGMEDIUM *stgmed, int count = 1);
	~CDataObjecto();

private:
	int LookupFormatEtc(FORMATETC *pFormatEtc);
	FORMATETC *m_pFormatEtc;
	STGMEDIUM *m_pStgMedium;
	LONG       m_nNumFormats;
};

class CDropTarget : public CIDropTarget
{
protected:
	SBrowserTab *m_pBrowserCtrl;
	int tabWid;
public:
	CDropTarget(SBrowserTab *pBrowserCtrl);
	~CDropTarget();
public:
	virtual HRESULT STDMETHODCALLTYPE DragEnter(
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect);

	virtual HRESULT STDMETHODCALLTYPE DragOver(
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect);

	virtual HRESULT STDMETHODCALLTYPE DragLeave(void);
	virtual HRESULT STDMETHODCALLTYPE Drop(
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect);
};

class CDropHelper
{	private:
	CDropHelper();
		static HRESULT CreateDropSource(SBrowserTab* tab, SPrintWindow *printwnd, CDropWindow *dropwindow, CPoint *pos, IDropSource **ppDropSource);
		static HRESULT CreateDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, IDataObject **ppDataObject);
		static void UpdateWindowLocation(CDropWindow *dropwindow, int offset_x, int offset_y);
public:
	static void StartDrop(TargetData &data);
};