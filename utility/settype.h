#pragma once

#include <QString>

#include "../types/types.h"

void SetStr(str64* str, const QString& value) {
  char* strPtr = (char*)str;

  QByteArray newValue = value.toUtf8();
  unsigned int len = newValue.size();
  size_t lenMax = sizeof(*str);
  Q_ASSERT(len < lenMax);

  strncpy_s(strPtr, lenMax, newValue, lenMax);
  memset(strPtr + len, '\0', lenMax - len);
}
