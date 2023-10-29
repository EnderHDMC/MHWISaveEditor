#pragma once

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <Windows.h>

class Platform
{
public:
  static bool OpenConsole();
};
#endif // Q_OS_WIN
