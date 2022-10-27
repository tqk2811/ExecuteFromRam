#ifndef MinimalHostControl_H
#define MinimalHostControl_H

#import "MyAppDomainManager.tlb"
using namespace MyAppDomainManager;

class MinimalHostControl : public IHostControl
{
public:
	MinimalHostControl();
	virtual ~MinimalHostControl();
	ICustomAppDomainManager* GetDomainManagerForDefaultDomain();
	HRESULT STDMETHODCALLTYPE GetHostManager(REFIID riid, void** ppv);

	HRESULT STDMETHODCALLTYPE SetAppDomainManager(DWORD dwAppDomainID, IUnknown* pUnkAppDomainManager);
	HRESULT STDMETHODCALLTYPE QueryInterface(const IID& iid, void** ppv);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
private:
	long m_refCount;
	ICustomAppDomainManager* m_defaultDomainManager;
};

#endif