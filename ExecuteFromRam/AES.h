#pragma once
//https://github.com/SergeyBel/AES
#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef UINT
typedef unsigned int UINT;
#endif

class AES
{
private:
    int Nb;
    int Nk;
    int Nr;

    UINT blockBytesLen;

    void SubBytes(BYTE** state);

    void ShiftRow(BYTE** state, int i, int n);    // shift row i on n positions

    void ShiftRows(BYTE** state);

    BYTE xtime(BYTE b);    // multiply on x

    BYTE mul_bytes(BYTE a, BYTE b);

    void MixColumns(BYTE** state);

    void MixSingleColumn(BYTE* r);

    void AddRoundKey(BYTE** state, BYTE* key);

    void SubWord(BYTE* a);

    void RotWord(BYTE* a);

    void XorWords(BYTE* a, BYTE* b, BYTE* c);

    void Rcon(BYTE* a, int n);

    void InvSubBytes(BYTE** state);

    void InvMixColumns(BYTE** state);

    void InvShiftRows(BYTE** state);

    BYTE* PaddingNulls(BYTE* in, UINT inLen, UINT alignLen);

    UINT GetPaddingLength(UINT len);

    void KeyExpansion(BYTE* key, BYTE* w);

    void EncryptBlock(BYTE* in, BYTE* out, BYTE* key);

    void DecryptBlock(BYTE* in, BYTE* out, BYTE* key);

    void XorBlocks(BYTE* a, BYTE* b, BYTE* c, UINT len);
    
public:
    AES(int keyLen = 256);

    //https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation

    BYTE* EncryptECB(BYTE* in, UINT inLen, BYTE* key, UINT& outLen);

    BYTE* DecryptECB(BYTE* in, UINT inLen, BYTE* key);

    BYTE* EncryptCBC(BYTE* in, UINT inLen, BYTE* key, BYTE* iv, UINT& outLen);

    BYTE* DecryptCBC(BYTE* in, UINT inLen, BYTE* key, BYTE* iv);

    BYTE* EncryptCFB(BYTE* in, UINT inLen, BYTE* key, BYTE* iv, UINT& outLen);

    BYTE* DecryptCFB(BYTE* in, UINT inLen, BYTE* key, BYTE* iv);

    void printHexArray(BYTE* a, UINT n);
};
