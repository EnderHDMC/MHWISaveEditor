#pragma once

#include <QFile>

#include "../types/types.h"

#include "../types/file/gmd.h"
#include "../types/file/itm.h"

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
    qWarning("Cannot open file: %s", qUtf8Printable(path));
    return nullptr;
  }

  QByteArray blob = file.readAll();
  if (size == 0) size = blob.size();
  file.close();

  return QByteArrayToU8(blob, dst, size);
}

static bool Read_gmd(gmd_meta* meta, const QString& path)
{
  gmd_header* header = (gmd_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMeta_gmd(meta, header);
}

static bool Read_itm(itm_meta* meta, const QString& path) {
  itm_header* header = (itm_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMeta_itm(meta, header);
}
