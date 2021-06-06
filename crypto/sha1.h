#pragma once

#include "../types/types.h"
#include <cryptopp/sha.h>

static u8* GenerateHash(u8* save, int length, int offset)
{
    u8* hash = (u8*)malloc(sizeof(u8) * 20);
    CryptoPP::SHA1 sha1;
    sha1.CalculateDigest(hash, (save + offset), length - offset);
    byteswap(hash, 20);
    return hash;
}
