#include "gui/mhwisaveeditor.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>

#include "data/BitmapDB.h"
#include "data/ItemDB.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif // Q_OS_WIN

int main(int argc, char* argv[])
{
  bool debugger = false;

#ifdef Q_OS_WIN
  debugger = IsDebuggerPresent();
  qInfo("Debugger: %d", debugger);
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
