#include "pch.h"

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

const int skip_arg = 3;
SAFEARRAY* GenArg(const LPWSTR* args, const int argc)
{
    if (argc <= skip_arg) return nullptr;

    HRESULT hr;
    SAFEARRAYBOUND rgsabound;
    rgsabound.cElements = argc - skip_arg;
    rgsabound.lLbound = 0;
    SAFEARRAY* pSafeArray = SafeArrayCreate(VT_BSTR, 1, &rgsabound);
    if (pSafeArray == nullptr) return nullptr;

    long index = 0;
    for (int i = skip_arg; i < argc; i++)
    {
        BSTR pData = SysAllocString(args[i]);
        long rgIndicies = index;
        hr = SafeArrayPutElement(pSafeArray, &rgIndicies, pData);
        CheckHr(hr);
        index++;
    }

    return pSafeArray;
}

void FillKey_128(BYTE* key)
{
    key[0] = 0x67;
    key[1] = 0x56;
    key[2] = 0x6B;
    key[3] = 0x59;
    key[4] = 0x70;
    key[5] = 0x33;
    key[6] = 0x73;
    key[7] = 0x36;
    key[8] = 0x76;
    key[9] = 0x39;
    key[10] = 0x79;
    key[11] = 0x24;
    key[12] = 0x42;
    key[13] = 0x26;
    key[14] = 0x45;
    key[15] = 0x28;
}
void FillIV_128(BYTE* iv)
{
    iv[0] = 0x40; 
    iv[1] = 0x4E;
    iv[2] = 0x63;
    iv[3] = 0x52;
    iv[4] = 0x66;
    iv[5] = 0x55;
    iv[6] = 0x6A;
    iv[7] = 0x58;
    iv[8] = 0x6E;
    iv[9] = 0x5A;
    iv[10] = 0x72;
    iv[11] = 0x34;
    iv[12] = 0x75;
    iv[13] = 0x37;
    iv[14] = 0x78;
    iv[15] = 0x21;
}

BYTE* ReadFile(LPCWSTR fileName, UINT* fileSize)
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

void WriteFile(LPCWSTR fileName,BYTE* buffFile,UINT fileSize)
{
    HANDLE fhandle = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fhandle == INVALID_HANDLE_VALUE) exit(GetLastError());
    DWORD byte_write{ 0 };
    if (!WriteFile(fhandle, (LPCVOID)buffFile, fileSize, &byte_write, nullptr)) exit(GetLastError());
    CloseHandle(fhandle);
}

BYTE* Encrypt(BYTE* buffer,UINT size,UINT &outSize)
{
    BYTE key[16];
    FillKey_128(key);

    BYTE iv[16];
    FillIV_128(iv);

    AES aes(128);
    return aes.EncryptCBC(buffer, size, key, iv, outSize);
}
BYTE* Decrypt(BYTE* buffer, UINT size)
{
    BYTE key[16];
    FillKey_128(key);

    BYTE iv[16];
    FillIV_128(iv);

    AES aes(128);
    return aes.DecryptCBC(buffer, size, key, iv);
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


    std::wstring asmName(domainAssemblyName);
    asmName.append(L"e7aba3ea");
    asmName.append(L"9a932f01");
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

    return hr;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int argc;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc > 2)
    {
        if (!wcscmp(L"-r", args[1]))
        {
            //args:
            //0 : this exe
            //1 : -r
            //2 : file_name
            //3 : args....

            UINT buffSize;
            BYTE* file_buff = ReadFile(args[2], &buffSize);

            BYTE* file_decrypt = Decrypt(file_buff, buffSize);
            delete[] file_buff;

            SAFEARRAY* binary = GenBinary(file_decrypt, buffSize);
            SAFEARRAY* args_sa = GenArg(args, argc);

            delete[] file_decrypt;

            InitClrHost();
            return RunFromMemory(binary, args_sa);
        }
        else if (!wcscmp(L"-e", args[1]))
        {
            //args:
            //0 : this exe
            //1 : -e
            //2 : file_name

            UINT buffSize;
            BYTE* file_buff = ReadFile(args[2], &buffSize);

            UINT encrypt_size;
            BYTE* encrypt = Encrypt(file_buff, buffSize, encrypt_size);
            delete[] file_buff;

            std::wstring fileOut(args[2]);
            fileOut.append(L".encrypt");
            WriteFile(fileOut.c_str(), encrypt, encrypt_size);

            delete[] encrypt;
        }
    }
    return 0;
    
}
