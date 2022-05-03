#pragma once

#include <QMap>
#include <QStack>
#include <QStatusBar>
#include <QMessageBox>

enum class NotificationMode : int
{
  NotifModeInvalid = -1,
  NotifModeNone = 0,
  NotifModeMessageBox = 1,
  NotifModeStatusBar = 2
};

class Notification
{
private:
  static Notification* instance;
  Notification();

  QStack<NotificationMode> modeStack;

  NotificationMode defaultMode = NotificationMode::NotifModeNone;
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
  bool PushMode(NotificationMode mode);
  NotificationMode PopMode();
};

