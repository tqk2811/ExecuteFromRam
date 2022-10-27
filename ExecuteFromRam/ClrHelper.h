#ifndef ClrHelper_H
#define ClrHelper_H

SAFEARRAY* GenBinary(const BYTE* pImage, ULONG assembyLength);
SAFEARRAY* GenArg(const LPWSTR* args, const int argc, const int argsIgnoreCount);
void InitClrHost();
int RunFromMemory(SAFEARRAY* binary, SAFEARRAY* args);

#endif // !ClrHelper_H


