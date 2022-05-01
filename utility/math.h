#pragma once
#include "../types/types.h"

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)

static u32 BSR(u32 x) {
  unsigned long index = 0;
  u8 nonZero = _BitScanReverse(&index, x);
  if (!nonZero) index = 0;

  return index;
}
#endif

static u32 RoundUpPow4(u32 x) {
  u32 res = 1 << BSR(x);

  if (x & (res - 1)) res <<= 1;
  return res << BSR(res) % 2;
}
