#pragma once

#include <QDir>
#include <QString>
#include <QStandardPaths>

#include "../../types/constants.h"

#include "Registry.h"

class Paths
{
public:
  // Game info
  static inline const QString GetGameInstallPath() { return Registry::Query(MHW_INSTALL_REG.c_str(), "InstallLocation").toString(); }

  // Steam info
  static inline const QString GetSteamPath() { return Registry::Query(STEAM_INSTALL_REG.c_str(), "SteamPath").toString(); }

  // Game info
  static inline const QString GetGamePath() {
    static QString selectedGamePath = NULL;
    QString path = selectedGamePath;
    if (path.isNull()) path = GetGameInstallPath();

    if (path.isNull()) {
      QDir fullpath(GetSteamPath());
      bool found = fullpath.cd("steamapps/common/Monster Hunter World");

      if (found) {
        path = fullpath.path();
      }
    }

    if (selectedGamePath.isNull()) selectedGamePath = path;
    return path;
  }

  static QString GetGameSavePath(const QString& user)
  {
    QString path = GetSteamPath();

    if (!path.isNull() && !user.isNull()) {
      QDir fullpath(path);
      QString rel = QString("userdata/%1/%2/remote").arg(user).arg(QString::fromUtf8(MHW_ID));
      fullpath.cd(rel);
      path = fullpath.path();
    }
    else {
      path = QDir::homePath();
    }

    return path;
  }

  static QString GetGameSaveFilePath(const QString& user)
  {
    return GetGameSavePath(user) + "/" + QString::fromUtf8(MHW_SAVE_NAME);
  }

  static QString GetTheoreticalGameSaveFilePath()
  {
    QString path = GetSteamPath();
    QString user = "<SteamAccountID>";

    if (path.isNull()) path = "c:/program files (x86)/steam";
    path = QString("%1/userdata/%2/%3/remote/").arg(path).arg(user).arg(QString::fromUtf8(MHW_ID));
    return path;
  }

  static inline QString GetDumpSlot(int slot) {
    QString dumpFile = QString::fromUtf8(MHW_SAVE_NAME_REL);
    dumpFile[dumpFile.length() - 5] = QChar('0' + slot);
    return dumpFile;
  }

  static QString GetDefaultSaveDumpPath(const QString& user, int slot)
  {
    Q_ASSERT(slot >= 0 && slot <= 9);
    QString path = GetGameSavePath(user);
    QString dumpFile = GetDumpSlot(slot);

    return path.isNull() ? path : path + dumpFile;
  }

  static QString GetSaveDumpPath(const QString& file, const QString& user, int slot)
  {
    // TODO: Use the filepath.
    Q_ASSERT(slot >= 0 && slot <= 9);
    QString defaultPath = GetDefaultSaveDumpPath(user, slot);
    QString dumpFile = GetDumpSlot(slot);
    QDir filePath = file;
    filePath.cdUp();

    QString path = filePath.absolutePath();
    return file.isNull() ? defaultPath : path + dumpFile;
  }

  static QString GetDataPath()
  {
    QString path = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation)[0];

    QDir dir = QDir();
    if (dir.mkpath(path)) return path;
    else return NULL;
  }


  static QString GetDataPathBackups()
  {
    QString path = GetDataPath();
    path = path + "/backups/";

    QDir dir = QDir();
    if (dir.mkpath(path)) return path;
    else return NULL;
  }

  static QString GetBackupPath(const QString& file) {
    QFileInfo fi = QFileInfo(file);
    QString basename = fi.baseName();
    QString ext = fi.completeSuffix();

    if (!ext.isEmpty()) ext = "_" + ext.replace('.', '_');
    if (basename.isEmpty()) basename = QString::fromUtf8(MHW_SAVE_NAME);

    QDateTime date = QDateTime::currentDateTime();
    QString datetime = date.toString("yyyy-MM-dd_hh-mm-ss");
    QString writeFile = basename + ext + '_' + datetime + ".zlib";
    QString path = Paths::GetDataPathBackups() + writeFile;
    return path;
  }

  static QString GetIconDumpPath()
  {
    QString path = GetDataPath();
    path = path + "/icons/";

    QDir dir = QDir();
    if (dir.mkpath(path)) return path;
    else return NULL;
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
