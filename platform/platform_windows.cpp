#include "platform_windows.h"
#ifdef Q_OS_WIN

#include <stdio.h>

#include "oodle.h"

HINSTANCE Platform::hinst_oo2core = nullptr;

HINSTANCE Platform::LoadOo2core(Proto_OodleLZ_Decompress* OodleLZDecompress)
{
  HINSTANCE hinstOo2core;
  bool linkSuccess = true;

  hinstOo2core = LoadLibrary(L"oo2core_9_win64_f2db01967705b62aecef3cd3e5a28e4d.dll");
  linkSuccess &= (bool)hinstOo2core;

  if (hinstOo2core)
  {
    Proto_OodleLZ_Decompress decompress = (Proto_OodleLZ_Decompress)GetProcAddress(hinstOo2core, "OodleLZ_Decompress");
    *OodleLZDecompress = decompress;
    linkSuccess &= (bool)decompress;
  }

  return hinstOo2core;
}

void Platform::Init()
{
  hinst_oo2core = LoadOo2core(&Oodle::OodleLZ_Decompress);
}

bool Platform::OpenConsole()
{
  bool console = false;

  console = AllocConsole();
  if (console) {
    UINT codepage = CP_UTF8;
    SetConsoleCP(codepage);
    SetConsoleOutputCP(codepage);

    errno_t error = 0;
    FILE* file_stdout = nullptr;
    FILE* file_stderr = nullptr;
    FILE* file_stdin = nullptr;
    error = freopen_s(&file_stdout, "CONOUT$", "w", stdout);
    error = freopen_s(&file_stderr, "CONOUT$", "w", stderr);
    error = freopen_s(&file_stdin, "CONOUT$", "r", stdin);

    HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE stderrHandle = GetStdHandle(STD_ERROR_HANDLE);

    DWORD flags = ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(stdinHandle, flags);
    SetConsoleMode(stdoutHandle, flags);
    SetConsoleMode(stderrHandle, flags);

    HWND consoleHandle = GetConsoleWindow();
    LONG newLong = GetWindowLongW(consoleHandle, GWL_EXSTYLE) | WS_EX_LAYERED;
    SetWindowLongW(consoleHandle, GWL_EXSTYLE, newLong);
    SetLayeredWindowAttributes(consoleHandle, 0, 0xf0, LWA_ALPHA);
  }

  return console;
}
#endif // Q_OS_WIN
