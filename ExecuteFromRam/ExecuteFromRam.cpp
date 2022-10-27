#include <Windows.h>
#include <string>
#include "Encryption.h"
#include "ClrHelper.h"

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

int RunFile(const LPCWSTR filePath, const LPWSTR* args, const int argc, const int argsIgnoreCount)
{
    UINT buffSize;
    BYTE* file_buff = ReadFile(filePath, &buffSize);

    BYTE* file_decrypt = Decrypt(file_buff, buffSize);
    delete[] file_buff;

    SAFEARRAY* binary = GenBinary(file_decrypt, buffSize);
    SAFEARRAY* args_sa = GenArg(args, argc, argsIgnoreCount);

    delete[] file_decrypt;

    InitClrHost();
    return RunFromMemory(binary, args_sa);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    int argc;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc > 2)
    {
        if (!wcscmp(L"-r", args[1]))//run
        {
            //args:
            //0 : this exe
            //1 : -r
            //2 : file_name
            //3 : args....

            return RunFile(args[2], args, argc, 3);
        }
        else if (!wcscmp(L"-e", args[1]))//encrypt
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
    else//auto search
    {
        WIN32_FIND_DATA data;
        HANDLE hFind = FindFirstFile(L"*.exe.encrypt", &data);
        if (hFind == INVALID_HANDLE_VALUE) return ERROR_FILE_NOT_FOUND;
        FindClose(hFind);

        return RunFile(data.cFileName, args, argc, 1);
    }
    return 0;
}
