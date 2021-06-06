#pragma once

#include <cryptopp/blowfish.h>
#include <cryptopp/modes.h>
#include <string>
#include "../types/types.h"
#include "../types/constants.h"
#include "../utility/endian.h"

static void blowfish_decrypt(u8 data[], int length) {
    std::string key = BLOWFISH_KEY;

    CryptoPP::ECB_Mode<CryptoPP::Blowfish>::Decryption decryption;
    decryption.SetKey((u8*)key.c_str(), key.length());

    byteswap(data, length);
    decryption.ProcessData(data, data, length);
    byteswap(data, length);
}

static void blowfish_encrypt(u8 data[], int length) {
    std::string key = BLOWFISH_KEY;

    CryptoPP::ECB_Mode<CryptoPP::Blowfish>::Encryption encryption;
    encryption.SetKey((u8*)key.c_str(), key.length());

    byteswap(data, length);
    encryption.ProcessData(data, data, length);
    byteswap(data, length);
}
