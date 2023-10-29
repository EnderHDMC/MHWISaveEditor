#pragma once

#include <QFileDialog>
#include <QSaveFile>
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

  static bool WriteFileSafe(const QString& path, u8* data, u64 size)
  {
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning("File: %s, cannot be written.", qUtf8Printable(path));
      return false;
    }

    u64 bytesToWrite = size;
    u64 bytesWritten = 0;
    char* writeData = (char*)data;
    while (bytesWritten < bytesToWrite)
    {
      u64 writeSize = bytesToWrite - bytesWritten;
      i64 bytesWrite = file.write(writeData, writeSize);
      if (bytesWrite < 0) {
        qWarning("File: %s, cannot be written.", qUtf8Printable(path));
        file.cancelWriting();
        return false;
      }
      bytesWritten += bytesWrite;
      writeData += bytesWrite;
    }

    file.commit();
    return true;
  }

  static bool WriteFile(const QString& path, u8* data, u64 size)
  {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning("File: %s, cannot be written.", qUtf8Printable(path));
      return false;
    }

    u64 bytesToWrite = size;
    u64 bytesWritten = 0;
    char* writeData = (char*)data;
    while (bytesWritten < bytesToWrite)
    {
      u64 writeSize = bytesToWrite - bytesWritten;
      i64 bytesWrite = file.write(writeData, writeSize);
      if (bytesWrite < 0) {
        qWarning("File: %s, cannot be written.", qUtf8Printable(path));
        file.close();
        return false;
      }
      bytesWritten += bytesWrite;
      writeData += bytesWrite;
    }

    file.close();
    return true;
  }
};
