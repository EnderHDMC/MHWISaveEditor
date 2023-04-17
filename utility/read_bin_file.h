#pragma once

#include <QFile>
#include <QSaveFile>

#include "../types/types.h"
#include "../types/language.h"

#include "../crypto/blowfish.h"

#include "../types/file/am_dat.h"
#include "../types/file/eq_crt.h"
#include "../types/file/eq_cus.h"
#include "../types/file/gmd.h"
#include "../types/file/itm.h"
#include "../types/file/rod_inse.h"
#include "../types/file/uct.h"
#include "../types/file/wp_dat.h"
#include "../types/file/wp_dat_g.h"

#include "system/paths.h"
#include "system/file_utils.h"

static bool ReadMetaFile(gmd_meta* meta, const QString& path)
{
  FreeMetaFile(meta);
  gmd_header* header = (gmd_header*)FileUtils::ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(itm_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  itm_header* header = (itm_header*)FileUtils::ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(am_dat_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  am_dat_header* header = (am_dat_header*)FileUtils::ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(eq_crt_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  eq_crt_header* header = (eq_crt_header*)FileUtils::ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(eq_cus_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  eq_cus_header* header = (eq_cus_header*)FileUtils::ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(rod_inse_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  u32 size = 0;
  rod_inse_header* header = (rod_inse_header*)FileUtils::ReadEntireFileSize(path, nullptr, &size);
  if (!header) {
    return false;
  }

  blowfish_decrypt((u8*)header, size, KEY_ROD_INSE);
  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(wp_dat_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  wp_dat_header* header = (wp_dat_header*)FileUtils::ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(wp_dat_g_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  wp_dat_g_header* header = (wp_dat_g_header*)FileUtils::ReadEntireFile(path);
  if (!header) {
    return false;
  }

  return InitMetaFile(meta, header);
}

static bool ReadMetaFile(uct_meta* meta, const QString& path) {
  FreeMetaFile(meta);
  uct_header* header = (uct_header*)FileUtils::ReadEntireFile(path);
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
