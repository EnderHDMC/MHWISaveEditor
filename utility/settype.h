#pragma once

#include <QString>

#include "../types/types.h"

static void SetStr(str64* str, const QString& value) {
  char* strPtr = (char*)str;

  QByteArray newValue = value.toUtf8();
  unsigned int len = newValue.size();
  size_t lenMax = sizeof(*str);
  Q_ASSERT(len < lenMax);

  strncpy_s(strPtr, lenMax, newValue, lenMax);
  memset(strPtr + len, '\0', lenMax - len);
}

static void SetStr(str256* str, const QString& value) {
  char* strPtr = (char*)str;

  QByteArray newValue = value.toUtf8();
  unsigned int len = newValue.size();
  size_t lenMax = sizeof(*str);
  Q_ASSERT(len < lenMax);

  strncpy_s(strPtr, lenMax, newValue, lenMax);
  memset(strPtr + len, '\0', lenMax - len);
}

static u8* QByteArrayToU8(QByteArray arr, u8* dst, u32 size) {
  if (arr.length() != size) {
    qWarning("Error: array size does not match desired size.");
    return nullptr;
  }

  if (!dst) dst = (u8*)malloc(size);
  if (!dst) {
    qWarning("Error allocating memory.");
    return nullptr;
  };
  memcpy(dst, arr.constData(), arr.length());

  return dst;
}
