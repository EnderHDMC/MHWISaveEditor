#pragma once

// Thanks to StackOverflow: https://stackoverflow.com/a/37460760
#include <QObject>

class WheelGuard : public QObject
{
public:
  explicit WheelGuard(QObject* parent);

protected:
  bool eventFilter(QObject* o, QEvent* e) override;
};