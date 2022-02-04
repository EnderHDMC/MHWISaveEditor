#include "Notification.h"

#include <QIcon>

Notification* Notification::instance = nullptr;

Notification::Notification()
{
  messageBox = new QMessageBox();
  messageBox->setIcon(QMessageBox::Icon::Information);
  messageBox->setWindowTitle("Notification");

  notificationModes.insert(NotificationMode::None, nullptr);
  notificationModes.insert(NotificationMode::StatusBar, statusBar);
  notificationModes.insert(NotificationMode::MessageBox, messageBox);
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
  QObject *system = notificationModes.value(mode, nullptr);
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

  switch (defaultMode)
  {
  case NotificationMode::None: break;

  case NotificationMode::MessageBox: {
    if (messageBox) {
      messageBox->setText(text);
      messageBox->exec();
    }
  } break;

  case NotificationMode::StatusBar: {
    if (statusBar) {
      statusBar->showMessage(text, timeout);
    }
  } break;

  default:
    break;
  }
}

void Notification::Register(QStatusBar* statusbar)
{
  statusBar = statusbar;
  notificationModes.insert(NotificationMode::StatusBar, statusBar);
}

void Notification::Silence(unsigned int times)
{
  silence = times;
}
