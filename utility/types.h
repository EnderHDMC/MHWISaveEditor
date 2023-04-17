#pragma once
#include "../types/types.h"

#include <QByteArray>

static u8* QByteArrayToU8(QByteArray arr, u8* dst, u32 size) {
  u8* newdst = dst;
  if (arr.length() != size) {
    qWarning("Error: array size does not match desired size.");
    return nullptr;
  }

  if (!newdst) newdst = (u8*)malloc(size);
  if (!newdst) {
    qWarning("Error allocating memory.");
    return nullptr;
  };
  memcpy(newdst, arr.constData(), arr.length());

  if (dst != newdst && dst) free(dst);
  return newdst;
}
