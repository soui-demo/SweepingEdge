#include "stdafx.h"
#include "IDropInterface.h"

CIDropSource::CIDropSource() :m_lRefCount(1) {}
HRESULT STDMETHODCALLTYPE CIDropSource::QueryInterface(REFIID iid, void ** ppvObject)
	{
		if (iid == __uuidof(IDropSource) || iid == __uuidof(IUnknown))
		{
			AddRef();
			*ppvObject = this;
			return S_OK;
		}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

ULONG   STDMETHODCALLTYPE CIDropSource::AddRef(void)
	{
		return InterlockedIncrement(&m_lRefCount);
	}

ULONG   STDMETHODCALLTYPE CIDropSource::Release(void)
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&m_lRefCount);
		if (count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}

	CIDataObject::CIDataObject() :m_lRefCount(1) {}
	HRESULT STDMETHODCALLTYPE CIDataObject::QueryInterface(REFIID iid, void ** ppvObject)
	{
		if (iid == __uuidof(IDataObject) || iid == __uuidof(IUnknown))
		{
			AddRef();
			*ppvObject = this;
			return S_OK;
		}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	ULONG   STDMETHODCALLTYPE CIDataObject::AddRef(void)
	{
		return InterlockedIncrement(&m_lRefCount);
	}

	ULONG   STDMETHODCALLTYPE CIDataObject::Release(void)
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&m_lRefCount);
		if (count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}
	CIDropTarget::CIDropTarget() :m_lRefCount(1){}
	HRESULT STDMETHODCALLTYPE CIDropTarget::QueryInterface(REFIID iid, void ** ppvObject)
	{
		if (iid == __uuidof(IDropTarget) || iid == __uuidof(IUnknown))
		{
			AddRef();
			*ppvObject = this;
			return S_OK;
		}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	ULONG   STDMETHODCALLTYPE CIDropTarget::AddRef(void)
	{
		return InterlockedIncrement(&m_lRefCount);
	}

	ULONG   STDMETHODCALLTYPE CIDropTarget::Release(void)
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&m_lRefCount);
		if (count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}