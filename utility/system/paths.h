#pragma once

#include <QDir>
#include <QString>
#include <QSettings>
#include <QStandardPaths>

#include "../../types/constants.h"

class Paths
{
public:
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

  static QString GetDefaultSaveDir()
  {
    QSettings regUsers(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam\Users)", QSettings::NativeFormat);
    QStringList users = regUsers.childGroups();
    bool steam = true;

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
      if (!cd) {
        cd |= fullpath.cd(QDir::homePath());
        steam = false;
      }

      path = fullpath.path();
    }
    else {
      path = QDir::homePath();
    }

    qDebug() << "Save Path:" << path << (steam ? "(steam)" : "(home)");
    return path;
  }

  static QString GetDefaultSavePath()
  {
    return GetDefaultSaveDir() + "/" + QString::fromUtf8(SAVE_NAME);
  }

  static QString GetDefaultDumpPath(int slot)
  {
    assert(slot >= 0 && slot <= 9);
    QString path = GetDefaultSavePath();
    int last = path.length() - 1;
    path[last] = QChar('0' + slot);
    return path + ".bin";
  }

  static QString GetDataPath()
  {
    QString path = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation)[0];

    QDir dir = QDir();
    if (dir.mkpath(path)) return path;
    else return "";
  }


  static QString GetDataPathBackups()
  {
    QString path = GetDataPath();
    path = path + "/backups/";

    QDir dir = QDir();
    if (dir.mkpath(path)) return path;
    else return "";
  }

  static QString GetIconDumpPath()
  {
    QString path = GetDataPath();
    path = path + "/icons/";

    QDir dir = QDir();
    if (dir.mkpath(path)) return path;
    else return "";
  }

  static QString GetBasePath() {
    QString result = "";
    static QString cachedBasePath = NULL;

    if (cachedBasePath.isNull()) {
      QDir resSearch = QDir::currentPath();
      bool no_res = !resSearch.cd("res");

      if (no_res) {
        while (no_res) {
          no_res = !resSearch.cd("res");
          if (no_res && !resSearch.cdUp()) break;
          else resSearch.cdUp();
        }
      }
      else resSearch.cdUp();

      if (!no_res)
        result = resSearch.absolutePath();

      cachedBasePath = result;
    }
    else result = cachedBasePath;

    return result;
  }

  static QString GetResourcesPath(const QString& subpath)
  {
    return GetBasePath() + "/res/" + subpath;
  }
};
