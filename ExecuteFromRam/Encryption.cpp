#include "AES.h"
#include "Encryption.h"

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

BYTE* Encrypt(BYTE* buffer, UINT size, UINT& outSize)
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