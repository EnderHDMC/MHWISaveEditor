#pragma once

#include <QObject>
#include <QSettings>

#include "../../types/editor_enums.h"

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
  QString _uiLanguage = "";
  item_language _itemLanguage = item_language::English;

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

  QString GetUiLanguage() { return _uiLanguage; }
  void SetUiLanguage(QString value) { if (_uiLanguage != value) _requireRestart = true; _uiLanguage = value; }
  item_language GetItemLanguage() { return _itemLanguage; }
  void SetItemLanguage(item_language value) { _itemLanguage = value; }
#pragma endregion

  // Needs to match up with the settings UI options.
  static const item_language Languages(u32 index) {
    item_language languages[] = {
    item_language::GameLanguage,
    item_language::Japanese,
    item_language::English,
    item_language::French,
    item_language::Italian,
    item_language::Dutch,
    item_language::Spanish,
    item_language::PortugueseBrazil,
    item_language::Polish,
    item_language::Russian,
    item_language::Korean,
    item_language::TraditionalChinese,
    item_language::SimplifiedChinese,
    item_language::Arabic,
    item_language::InvalidLanguage
    };

    if (index >= COUNTOF(languages)) return item_language::InvalidLanguage;
    return languages[index];
  }

  static QString GetLanguageCode(item_language language)
  {
    QString result = "eng";
    switch (language) {
    case item_language::Japanese:            result = "jpn"; break;
    case item_language::English:             result = "eng"; break;
    case item_language::French:              result = "fre"; break;
    case item_language::Spanish:             result = "spa"; break;
    case item_language::Dutch:               result = "ger"; break;
    case item_language::Italian:             result = "ita"; break;
    case item_language::Korean:              result = "kor"; break;
    case item_language::TraditionalChinese:  result = "chT"; break;
    case item_language::SimplifiedChinese:   result = "chS"; break;
    case item_language::Russian:             result = "rus"; break;
    case item_language::Polish:              result = "pol"; break;
    case item_language::PortugueseBrazil:    result = "ptB"; break;
    case item_language::Arabic:              result = "ara"; break;

    case item_language::InvalidLanguage:     result = NULL;  break;
    case item_language::GameLanguage:        result = NULL;  break;
    case item_language::EditorUILanguage:    result = NULL;  break;
    }
    return result;
  }

  static item_language LanguageFromCode(const QString& code)
  {
    item_language result = item_language::InvalidLanguage;
    for (int i = 0; Languages(i) != item_language::InvalidLanguage; i++) {
      item_language language = Languages(i);
      QString languageCode = GetLanguageCode(language);

      if (languageCode == code) {
        result = language;
        break;
      }
    }

    return result;
  }

  static item_language LanguageIndexToEnum(int index)
  {
    return Languages(index);
  }

  static int LanguageEnumToIndex(item_language language)
  {
    int index = -1;
    for (int i = 0; (u8)Languages(i) != 0xFF; i++) {
      if (Languages(i) == language) {
        index = i;
      }
    }
    return index;
  }
};
