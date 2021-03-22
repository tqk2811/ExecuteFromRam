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

SAFEARRAY* GenArg(const LPWSTR* args, const int argc)
{
    if (argc < 2) return nullptr;

    HRESULT hr;
    SAFEARRAYBOUND rgsabound;
    rgsabound.cElements = argc - 1;
    rgsabound.lLbound = 0;
    SAFEARRAY* pSafeArray = SafeArrayCreate(VT_BSTR, 1, &rgsabound);
    if (pSafeArray == nullptr) return nullptr;

    for (int i = 1; i < argc; i++)
    {
        BSTR pData = SysAllocString(args[i]);
        long rgIndicies;
        rgIndicies = rgsabound.lLbound + i - 1;
        hr = SafeArrayPutElement(pSafeArray, &rgIndicies, pData);
        CheckHr(hr);
    }

    return pSafeArray;
}


void InitClrHost()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);//Install WPF Mode
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

    hr = pCLRControl->SetAppDomainManagerType(domainAssemblyName, domainTypename);
    CheckHr(hr);

    hr = pRuntimeHost->Start();//Start CLR host
    CheckHr(hr);
}

int RunFromMemory(SAFEARRAY* binary,SAFEARRAY* args)
{
    ICustomAppDomainManager* pAppDomainManager = pMyHostControl->GetDomainManagerForDefaultDomain();

    BSTR name = SysAllocString(L"TestfriendlyName");
    _bstr_t name_t;
    name_t.Assign(name);

    TCHAR path[2001] = L"";
    DWORD len = GetCurrentDirectory(2000, path);

    _bstr_t work_dir;
    work_dir.Assign(SysAllocString(path));

    HRESULT hr = pAppDomainManager->Run(name_t, work_dir, binary, args);
    CheckHr(hr);

    HRESULT hr_ = pRuntimeHost->Stop();
    CheckHr(hr_);

    return hr;
}

BYTE* ReadFile(LPCWSTR fileName, ULONG* fileSize)
{
    HANDLE fhandle = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fhandle == INVALID_HANDLE_VALUE) exit(GetLastError());

    *fileSize = GetFileSize(fhandle, nullptr);
    if (!fileSize) exit(ERROR_FILE_NOT_SUPPORTED);

    DWORD byte_read;
    BYTE* file_buff = new BYTE[*fileSize];
    if (!ReadFile(fhandle, file_buff, *fileSize, &byte_read, NULL)) exit(GetLastError());
    if (byte_read != *fileSize) exit(-1);
    CloseHandle(fhandle);

    //decrypt here

    return file_buff;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int argc;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argc);

    ULONG buffSize;
    BYTE* file_buff = ReadFile(L"TestExe.exe", &buffSize);

    SAFEARRAY* binary = GenBinary(file_buff, buffSize);
    SAFEARRAY* args_sa = GenArg(args, argc);

    delete[] file_buff;

    InitClrHost();
    return RunFromMemory(binary, args_sa);
}
