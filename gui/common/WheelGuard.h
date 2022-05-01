#pragma once

// Thanks to StackOverflow: https://stackoverflow.com/a/37460760
#include <QObject>

class WheelGuard : public QObject
{
private:
  static WheelGuard* instance;
  explicit WheelGuard(QObject* parent);

public:
  static WheelGuard* GetInstance(QObject* parent = nullptr);
  void Free();


protected:
  bool eventFilter(QObject* o, QEvent* e) override;
};
