#include <QEvent>
#include <QWidget>

// Thanks to StackOverflow: https://stackoverflow.com/a/37460760
#include "WheelGuard.h"

WheelGuard::WheelGuard(QObject* parent) : QObject(parent)
{
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