#pragma once

#include <QFile>

#include "../crypto/blowfish.h"

#include "../types/language.h"

#include "../types/types.h"

#include "../types/file/am_dat.h"
#include "../types/file/gmd.h"
#include "../types/file/itm.h"
#include "../types/file/rod_inse.h"
#include "../types/file/wp_dat.h"
#include "../types/file/wp_dat_g.h"

#include "system/paths.h"

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

static u8* ReadEntireFileSize(const QString& path, u8* dst = nullptr, u32 *size = nullptr) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning("Cannot open file: %s", qUtf8Printable(path));
    return nullptr;
  }

  QByteArray blob = file.readAll();
  u32 array_size = 0;
  if (size) array_size = *size;
  if (array_size == 0) array_size = blob.size();
  if (size) *size = array_size;
  file.close();

  return QByteArrayToU8(blob, dst, array_size);
}

static u8* ReadEntireFile(const QString& path, u8* dst = nullptr, u32 size = 0) {
  return ReadEntireFileSize(path, dst, &size);
}

static bool ReadMetaFile(gmd_meta* meta, const QString& path)
{
  FreeMetaFile(meta);
  gmd_header* header = (gmd_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(itm_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  itm_header* header = (itm_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(am_dat_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  am_dat_header* header = (am_dat_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(rod_inse_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  u32 size = 0;
  rod_inse_header* header = (rod_inse_header*)ReadEntireFileSize(path, nullptr, &size);
  if (!header) {
    return false;
  }

  blowfish_decrypt((u8*)header, size, KEY_ROD_INSE);
  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(wp_dat_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  wp_dat_header* header = (wp_dat_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(wp_dat_g_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  wp_dat_g_header* header = (wp_dat_g_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFileLanguage(gmd_meta* meta, const QString& res, game_language language = game_language::InvalidLanguage)
{
  QString languageCode = Language::GetLanguageCode(language);

  QString gmdPath = res;
  if (language != game_language::InvalidLanguage) {
    gmdPath = QString(Paths::GetResourcesPath(res)).arg(languageCode);
  }

  return ReadMetaFile(meta, gmdPath);
}
