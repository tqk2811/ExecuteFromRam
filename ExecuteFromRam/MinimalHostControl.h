#pragma once

class MinimalHostControl : public IHostControl
{
public:
    MinimalHostControl()
    {
        m_refCount = 0;
        m_defaultDomainManager = NULL;
    }
    virtual ~MinimalHostControl()
    {
        if (m_defaultDomainManager != nullptr)
        {
            m_defaultDomainManager->Release();
        }
    }
    ICustomAppDomainManager* GetDomainManagerForDefaultDomain()
    {
        if (m_defaultDomainManager)
        {
            m_defaultDomainManager->AddRef();
        }
        return m_defaultDomainManager;
    }
    HRESULT STDMETHODCALLTYPE GetHostManager(REFIID riid, void** ppv)
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE SetAppDomainManager(DWORD dwAppDomainID, IUnknown* pUnkAppDomainManager)
    {
        HRESULT hr = S_OK;
        hr = pUnkAppDomainManager->QueryInterface(__uuidof(ICustomAppDomainManager), (PVOID*)&m_defaultDomainManager);
        return hr;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID& iid, void** ppv)
    {
        if (!ppv) return E_POINTER;
        *ppv = this;
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&m_refCount);
    }
    ULONG STDMETHODCALLTYPE Release()
    {
        if (InterlockedDecrement(&m_refCount) == 0)
        {
            delete this;
            return 0;
        }
        return m_refCount;
    }
private:
    long m_refCount;
    ICustomAppDomainManager* m_defaultDomainManager;
};