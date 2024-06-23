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

    qInfo() << "Registry Query:" << path << "Member:" << member;
    qInfo() << "Registry Value:" << value.toString();
    return value;
  }

  static inline const QStringList Groups(const QString& path) {
    QStringList groups = QSettings(path, QSettings::NativeFormat).childGroups();
    qInfo() << "Registry Groups:" << path;
    qInfo() << groups.join('\n');

    return groups;
  }
};
