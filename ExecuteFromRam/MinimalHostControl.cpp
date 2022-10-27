#include <mscoree.h>
#include "MinimalHostControl.h"

MinimalHostControl::MinimalHostControl()
{
	m_refCount = 0;
	m_defaultDomainManager = NULL;
}

MinimalHostControl::~MinimalHostControl()
{
	if (m_defaultDomainManager != nullptr)
	{
		m_defaultDomainManager->Release();
	}
}

ICustomAppDomainManager* MinimalHostControl::GetDomainManagerForDefaultDomain()
{
	if (m_defaultDomainManager)
	{
		m_defaultDomainManager->AddRef();
	}
	return m_defaultDomainManager;
}

HRESULT MinimalHostControl::GetHostManager(REFIID riid, void** ppv)
{
	*ppv = NULL;
	return E_NOINTERFACE;
}

HRESULT MinimalHostControl::SetAppDomainManager(DWORD dwAppDomainID, IUnknown* pUnkAppDomainManager)
{
	HRESULT hr = S_OK;
	hr = pUnkAppDomainManager->QueryInterface(__uuidof(ICustomAppDomainManager), (PVOID*)&m_defaultDomainManager);
	return hr;
}
HRESULT MinimalHostControl::QueryInterface(const IID& iid, void** ppv)
{
	if (!ppv) return E_POINTER;
	*ppv = this;
	AddRef();
	return S_OK;
}
ULONG MinimalHostControl::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}
ULONG MinimalHostControl::Release()
{
	if (InterlockedDecrement(&m_refCount) == 0)
	{
		delete this;
		return 0;
	}
	return m_refCount;
}