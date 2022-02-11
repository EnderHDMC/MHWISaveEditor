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

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString& locale : uiLanguages) {
    const QString baseName = "MHWISaveEditor_" + QLocale(locale).name();
    if (translator.load(":/i18n/" + baseName)) {
      a.installTranslator(&translator);
      break;
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
