#pragma once

#include <QString>

#include "../types/types.h"

static void SetStrImpl(void* dst, size_t dstSize, const QString& value)
{
  char* strPtr = (char*)dst;
  QByteArray utf8 = value.toUtf8();

  size_t srcLen = utf8.size();
  Q_ASSERT(srcLen < dstSize);
  size_t len = (srcLen < dstSize) ? srcLen : dstSize - 1;

  memcpy(strPtr, utf8.constData(), len);
  memset(strPtr + len, 0, dstSize - len);
}

inline static void SetStr(str54* str, const QString& value) {
  SetStrImpl(str, sizeof(*str), value);
}

inline static void SetStr(str64* str, const QString& value) {
  SetStrImpl(str, sizeof(*str), value);
}

inline static void SetStr(str256* str, const QString& value) {
  SetStrImpl(str, sizeof(*str), value);
}
