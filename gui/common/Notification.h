#pragma once

#include <QMap>
#include <QStatusBar>
#include <QMessageBox>

enum class NotificationMode : int
{
  None = -1,
  MessageBox = 0,
  StatusBar = 1
};

class Notification
{
private:
  static Notification* instance;
  Notification();

  NotificationMode defaultMode = NotificationMode::None;
  QMessageBox* messageBox = nullptr;
  QStatusBar* statusBar = nullptr;

  QMap<NotificationMode, QObject*> notificationModes;
  unsigned int silence = 0;

public:
  static Notification* GetInstance();
  void Free();

  NotificationMode GetDefaultMode();
  bool SetDefaultMode(NotificationMode mode);
  void ShowMessage(const QString& text, int timeout = 0);
  void Register(QStatusBar* statusbar);

  void Silence(unsigned int times);
};

