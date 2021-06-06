#pragma once

#include <QString>
#include <QSettings>
#include <QDir>

#include "../types/constants.h"

static QString GetSteamPath()
{
  QSettings regSteam(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)", QSettings::NativeFormat);
  if (regSteam.childKeys().contains("SteamPath"))
    return regSteam.value("SteamPath").toString();

  return NULL;
}

static QString GetGamePath()
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

static QString GetDefaultSavePath()
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
