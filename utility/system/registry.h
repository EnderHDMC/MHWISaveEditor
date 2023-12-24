#pragma once

#include <QString>
#include <QSettings>

class Registry {
public:
  static inline const QVariant Query(const QString& path, const QString& member) {
    QVariant value = NULL;
    QSettings regSteam(path, QSettings::NativeFormat);
    if (regSteam.childKeys().contains(member))
      value = regSteam.value(member);

    return value;
  }

  static inline const QStringList Groups(const QString& path) {
    return QSettings(path, QSettings::NativeFormat).childGroups();
  }
};
