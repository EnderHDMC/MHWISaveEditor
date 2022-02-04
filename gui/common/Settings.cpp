#include "Settings.h"

#include "../../utility/paths.h"

Settings* Settings::instance = nullptr;

Settings::Settings()
{
  ReadSettings();
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

  bool requireRestart = false;
  bool oldMatrixMode = matrixMode;
  bool oldShowUnobtainable = showUnobtainable;

  settings->beginGroup("backups");
  doAutoBackups = settings->value("doAutoBackups", doAutoBackups).toBool();
  maxBackups = settings->value("maxBackups", maxBackups).toInt();
  settings->endGroup();

  settings->beginGroup("items");
  matrixMode = settings->value("matrixMode", matrixMode).toBool();
  showUnobtainable = settings->value("showUnobtainable", showUnobtainable).toBool();
  settings->endGroup();

  requireRestart |= oldMatrixMode != matrixMode;
  requireRestart |= oldShowUnobtainable != showUnobtainable;

  return requireRestart && sync;
}

void Settings::ReadSettings()
{
  QString path = GetDataPath() + "/settings.ini";
  QSettings::Format format = QSettings::Format::IniFormat;
  QSettings::setDefaultFormat(format);
  settings = new QSettings(path, format, nullptr);
  SyncSettings(false);

  qDebug() << "Read settings file: " + settings->fileName();
}

void Settings::WriteSettings()
{
  settings->beginGroup("backups");
  settings->setValue("doAutoBackups", doAutoBackups);
  settings->setValue("maxBackups", maxBackups);
  settings->endGroup();

  settings->beginGroup("items");
  settings->setValue("matrixMode", matrixMode);
  settings->setValue("showUnobtainable", showUnobtainable);
  settings->endGroup();

  qDebug() << "Wrote settings file: " + settings->fileName();
}

QString Settings::FileName()
{
  return settings->fileName();
}
