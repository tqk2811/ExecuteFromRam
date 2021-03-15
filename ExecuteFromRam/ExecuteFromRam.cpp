#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cassert>
#include <fstream>
#include <Windows.h>
#include <metahost.h>
#include <vector>
#include <assert.h>

#define CheckHr(hr)assert(hr >= 0);

#pragma comment(lib, "mscoree.lib")
#import "mscorlib.tlb" raw_interfaces_only\
            high_property_prefixes("_get","_put","_putref")\
            rename("ReportEvent", "InteropServices_ReportEvent")

using namespace mscorlib;
LPCWSTR DotNetVersion = L"v4.0.30319";
//LPCWSTR DotNetVersion = L"v4.8.04084";



//https://www.codeproject.com/Articles/1236146/Protecting-NET-plus-Application-By-Cplusplus-Unman

//Install CLRHost, MetaHost and Instances
ICLRMetaHost* pMetaHost = NULL;  /// Metahost installed.
ICLRMetaHostPolicy* pMetaHostPolicy = NULL;  /// Metahost Policy installed.
ICLRDebugging* pCLRDebugging = NULL;  /// Metahost Debugging installed.
//Install .NET Runtime
ICLRRuntimeInfo* pRuntimeInfo = NULL;
//Start and Load CLRApp
ICorRuntimeHost* pRuntimeHost = NULL;

void Init()
{
    //Install WPF Mode
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    //Install CLRHost, MetaHostand Instances
    HRESULT hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost,(LPVOID*)&pMetaHost);
    CheckHr(hr);
    hr = CLRCreateInstance(CLSID_CLRMetaHostPolicy, IID_ICLRMetaHostPolicy,(LPVOID*)&pMetaHostPolicy);
    CheckHr(hr);
    hr = CLRCreateInstance(CLSID_CLRDebugging, IID_ICLRDebugging,(LPVOID*)&pCLRDebugging);
    CheckHr(hr);

    //Install .NET Runtime
    hr = pMetaHost->GetRuntime(DotNetVersion, IID_ICLRRuntimeInfo, (VOID**)&pRuntimeInfo);
    CheckHr(hr);

    //Start and Load CLRApp
    BOOL bLoadable;
    hr = pRuntimeInfo->IsLoadable(&bLoadable);
    CheckHr(hr);
    hr = pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, (VOID**)&pRuntimeHost);
    CheckHr(hr);
    hr = pRuntimeHost->Start();
    CheckHr(hr);
}

void RunFromMemory(const BYTE* pImage,ULONG64 assembyLength)
{ 
    //Install AppDomain
    IUnknownPtr pAppDomainThunk = NULL;
    HRESULT hr = pRuntimeHost->GetDefaultDomain(&pAppDomainThunk);
    CheckHr(hr);
    _AppDomainPtr pDefaultAppDomain = NULL;
    hr = pAppDomainThunk->QueryInterface(__uuidof(_AppDomain), (VOID**)&pDefaultAppDomain);
    CheckHr(hr);
    _AssemblyPtr pAssembly = NULL;
    SAFEARRAYBOUND rgsabound[1];
    rgsabound[0].cElements = assembyLength;
    rgsabound[0].lLbound = 0;
    SAFEARRAY* pSafeArray = SafeArrayCreate(VT_UI1, 1, rgsabound);
    void* pvData = NULL;
    hr = SafeArrayAccessData(pSafeArray, &pvData);
    CheckHr(hr);

    //Add , execution , clean up , and memory reader
    memcpy(pvData, pImage, assembyLength);
    hr = SafeArrayUnaccessData(pSafeArray);
    CheckHr(hr);
    hr = pDefaultAppDomain->Load_3(pSafeArray, &pAssembly);
    CheckHr(hr);
    _MethodInfoPtr pMethodInfo = NULL;
    hr = pAssembly->get_EntryPoint(&pMethodInfo);
    CheckHr(hr);
    VARIANT retVal;
    ZeroMemory(&retVal, sizeof(VARIANT));
    VARIANT obj;
    ZeroMemory(&obj, sizeof(VARIANT));
    obj.vt = VT_NULL;
    SAFEARRAY* psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 0);
    hr = pMethodInfo->Invoke_3(obj, psaStaticMethodArgs, &retVal);
    CheckHr(hr);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Init();
    TCHAR NPath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, NPath);
    wprintf(NPath);
    HANDLE fhandle = CreateFile(L"TestExe.exe", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fhandle != INVALID_HANDLE_VALUE)
    {
        DWORD fileSize_H{ 0 };
        ULONG64 fileSize = GetFileSize(fhandle, &fileSize_H);
        fileSize |= (DWORD64)fileSize_H << sizeof(DWORD);
        BYTE* file_buff = new BYTE[fileSize];
        if (fileSize > 0 && ReadFile(fhandle, file_buff, fileSize, &fileSize_H, NULL))
        {
            RunFromMemory(file_buff, fileSize);
        }
    }
    else
    {
        wprintf(L"Error");
    }
}
