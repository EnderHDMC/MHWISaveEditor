#pragma once

#include <QString>
#include <QFile>

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

static u8* ReadEntireFile(const QString& path, u8* dst = nullptr, u32 size = 0) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Cannot open file: " + path;
    return nullptr;
  }

  QByteArray blob = file.readAll();
  if (size == 0) size = blob.size();
  file.close();

  return QByteArrayToU8(blob, dst, size);
}
