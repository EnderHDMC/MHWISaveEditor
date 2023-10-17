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

  // Debug
  bool _showConsole = false;

public:
  static Settings* GetInstance();
  void Free();

  bool SyncSettings(bool sync = false);
  void WriteSettings();
  QString FileName();

  void LogReadPath();

  static bool DebuggerPresent();

#pragma region Settings Access
  bool GetRequireRestart() { return _requireRestart; }

  bool GetDoAutoBackups() { return _doAutoBackups; }
  void SetDoAutoBackups(bool value) { _doAutoBackups = value; }
  u32 GetMaxBackups() { return _maxBackups; }
  void SetMaxBackups(u32 value) { _maxBackups = value; }

  bool GetMatrixMode() { return _matrixMode; }
  void SetMatrixMode(bool value) { _matrixMode = value; }
  bool GetShowUnobtainable() { return _showUnobtainable; }
  void SetShowUnobtainable(bool value) { _showUnobtainable = value; }
  bool GetItemPouchSearchAll() { return _itemPouchSearchAll; }
  void SetItemPouchSearchAll(bool value) { _itemPouchSearchAll = value; }

  bool GetDarkMode() { return _darkMode; }
  void SetDarkMode(bool value) { if (_darkMode != value) _requireRestart = true; _darkMode = value; }

  QString GetUiLanguage() { return _uiLanguage; }
  void SetUiLanguage(QString value) { if (_uiLanguage != value) _requireRestart = true; _uiLanguage = value; }
  game_language GetItemLanguage() { return _itemLanguage; }
  void SetItemLanguage(game_language value) { _itemLanguage = value; }

  bool GetShowConsole() { return _showConsole; }
  void SetShowConsole(bool value) { if (_showConsole != value) _requireRestart = true; _showConsole = value; }
#pragma endregion
};
