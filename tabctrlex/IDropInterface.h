#pragma once
class CIDropSource :public IDropSource
{
private:
	volatile LONG m_lRefCount;
public:
	CIDropSource();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);

	ULONG   STDMETHODCALLTYPE AddRef(void);

	ULONG   STDMETHODCALLTYPE Release(void);
};
class CIDataObject :public IDataObject
{
private:
	volatile LONG m_lRefCount;
public:
	CIDataObject();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);

	ULONG   STDMETHODCALLTYPE AddRef(void);

	ULONG   STDMETHODCALLTYPE Release(void);
};
class CIDropTarget :public IDropTarget
{
private:
	volatile LONG m_lRefCount;
public:
	CIDropTarget();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);

	ULONG   STDMETHODCALLTYPE AddRef(void);

	ULONG   STDMETHODCALLTYPE Release(void);
};