#pragma once

#include <QFileDialog>
#include <QProcess>

class FileUtils {
public:
  static const void showInGraphicalShell(const QString& path)
  {
#ifdef Q_OS_WIN
    QStringList args;

    QFileInfo fileInfo(path);
    if (!fileInfo.isDir())
      args << "/select,";

    args << QDir::toNativeSeparators(path);
    QProcess::startDetached("explorer", args);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    return;
#endif
  }
};
