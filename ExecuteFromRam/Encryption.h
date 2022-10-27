#ifndef Encryption_H
#define Encryption_H

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef UINT
typedef unsigned int UINT;
#endif

BYTE* Encrypt(BYTE* buffer, UINT size, UINT& outSize);
BYTE* Decrypt(BYTE* buffer, UINT size);

#endif // !Encryption_H
