#include "pch.h"

//C:\Windows\Microsoft.NET\Framework
//C:\Windows\Microsoft.NET\Framework64
const LPCWSTR RuntimeVersion = L"v4.0.30319";
const LPCWSTR domainAssemblyName = L"MyAppDomainManager";
const LPCWSTR domainTypename = L"MyAppDomainManager.CustomAppDomainManager";

//https://www.codeproject.com/Articles/1236146/Protecting-NET-plus-Application-By-Cplusplus-Unman
//https://www.codeproject.com/Articles/416471/CLR-Hosting-Customizing-the-CLR

ICLRMetaHost* pMetaHost = NULL;
ICLRRuntimeInfo* pRuntimeInfo = NULL;
ICLRRuntimeHost* pRuntimeHost = NULL;
ICLRControl* pCLRControl = NULL;
MinimalHostControl* pMyHostControl = NULL;

SAFEARRAY* GenBinary(const BYTE* pImage, ULONG assembyLength)
{
    SAFEARRAYBOUND rgsabound[1];
    rgsabound[0].cElements = assembyLength;
    rgsabound[0].lLbound = 0;
    SAFEARRAY* pSafeArray = SafeArrayCreate(VT_UI1, 1, rgsabound);
    if (pSafeArray == nullptr) return nullptr;

    void* pvData = NULL;
    HRESULT hr = SafeArrayAccessData(pSafeArray, &pvData);
    CheckHr(hr);
    memcpy(pvData, pImage, assembyLength);
    hr = SafeArrayUnaccessData(pSafeArray);
    CheckHr(hr);
    return pSafeArray;
}

SAFEARRAY* GenArg(const LPWSTR* args, const int argc)
{
    SAFEARRAYBOUND rgsabound[1];
    rgsabound[0].cElements = argc;
    rgsabound[0].lLbound = 0;
    SAFEARRAY* pSafeArray = SafeArrayCreate(VT_BSTR, 1, rgsabound);
    if (pSafeArray == nullptr) return nullptr;

    for (int i = 0; i < argc; i++)
    {
        BSTR pData = SysAllocString(args[i]);
        long rgIndicies[1];
        rgIndicies[0] = rgsabound[0].lLbound + i;
        HRESULT hr = SafeArrayPutElement(pSafeArray, rgIndicies, pData);
    }
    return pSafeArray;
}



void RunFromMemory(SAFEARRAY* binary,SAFEARRAY* args)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);//Install WPF Mode
    CheckHr(hr);
    hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);//Install MetaHost
    CheckHr(hr);
    hr = pMetaHost->GetRuntime(RuntimeVersion, IID_PPV_ARGS(&pRuntimeInfo));//Install .NET Runtime
    CheckHr(hr);
    hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pRuntimeHost));
    CheckHr(hr);
    
    //Install AppDomain
    hr = pRuntimeHost->GetCLRControl(&pCLRControl);
    CheckHr(hr);

    pMyHostControl = new MinimalHostControl();
    hr = pRuntimeHost->SetHostControl(pMyHostControl);
    CheckHr(hr);

    hr = pCLRControl->SetAppDomainManagerType(domainAssemblyName, domainTypename);
    CheckHr(hr);

    hr = pRuntimeHost->Start();//Start CLR host
    CheckHr(hr);


    ICustomAppDomainManager* pAppDomainManager = pMyHostControl->GetDomainManagerForDefaultDomain();
    BSTR name = SysAllocString(L"TestExe");
    _bstr_t name_t;
    name_t.Assign(name);

    hr = pAppDomainManager->Run(name_t, binary, args);
    CheckHr(hr);

    hr = pRuntimeHost->Stop();
    CheckHr(hr);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int argc;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argc);

    HANDLE fhandle = CreateFile(L"TestExe.exe", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fhandle != INVALID_HANDLE_VALUE)
    {
        ULONG fileSize = GetFileSize(fhandle, nullptr);
        if (fileSize)
        {
            DWORD byte_read;
            BYTE* file_buff = new BYTE[fileSize];
            if (fileSize > 0 && ReadFile(fhandle, file_buff, fileSize, &byte_read, NULL) && byte_read == fileSize)
            {
                SAFEARRAY* binary = GenBinary(file_buff, fileSize);
                SAFEARRAY* args_sa = GenArg(args, argc);
                RunFromMemory(binary, args_sa);
            }
            delete[] file_buff;
        }
        CloseHandle(fhandle);
    }
    else
    {
        wprintf(L"Error");
    }
    system("pause");
}
