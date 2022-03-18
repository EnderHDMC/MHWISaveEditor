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

static bool Read_gmd(gmd_meta* meta, const QString& path)
{
  FreeMeta_gmd(meta);
  gmd_header* header = (gmd_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMeta_gmd(meta, header);
}

static bool Read_itm(itm_meta* meta, const QString& path) {
  FreeMeta_itm(meta);
  itm_header* header = (itm_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMeta_itm(meta, header);
}

static bool Read_am_dat(am_dat_meta* meta, const QString& path) {
  FreeMeta_am_dat(meta);
  am_dat_header* header = (am_dat_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMeta_am_dat(meta, header);
}

static bool Read_rod_inse(rod_inse_meta* meta, const QString& path) {
  FreeMeta_rod_inse(meta);
  u32 size = 0;
  rod_inse_header* header = (rod_inse_header*)ReadEntireFileSize(path, nullptr, &size);
  if (!header) {
    return false;
  }

  blowfish_decrypt((u8*)header, size, KEY_ROD_INSE);
  return InitMeta_rod_inse(meta, header);
}

static bool Read_wp_dat(wp_dat_meta* meta, const QString& path) {
  FreeMeta_wp_dat(meta);
  wp_dat_header* header = (wp_dat_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMeta_wp_dat(meta, header);
}

static bool Read_wp_dat_g(wp_dat_g_meta* meta, const QString& path) {
  FreeMeta_wp_dat_g(meta);
  wp_dat_g_header* header = (wp_dat_g_header*)ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMeta_wp_dat_g(meta, header);
}

static bool ReadLanguage_gmd(gmd_meta* meta, const QString& res, game_language language = game_language::InvalidLanguage)
{
  QString languageCode = Language::GetLanguageCode(language);

  QString gmdPath = res;
  if (language != game_language::InvalidLanguage) {
    gmdPath = QString(Paths::GetResourcesPath(res)).arg(languageCode);
  }

  return Read_gmd(meta, gmdPath);
}
