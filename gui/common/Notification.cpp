#include "Notification.h"

#include <QIcon>

Notification* Notification::instance = nullptr;

Notification::Notification()
{
  notificationModes.insert(NotificationMode::NotifModeInvalid, nullptr);
  notificationModes.insert(NotificationMode::NotifModeNone, nullptr);
  notificationModes.insert(NotificationMode::NotifModeStatusBar, statusBar);
  notificationModes.insert(NotificationMode::NotifModeMessageBox, messageBox);
}

Notification* Notification::GetInstance()
{
  if (!instance) instance = new Notification();
  return instance;
}

void Notification::Free()
{
  delete(instance);
  instance = nullptr;
}

bool Notification::SetDefaultMode(NotificationMode mode)
{
  QObject* system = notificationModes.value(mode, nullptr);
  if (system) defaultMode = mode;
  return system;
}

NotificationMode Notification::GetDefaultMode()
{
  return defaultMode;
}

void Notification::ShowMessage(const QString& text, int timeout)
{
  if (silence) { silence--; return; }
  NotificationMode messageMode = defaultMode;
  if (!modeStack.isEmpty()) messageMode = modeStack.top();

  switch (messageMode)
  {
  case NotificationMode::NotifModeInvalid: break;
  case NotificationMode::NotifModeNone: break;

  case NotificationMode::NotifModeMessageBox: {
    if (messageBox) {
      messageBox->setText(text);
      messageBox->exec();
    }
  } break;

  case NotificationMode::NotifModeStatusBar: {
    if (statusBar) {
      statusBar->showMessage(text, timeout);
    }
  } break;

  default:
    break;
  }
}

void Notification::Register(QMessageBox* messageBox)
{
  this->messageBox = messageBox;
  notificationModes.insert(NotificationMode::NotifModeMessageBox, messageBox);
}

void Notification::Register(QStatusBar* statusbar)
{
  this->statusBar = statusbar;
  notificationModes.insert(NotificationMode::NotifModeStatusBar, statusBar);
}

void Notification::Silence(unsigned int times)
{
  silence = times;
}

bool Notification::PushMode(NotificationMode mode)
{
  bool canPush = modeStack.count() < 16;
  if (canPush) {
    modeStack.push(mode);
  }

  return canPush;
}

NotificationMode Notification::PopMode()
{
  NotificationMode result = NotificationMode::NotifModeInvalid;
  if (!modeStack.isEmpty()) {
    result = modeStack.pop();
  }

  return result;
}
