#pragma once

#include <QString>
#include <QSettings>
#include <QDir>

#include "../types/constants.h"

static QString GetDefaultSavePath()
{
  QSettings regSteam(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)", QSettings::NativeFormat);
  QSettings regUsers(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam\Users)", QSettings::NativeFormat);
  QStringList users = regUsers.childGroups();

  QString path, user;
  if (users.length() > 0)
    user = users.at(0);

  if (regSteam.childKeys().contains("SteamPath")) {
    QDir fullpath(regSteam.value("SteamPath").toString());

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