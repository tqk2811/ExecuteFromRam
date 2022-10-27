#include <Windows.h>
#include <metahost.h>
#include <mscoree.h>
#pragma comment(lib, "mscoree.lib")

#include <string>
#include <assert.h>
#define CheckHr(hr)assert(hr >= 0);


//#import "mscorlib.tlb" raw_interfaces_only\
//            high_property_prefixes("_get","_put","_putref")\
//            rename("ReportEvent", "InteropServices_ReportEvent")

#include "MinimalHostControl.h"

#include "ClrHelper.h"

//C:\Windows\Microsoft.NET\Framework
//C:\Windows\Microsoft.NET\Framework64
const LPCWSTR RuntimeVersion = L"v4.0.30319";
//https://stackoverflow.com/a/24136074/5034139 public key token
const LPCWSTR domainAssemblyName = L"MyAppDomainManager, Culture=neutral, PublicKeyToken=";
const LPCWSTR domainTypename = L"MyAppDomainManager.CustomAppDomainManager";

//https://www.codeproject.com/Articles/1236146/Protecting-NET-plus-Application-By-Cplusplus-Unman
//https://www.codeproject.com/Articles/416471/CLR-Hosting-Customizing-the-CLR
//https://www.codeproject.com/Articles/418259/CLR-Hosting-Customizing-the-CLR-Part-2

ICLRMetaHost* pMetaHost = NULL;
ICLRRuntimeInfo* pRuntimeInfo = NULL;
ICLRRuntimeHost* pRuntimeHost = NULL;
ICLRControl* pCLRControl = NULL;
MinimalHostControl* pMyHostControl = NULL;


SAFEARRAY* GenBinary(const BYTE* pImage, ULONG assembyLength)
{
    SAFEARRAYBOUND rgsabound;
    rgsabound.cElements = assembyLength;
    rgsabound.lLbound = 0;
    SAFEARRAY* pSafeArray = SafeArrayCreate(VT_UI1, 1, &rgsabound);
    if (pSafeArray == nullptr) return nullptr;

    void* pvData = NULL;
    HRESULT hr = SafeArrayAccessData(pSafeArray, &pvData);
    CheckHr(hr);
    memcpy(pvData, pImage, assembyLength);
    hr = SafeArrayUnaccessData(pSafeArray);
    CheckHr(hr);
    return pSafeArray;
}

SAFEARRAY* GenArg(const LPWSTR* args, const int argc, const int argsIgnoreCount)
{
    if (argc <= argsIgnoreCount) return nullptr;

    HRESULT hr;
    SAFEARRAYBOUND rgsabound;
    rgsabound.cElements = argc - argsIgnoreCount;
    rgsabound.lLbound = 0;
    SAFEARRAY* pSafeArray = SafeArrayCreate(VT_BSTR, 1, &rgsabound);
    if (pSafeArray == nullptr) return nullptr;

    long index = 0;
    for (int i = argsIgnoreCount; i < argc; i++)
    {
        BSTR pData = SysAllocString(args[i]);
        long rgIndicies = index;
        hr = SafeArrayPutElement(pSafeArray, &rgIndicies, pData);
        CheckHr(hr);
        index++;
    }

    return pSafeArray;
}

void InitClrHost()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);//COM init
    CheckHr(hr);
    hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);//Install MetaHost
    CheckHr(hr);
    hr = pMetaHost->GetRuntime(RuntimeVersion, IID_PPV_ARGS(&pRuntimeInfo));//Install .NET Runtime
    CheckHr(hr);
    hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pRuntimeHost));
    CheckHr(hr);

    hr = pRuntimeHost->GetCLRControl(&pCLRControl);
    CheckHr(hr);

    pMyHostControl = new MinimalHostControl();
    hr = pRuntimeHost->SetHostControl(pMyHostControl);
    CheckHr(hr);


    std::wstring asmName(domainAssemblyName);
    //public key token for ensure MyAppDomainManager not fake
    asmName.append(L"e7ab");
    asmName.append(L"a3ea");
    asmName.append(L"9a93");
    asmName.append(L"2f01");
    hr = pCLRControl->SetAppDomainManagerType(asmName.c_str(), domainTypename);
    CheckHr(hr);

    hr = pRuntimeHost->Start();//Start CLR host
    CheckHr(hr);
}

int RunFromMemory(SAFEARRAY* binary, SAFEARRAY* args)
{
    ICustomAppDomainManager* pAppDomainManager = pMyHostControl->GetDomainManagerForDefaultDomain();

    HRESULT hr = pAppDomainManager->Run(binary, args);
    CheckHr(hr);

    HRESULT hr_ = pRuntimeHost->Stop();
    CheckHr(hr_);

    return hr_;
}