#pragma once

#include <QString>
#include <QSettings>

class Registry {
public:
  static inline const QVariant Query(const QString& path, const QString& member) {
    QSettings settings(path, QSettings::NativeFormat);
    QVariant value = settings.value(member);

    qInfo() << "Registry Query:" << path << "Member:" << member;
    qInfo() << "Registry Value:" << value.toString();
    return value;
  }

  static inline const QStringList Groups(const QString& path) {
    QStringList groups = QSettings(path, QSettings::NativeFormat).childGroups();
    qInfo() << "Registry Groups:" << path;
    qInfo() << "Registry Count :" << groups.count();
    if (!groups.isEmpty()) {
      qInfo() << groups.join('\n');
    }

    return groups;
  }
};
