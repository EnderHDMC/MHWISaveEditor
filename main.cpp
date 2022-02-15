#include "gui/mhwisaveeditor.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include "data/BitmapDB.h"
#include "data/ItemDB.h"

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("EnderHDMC");
  QCoreApplication::setOrganizationDomain("enderhdmc.github.io");
  QCoreApplication::setApplicationName("MHWI Save Editor");

  Settings* settings = settings->GetInstance();
  ItemDB* itemDB = new ItemDB();
  BitmapDB* bitmapDB = new BitmapDB(itemDB);

  QString uiLanguage = settings->GetUiLanguage();
  QTranslator translator;
  if (!uiLanguage.isEmpty()) {
    const QString trFile = "mhwisaveeditor_" + uiLanguage + ".qm";
    if (translator.load("res/translations/" + trFile)) {
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
