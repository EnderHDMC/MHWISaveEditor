#include "gui/mhwisaveeditor.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>

#include "data/BitmapDB.h"
#include "data/EquipmentDB.h"
#include "data/ItemDB.h"
#include "data/SmithyDB.h"

#include "platform/platform.h"

static void MessagePrintLog(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  static QString logPath = Paths::GetDataPath() + "/EnderHDMC/MHWI Save Editor/MHWISaveEditor.log";
  static QFile logFile(logPath);
  if (!logFile.isOpen()) logFile.open(QIODevice::WriteOnly);
  static QTextStream logger(&logFile);

  QString typeStr = "None";
  FILE* output = stdout;
  switch (type) {
  case QtDebugMsg:    typeStr = "Debug";    output = stderr; break;
  case QtWarningMsg:  typeStr = "Warning";  output = stderr; break;
  case QtCriticalMsg: typeStr = "Critical"; output = stderr; break;
  case QtFatalMsg:    typeStr = "Fatal";    output = stderr; break;
  case QtInfoMsg:     typeStr = "Info";     output = stdout; break;
  }

  QByteArray printMessage = QString("%1: %2\n").arg(typeStr).arg(msg).toLocal8Bit();
  fprintf(output, "%s", printMessage.constData());
  logger << printMessage;
  logger.flush();
}

int main(int argc, char* argv[])
{
  Platform::Init();
  Settings* settings = settings->GetInstance();

  bool debugger = Settings::DebuggerPresent();
  bool showConsole = settings->GetShowConsole();
  if (!debugger) qInstallMessageHandler(MessagePrintLog);
  if (!debugger && showConsole) Platform::OpenConsole();

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
