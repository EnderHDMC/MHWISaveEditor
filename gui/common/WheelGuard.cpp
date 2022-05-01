#include <QEvent>
#include <QWidget>

// Thanks to StackOverflow: https://stackoverflow.com/a/37460760
#include "WheelGuard.h"

WheelGuard* WheelGuard::instance = nullptr;

WheelGuard::WheelGuard(QObject* parent) : QObject(parent)
{
}

WheelGuard* WheelGuard::GetInstance(QObject* parent)
{
  if (!instance) instance = new WheelGuard(nullptr);
  return instance;
}

void WheelGuard::Free()
{
  delete instance;
  instance = nullptr;
}

bool WheelGuard::eventFilter(QObject* o, QEvent* e)
{
  const QWidget* widget = static_cast<QWidget*>(o);
  if (e->type() == QEvent::Wheel && widget && !widget->hasFocus())
  {
    e->ignore();
    return true;
  }

  return QObject::eventFilter(o, e);
}
