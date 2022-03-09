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
  game_language _itemLanguage = game_language::English;

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
  game_language GetItemLanguage() { return _itemLanguage; }
  void SetItemLanguage(game_language value) { _itemLanguage = value; }
#pragma endregion

  // Needs to match up with the settings UI options.
  static const game_language Languages(u32 index) {
    game_language languages[] = {
    game_language::GameLanguage,
    game_language::Japanese,
    game_language::English,
    game_language::French,
    game_language::Italian,
    game_language::Dutch,
    game_language::Spanish,
    game_language::PortugueseBrazil,
    game_language::Polish,
    game_language::Russian,
    game_language::Korean,
    game_language::TraditionalChinese,
    game_language::SimplifiedChinese,
    game_language::Arabic,
    game_language::InvalidLanguage
    };

    if (index >= COUNTOF(languages)) return game_language::InvalidLanguage;
    return languages[index];
  }

  static QString GetLanguageCode(game_language language)
  {
    QString result = "eng";
    switch (language) {
    case game_language::Japanese:            result = "jpn"; break;
    case game_language::English:             result = "eng"; break;
    case game_language::French:              result = "fre"; break;
    case game_language::Spanish:             result = "spa"; break;
    case game_language::Dutch:               result = "ger"; break;
    case game_language::Italian:             result = "ita"; break;
    case game_language::Korean:              result = "kor"; break;
    case game_language::TraditionalChinese:  result = "chT"; break;
    case game_language::SimplifiedChinese:   result = "chS"; break;
    case game_language::Russian:             result = "rus"; break;
    case game_language::Polish:              result = "pol"; break;
    case game_language::PortugueseBrazil:    result = "ptB"; break;
    case game_language::Arabic:              result = "ara"; break;

    case game_language::InvalidLanguage:     result = NULL;  break;
    case game_language::GameLanguage:        result = NULL;  break;
    case game_language::EditorUILanguage:    result = NULL;  break;
    }
    return result;
  }

  static game_language LanguageFromCode(const QString& code)
  {
    game_language result = game_language::InvalidLanguage;
    for (int i = 0; Languages(i) != game_language::InvalidLanguage; i++) {
      game_language language = Languages(i);
      QString languageCode = GetLanguageCode(language);

      if (languageCode == code) {
        result = language;
        break;
      }
    }

    return result;
  }

  static game_language LanguageIndexToEnum(int index)
  {
    return Languages(index);
  }

  static int LanguageEnumToIndex(game_language language)
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
