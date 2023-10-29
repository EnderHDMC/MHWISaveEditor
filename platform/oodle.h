#pragma once

#include "../types/types.h"

enum class OodleDecompressCallbackRet
{
  OodleDecompressCallbackRet_Continue = 0,
  OodleDecompressCallbackRet_Cancel = 1,
  OodleDecompressCallbackRet_Invalid = 2
};
static_assert(sizeof(OodleDecompressCallbackRet) == 4);

typedef OodleDecompressCallbackRet(__cdecl* OodleDecompressCallback) (
  void* userdata,
  const u8* rawBuf, uintptr_t rawLen, const u8* compBuf, uintptr_t compBufferSize,
  uintptr_t rawDone, uintptr_t compUsed);

enum class OodleLZ_FuzzSafe
{
  OodleLZ_FuzzSafe_No = 0,
  OodleLZ_FuzzSafe_Yes = 1,

  OodleLZ_FuzzSafe_Default = OodleLZ_FuzzSafe_Yes
};
static_assert(sizeof(OodleLZ_FuzzSafe) == 4);

enum class OodleLZ_CheckCRC
{
  OodleLZ_CheckCRC_No = 0,
  OodleLZ_CheckCRC_Yes = 1,

  OodleLZ_CheckCRC_Default = OodleLZ_CheckCRC_No
};
static_assert(sizeof(OodleLZ_CheckCRC) == 4);

enum class OodleLZ_Verbosity
{
  OodleLZ_Verbosity_None = 0,
  OodleLZ_Verbosity_Minimal = 1,
  OodleLZ_Verbosity_Some = 2,
  OodleLZ_Verbosity_Lots = 3,

  OodleLZ_Verbosity_Default = OodleLZ_Verbosity_None
};
static_assert(sizeof(OodleLZ_Verbosity) == 4);

enum class OodleLZ_Decode_ThreadPhase
{
  OodleLZ_Decode_ThreadPhase1 = 1,
  OodleLZ_Decode_ThreadPhase2 = 2,
  OodleLZ_Decode_ThreadPhaseAll = 3,
  OodleLZ_Decode_Unthreaded = OodleLZ_Decode_ThreadPhaseAll,

  OodleLZ_Decode_Default = OodleLZ_Decode_Unthreaded,
};
static_assert(sizeof(OodleLZ_Decode_ThreadPhase) == 4);

typedef int(__cdecl* Proto_OodleLZ_Decompress) (
  const void* compBuf, intptr_t compBufSize, void* rawBuf, intptr_t rawLen,
  OodleLZ_FuzzSafe fuzzSafe,
  OodleLZ_CheckCRC checkCRC,
  OodleLZ_Verbosity verbosity,
  void* decBufBase,
  intptr_t decBufSize,
  void* fpCallback,
  void* callbackUserData,
  void* decoderMemory,
  intptr_t decoderMemorySize,
  OodleLZ_Decode_ThreadPhase threadPhase
  );

class Oodle
{
public:
  static Proto_OodleLZ_Decompress OodleLZ_Decompress;
};
