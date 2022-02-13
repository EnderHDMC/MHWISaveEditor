#pragma once

#include <QObject>
#include <QSettings>

#include "../../types/mhw_enums.h"

class Settings
{
private:
  static Settings* instance;
  Settings();

  QSettings* settings = nullptr;
  void ReadSettings();

  //////////////////////////
  //////// Settings ////////
  //////////////////////////
  bool _requireRestart = false;

  // Backups
  bool _doAutoBackups = true;
  u32 _maxBackups = 64;

  // Items
  bool _matrixMode = false;
  bool _showUnobtainable = true;
  bool _itemPouchSearchAll = false;

  // Quality of Life
  bool _darkMode = false;

  // Language
  mhw_language _uiLanguage = mhw_language::English;
  mhw_language _itemLanguage = mhw_language::English;

public:
  static Settings* GetInstance();
  void Free();

  bool SyncSettings(bool sync = false);
  void WriteSettings();
  QString FileName();

  // Paths
  static QString GetSteamPath();
  static QString GetGamePath();
  static QString GetDefaultSaveDir();
  static QString GetDefaultSavePath();
  static QString GetDefaultDumpPath(int slot);
  static QString GetDataPath();
  static QString GetDataPathBackups();
  static QString GetIconDumpPath();

  // Language
  static QString GetLanguageCode(mhw_language language);
  static mhw_language LanguageIndexToEnum(int index);
  static int LanguageEnumToIndex(mhw_language language);

#pragma region Settings Access
  bool GetRequireRestart() { return _requireRestart; }

  bool GetDoAutoBackups() { return _doAutoBackups; }
  void SetDoAutoBackups(bool value) { _doAutoBackups = value; }
  u32 GetMaxBackups() { return _maxBackups; }
  void SetMaxBackups(u32 value) { _maxBackups = value; }

  bool GetMatrixMode() { return _matrixMode; }
  void SetMatrixMode(bool value) { if (_matrixMode != value) _requireRestart = true; _matrixMode = value; }
  bool GetShowUnobtainable() { return _showUnobtainable; }
  void SetShowUnobtainable(bool value) { if (_showUnobtainable != value) _requireRestart = true; _showUnobtainable = value; }
  bool GetItemPouchSearchAll() { return _itemPouchSearchAll; }
  void SetItemPouchSearchAll(bool value) { _itemPouchSearchAll = value; }

  bool GetDarkMode() { return _darkMode; }
  void SetDarkMode(bool value) { if (_darkMode != value) _requireRestart = true; _darkMode = value; }

  mhw_language GetUiLanguage() { return _uiLanguage; }
  void SetUiLanguage(mhw_language value) { if (_uiLanguage != value) _requireRestart = true; _uiLanguage = value; }
  mhw_language GetItemLanguage() { return _itemLanguage; }
  void SetItemLanguage(mhw_language value) { _itemLanguage = value; }
#pragma endregion

  // Needs to match up with the settings UI options.
  static const mhw_language Languages(u32 index) {
    mhw_language languages[] = {
    (mhw_language)0xFE,             // Special value to indicate to use the game's language.
    mhw_language::Japanese,
    mhw_language::English,
    mhw_language::French,
    mhw_language::Italian,
    mhw_language::Dutch,
    mhw_language::Spanish,
    mhw_language::PortugueseBrazil,
    mhw_language::Polish,
    mhw_language::Russian,
    mhw_language::Korean,
    mhw_language::TraditionalChinese,
    mhw_language::SimplifiedChinese,
    mhw_language::Arabic,
    (mhw_language)0xFF                // Sentinel value for end of array
    };

    if (index >= COUNTOF(languages)) return (mhw_language)0xFF;
    return languages[index];
  }
};
