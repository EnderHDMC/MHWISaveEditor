#pragma once

#include <QSettings>

class Settings
{
private:
  static Settings* instance;
  Settings();

  QSettings* settings = nullptr;
  void ReadSettings();

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

  // Backups
  bool doAutoBackups = true;
  int maxBackups = 64;

  // Items
  bool matrixMode = false;
  bool showUnobtainable = true;
  bool searchAllTabsIncludeItemPouch = false;

  // Quality of Life
  bool darkMode = false;

  // Language
  QString itemLanguage = "eng";
};
