#include "gui/mhwisaveeditor.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTranslator>
#include <QLocale>
#include <QDir>

#include <QFile>
#include <QSaveFile>
#include <QTextStream>

#include "crypto/iceborne_crypt.h"
#include "utility/mhw_save_utils.h"

#include "data/BitmapDB.h"
#include "data/EquipmentDB.h"
#include "data/ItemDB.h"
#include "data/SmithyDB.h"

#include "platform/platform.h"
#include "types/constants.h"

struct MWHI_Arguments
{
  QString input;
  QString encryptTarget;
  QString decryptTarget;
  bool showGui;

  QString message;
  bool error = false;
};

static QStringList listArguments(int argc, char* argv[]) {
  QStringList opts(argc);
  for (int i = 0; i < argc; ++i)
    opts[i] = QString::fromLocal8Bit(argv[i]);
  return opts;
}

static void parseCommandLine(const QStringList& opts, MWHI_Arguments* options)
{
  QCommandLineParser parser;
  const QCommandLineOption help = parser.addHelpOption();
  const QCommandLineOption version = parser.addVersionOption();

  const QCommandLineOption decrypt({"d", "decrypt"}, "Decrypt to file", "file");
  parser.addOption(decrypt);
  const QCommandLineOption encrypt({"e", "encrypt"}, "Encrypt to file", "file");
  parser.addOption(encrypt);

  parser.addPositionalArgument("input", "Save to convert", "[input]");

  if (!parser.parse(opts)) {
    options->error = true;
    options->message = parser.errorText();
  }
  else if (parser.isSet(help)) {
    options->message = parser.helpText().trimmed();
  }
  else if (parser.isSet(version)) {
    options->message = QCoreApplication::applicationVersion();
  }
  else if (parser.isSet(encrypt) || parser.isSet(decrypt)) {
    const auto inputs = parser.positionalArguments();
    options->input = inputs.value(0);
    options->encryptTarget = parser.value(encrypt);
    options->decryptTarget = parser.value(decrypt);
    if (inputs.size() != 1 || options->input.isEmpty() ||
        (parser.isSet(encrypt) && parser.isSet(decrypt)) ||
        (options->encryptTarget.isEmpty() && options->decryptTarget.isEmpty())) {
      options->error = true;
      options->message = "Specify one input and one output using either --encrypt or --decrypt.";
    }
  }
  else {
    options->showGui = true;
  }
}

static void MessagePrintLog(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  static QString logPath = Paths::GetDataPath("MHWISaveEditor.log");
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

static void handleGuiLogs() {
  bool debugger = Settings::DebuggerPresent();

  Settings* settings = settings->GetInstance();
  bool showConsole = settings->GetShowConsole();
  if (!debugger) qInstallMessageHandler(MessagePrintLog);
  if (!debugger && showConsole) Platform::OpenConsole();
  qInfo("Debugger present: %d", debugger);
}

static int gui(int argc, char* argv[]) {
  handleGuiLogs();

  QApplication a(argc, argv);
  QDir::setCurrent(qApp->applicationDirPath());
  qInfo("Current path: %s", qUtf8Printable(QDir::currentPath()));

  ItemDB* itemDB = new ItemDB();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  SmithyDB* smithyDB = smithyDB->GetInstance();
  BitmapDB* bitmapDB = new BitmapDB(itemDB, equipmentDB);

  Settings* settings = settings->GetInstance();
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

static int console(int argc, char* argv[], const MWHI_Arguments& options)
{
  Platform::OpenConsole();
  QCoreApplication app(argc, argv);
  
  if (!options.message.isEmpty()) {
    QTextStream stream(options.error ? stderr : stdout);
    stream << options.message << Qt::endl;
    return options.error ? 1 : 0;
  }

  QTextStream errors(stderr);
  QFile input(options.input);
  if (!input.open(QIODevice::ReadOnly)) {
    errors << options.input << ": " << input.errorString() << Qt::endl;
    return 1;
  }

  const qint64 size = input.size();
  if (size != sizeof(mhw_ib_save) && size != sizeof(mhw_ps4_save)) {
    errors << options.input << ": Unsupported save size." << Qt::endl;
    return 1;
  }

  QByteArray data = input.read(size + 1);
  if (data.size() != size || input.error() != QFileDevice::NoError) {
    errors << options.input << ": Could not read the complete save." << Qt::endl;
    return 1;
  }
  input.close();

  const bool encrypt = !options.encryptTarget.isEmpty();
  u8* bytes = (u8*)data.data();
  const int length = (int)size;

  if (size == sizeof(mhw_ib_save)) {
    mhw_ib_save* save = (mhw_ib_save*)bytes;
    const bool decrypted = MHWSaveUtils::IsBlowfishDecrypted(save);
    if (!decrypted && !DecryptSave(bytes, length)) {
      errors << options.input << ": Save checksum verification failed." << Qt::endl;
      return 1;
    }
    if (encrypt) EncryptSave(bytes, length);
  }
  else {
    const bool decrypted = MHWSaveUtils::IsBlowfishDecryptedPS4((mhw_ps4_save*)bytes);
    if (encrypt && decrypted) EncryptSavePS4(bytes, length);
    else if (!encrypt && !decrypted) DecryptSavePS4(bytes, length);
  }

  const QString& target = encrypt ? options.encryptTarget : options.decryptTarget;
  QSaveFile output(target);
  if (!output.open(QIODevice::WriteOnly) || output.write(data) != size || !output.commit()) {
    errors << target << ": " << output.errorString() << Qt::endl;
    return 1;
  }
  return 0;
}

int main(int argc, char* argv[])
{
#if 0
  while (!Settings::DebuggerPresent()) {
    Sleep(1000);
  }
  __debugbreak();
#endif

  QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
  QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
  QCoreApplication::setApplicationName(APPLICATION_NAME);
  QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

  MWHI_Arguments options{};
  QStringList args = listArguments(argc, argv);
  parseCommandLine(args, &options);

  int ret = 0;
  if (options.showGui) {
    ret = gui(argc, argv);
  }
  else {
    ret = console(argc, argv, options);
  }

  return ret;
}
