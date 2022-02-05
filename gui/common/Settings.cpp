#include "Settings.h"

#include <QDir>
#include <QStandardPaths>

#include "../../types/constants.h"

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
  bool oldDarkMode = darkMode;

  settings->beginGroup("backups");
  doAutoBackups = settings->value("doAutoBackups", doAutoBackups).toBool();
  maxBackups = settings->value("maxBackups", maxBackups).toInt();
  settings->endGroup();

  settings->beginGroup("items");
  matrixMode = settings->value("matrixMode", matrixMode).toBool();
  showUnobtainable = settings->value("showUnobtainable", showUnobtainable).toBool();
  searchAllTabsIncludeItemPouch =
    settings->value("searchAllTabsIncludeItemPouch", searchAllTabsIncludeItemPouch).toBool();
  settings->endGroup();

  settings->beginGroup("qol");
  darkMode = settings->value("darkMode", darkMode).toBool();
  settings->endGroup();

  requireRestart |= oldMatrixMode != matrixMode;
  requireRestart |= oldShowUnobtainable != showUnobtainable;
  requireRestart |= oldDarkMode != darkMode;

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
  settings->setValue("searchAllTabsIncludeItemPouch", searchAllTabsIncludeItemPouch);
  settings->endGroup();

  settings->beginGroup("qol");
  settings->setValue("darkMode", darkMode);
  settings->endGroup();

  qDebug() << "Wrote settings file: " + settings->fileName();
}

QString Settings::FileName()
{
  return settings->fileName();
}

QString Settings::GetSteamPath()
{
  QSettings regSteam(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)", QSettings::NativeFormat);
  if (regSteam.childKeys().contains("SteamPath"))
    return regSteam.value("SteamPath").toString();

  return NULL;
}

QString Settings::GetGamePath()
{
  QString path = GetSteamPath();
  if (!path.isNull()) {
    QDir fullpath(path);

    bool cd = true;
    cd &= fullpath.cd("steamapps");
    if (cd) cd &= fullpath.cd("common");
    if (cd) cd &= fullpath.cd(QString::fromUtf8(MHW_FOLDER_NAME));
    if (!cd) path = NULL;
    else path = fullpath.path();
  }

  return path;
}

QString Settings::GetDefaultSaveDir()
{
  QSettings regUsers(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam\Users)", QSettings::NativeFormat);
  QStringList users = regUsers.childGroups();

  QString path = GetSteamPath(), user;
  if (users.length() > 0)
    user = users.at(0);

  if (!path.isNull()) {
    QDir fullpath(path);
    bool cd = true;
    cd &= fullpath.cd("userdata");
    if (cd) cd &= fullpath.cd(user);
    if (cd) cd &= fullpath.cd(QString::fromUtf8(MHW_ID));
    if (cd) cd &= fullpath.cd("remote");
    if (!cd) cd |= fullpath.cd(QDir::homePath());
    assert(cd);

    path = fullpath.path();
  }
  else {
    path = QDir::homePath();
  }

  qDebug(qUtf8Printable(path));
  return path;
}

QString Settings::GetDefaultSavePath()
{
  return GetDefaultSaveDir() + "/" + QString::fromUtf8(SAVE_NAME);
}

QString Settings::GetDefaultDumpPath(int slot)
{
  assert(slot >= 0 && slot <= 9);
  QString path = GetDefaultSavePath();
  int last = path.length() - 1;
  path[last] = QChar('0' + slot);
  return path + ".bin";
}

QString Settings::GetDataPath()
{
  QString path = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation)[0];

  QDir dir = QDir();
  if (dir.mkpath(path)) return path;
  else return "";
}


QString Settings::GetDataPathBackups()
{
  QString path = GetDataPath();
  path = path + "/backups/";

  QDir dir = QDir();
  if (dir.mkpath(path)) return path;
  else return "";
}

QString Settings::GetIconDumpPath()
{
  QString path = GetDataPath();
  path = path + "/icons/";

  QDir dir = QDir();
  if (dir.mkpath(path)) return path;
  else return "";
}
