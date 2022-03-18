#include "Settings.h"

#include "../system/paths.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif // Q_OS_WIN

Settings* Settings::instance = nullptr;

Settings::Settings()
{
  ReadSettings();
  _requireRestart = false;
}

Settings* Settings::GetInstance()
{
  if (!instance) instance = new Settings();
  return instance;
}

void Settings::Free()
{
  WriteSettings();
  delete settings;

  delete(instance);
  instance = nullptr;
}

bool Settings::SyncSettings(bool sync)
{
  if (sync) settings->sync();

  settings->beginGroup("backups");
  SetDoAutoBackups(settings->value("doAutoBackups", _doAutoBackups).toBool());
  SetMaxBackups(settings->value("maxBackups", _maxBackups).toInt());
  settings->endGroup();

  settings->beginGroup("items");
  SetMatrixMode(settings->value("matrixMode", _matrixMode).toBool());
  SetShowUnobtainable(settings->value("showUnobtainable", _showUnobtainable).toBool());
  SetItemPouchSearchAll(settings->value("itemPouchSearchAll", _itemPouchSearchAll).toBool());
  settings->endGroup();

  settings->beginGroup("qol");
  SetDarkMode(settings->value("darkMode", _darkMode).toBool());
  settings->endGroup();

  settings->beginGroup("language");
  SetUiLanguage(settings->value("uiLanguage", _uiLanguage).toString());
  SetItemLanguage((game_language)settings->value("itemLanguage", (u8)_itemLanguage).toInt());
  settings->endGroup();

  settings->beginGroup("debug");
  SetShowConsole(settings->value("showConsole", _showConsole).toBool());
  settings->endGroup();

  return GetRequireRestart() && sync;
}

void Settings::ReadSettings()
{
  QString path = Paths::GetDataPath() + "/settings.ini";
  QSettings::Format format = QSettings::Format::IniFormat;
  QSettings::setDefaultFormat(format);
  settings = new QSettings(path, format, nullptr);
  SyncSettings(false);
}

void Settings::WriteSettings()
{
  settings->beginGroup("backups");
  settings->setValue("doAutoBackups", _doAutoBackups);
  settings->setValue("maxBackups", _maxBackups);
  settings->endGroup();

  settings->beginGroup("items");
  settings->setValue("matrixMode", _matrixMode);
  settings->setValue("showUnobtainable", _showUnobtainable);
  settings->setValue("itemPouchSearchAll", _itemPouchSearchAll);
  settings->endGroup();

  settings->beginGroup("qol");
  settings->setValue("darkMode", _darkMode);
  settings->endGroup();

  settings->beginGroup("language");
  settings->setValue("uiLanguage", _uiLanguage);
  settings->setValue("itemLanguage", (u8)_itemLanguage);
  settings->endGroup();

  settings->beginGroup("debug");
  settings->setValue("showConsole", _showConsole);
  settings->endGroup();

  qDebug() << "Wrote settings file:" << settings->fileName();
}

QString Settings::FileName()
{
  return settings->fileName();
}

void Settings::LogReadPath()
{
  qDebug() << "Read settings file (log delayed):" << settings->fileName();
}

bool Settings::DebuggerPresent()
{
  bool debugger = false;

#ifdef Q_OS_WIN
  debugger = IsDebuggerPresent();
#endif // Q_OS_WIN

  return debugger;
}
