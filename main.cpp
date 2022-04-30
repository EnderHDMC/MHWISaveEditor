#include "gui/mhwisaveeditor.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>

#include "data/BitmapDB.h"
#include "data/EquipmentDB.h"
#include "data/ItemDB.h"
#include "data/SmithyDB.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif // Q_OS_WIN

bool OpenConsole()
{
  bool console = false;

#ifdef Q_OS_WIN
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
#endif // Q_OS_WIN

  return console;
}

int main(int argc, char* argv[])
{
  Settings* settings = settings->GetInstance();

  bool debugger = Settings::DebuggerPresent();
  bool showConsole = settings->GetShowConsole();
  if (!debugger && showConsole) OpenConsole();

  qInfo("Debugger present: %d", debugger);
  settings->LogReadPath();

  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("EnderHDMC");
  QCoreApplication::setOrganizationDomain("enderhdmc.github.io");
  QCoreApplication::setApplicationName("MHWI Save Editor");

  QDir::setCurrent(qApp->applicationDirPath());
  qInfo("Current path: %s", qUtf8Printable(QDir::currentPath()));

  ItemDB* itemDB = new ItemDB();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  SmithyDB* smithyDB = smithyDB->GetInstance();
  BitmapDB* bitmapDB = new BitmapDB(itemDB, equipmentDB);

  QString uiLanguage = settings->GetUiLanguage();
  QTranslator translator;
  if (!uiLanguage.isEmpty()) {
    QString trFile = "mhwisaveeditor_" + uiLanguage + ".qm";
    QString translationPath = Paths::GetResourcesPath("translations/");
    if (translator.load(translationPath + trFile)) {
      a.installTranslator(&translator);
    }
  }

  MHWISaveEditor w;
  w.LoadResources(itemDB, bitmapDB);
  w.show();
  int ret = a.exec();

  delete bitmapDB;
  equipmentDB->Free();
  smithyDB->Free();
  delete itemDB;

  return ret;
}
