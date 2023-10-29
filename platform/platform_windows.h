#pragma once

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <Windows.h>

#include "common.h"

class Platform
{
private:
  static HINSTANCE hinst_oo2core;

  static HINSTANCE LoadOo2core(Proto_OodleLZ_Decompress* OodleLZDecompress);

public:
  static void Init();
  static bool OpenConsole();
};
#endif // Q_OS_WIN