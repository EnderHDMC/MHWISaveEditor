#include "gui/mhwisaveeditor.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>

#include "data/BitmapDB.h"
#include "data/ItemDB.h"

#ifdef Q_OS_WIN
#include <Windows.h>

void OpenConsole()
{
  bool console = AllocConsole();
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
}
#endif // Q_OS_WIN

int main(int argc, char* argv[])
{
  bool debugger = false;

#ifdef Q_OS_WIN
  debugger = IsDebuggerPresent();
  if (!debugger) OpenConsole();

  qInfo("Debugger present: %d", debugger);
#endif // Q_OS_WIN

  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("EnderHDMC");
  QCoreApplication::setOrganizationDomain("enderhdmc.github.io");
  QCoreApplication::setApplicationName("MHWI Save Editor");

  if (!debugger) {
    QDir::setCurrent(qApp->applicationDirPath());
  }

  Settings* settings = settings->GetInstance();
  ItemDB* itemDB = new ItemDB();
  BitmapDB* bitmapDB = new BitmapDB(itemDB);

  QString uiLanguage = settings->GetUiLanguage();
  QTranslator translator;
  if (!uiLanguage.isEmpty()) {
    QString trFile = "mhwisaveeditor_" + uiLanguage + ".qm";
    QString translationPath = Settings::GetResourcesPath("translations/");
    if (translator.load(translationPath + trFile)) {
      a.installTranslator(&translator);
    }
  }

  MHWISaveEditor w;
  w.LoadResources(itemDB, bitmapDB);
  w.show();
  int ret = a.exec();

  delete itemDB;
  delete bitmapDB;

  return ret;
}
