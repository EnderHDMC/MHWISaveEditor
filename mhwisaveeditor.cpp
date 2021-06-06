#include "mhwisaveeditor.h"
#include "ui_mhwisaveeditor.h"

#include <QFileDialog>
#include <QByteArray>
#include <QSettings>

#include "crypto/iceborne_crypt.h"
#include "types/constants.h"

MHWISaveEditor::MHWISaveEditor(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MHWISaveEditor)
{
  ui->setupUi(this);

  slotSignalMapper = new QSignalMapper(this);

  for (int i = 0; i < 3; ++i) {
    QAction* slotAction = new QAction("Slot " + QString::number(i + 1));
    connect(slotAction, SIGNAL(triggered()), slotSignalMapper, SLOT(map()));
    slotSignalMapper->setMapping(slotAction, i);
    ui->menuSlot->addAction(slotAction);
  }

  connect(slotSignalMapper, SIGNAL(mappedInt(int)),
    this, SLOT(Slot(int)));
}

MHWISaveEditor::~MHWISaveEditor()
{
  delete ui;
}

static QString GetDefaultSavePath()
{
  QSettings regSteam(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)", QSettings::NativeFormat);
  QSettings regUsers(R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam\Users)", QSettings::NativeFormat);
  QStringList users = regUsers.childGroups();

  QString path, user;
  if (users.length() > 0)
    user = users.at(0);

  if (regSteam.childKeys().contains("SteamPath")) {
    QDir fullpath(regSteam.value("SteamPath").toString());

    bool cd = true;
    cd &= fullpath.cd("userdata");
    if (cd) cd &= fullpath.cd(user);
    if (cd) cd &= fullpath.cd(QString::fromUtf8(MHW_ID));
    if (cd) cd &= fullpath.cd("remote");

    path = fullpath.path();
  }
  else {
    path = QDir::homePath();
  }

  qDebug(qUtf8Printable(path));
  return path;
}

void MHWISaveEditor::Open()
{
  qInfo("Hello from Open");

  QString path = GetDefaultSavePath();
  QString filepath = QString();

  QFileDialog dialog(this);
  dialog.setDirectory(path);

  if (dialog.exec()) {
    QStringList files = dialog.selectedFiles();
    filepath = files[0];

    qInfo("%s", qUtf8Printable(filepath));

    QFile file(filepath, this);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning("File: %s, cannot be read.", qUtf8Printable(filepath));
      return;
    }

    QByteArray saveBlob = file.readAll();
    if (saveBlob.length() != sizeof(MHWSaveRaw)) {
      qWarning("File: %s, cannot be read.", qUtf8Printable(filepath));
      return;
    }

    mhwRaw = (MHWSaveRaw*)malloc(sizeof(MHWSaveRaw));
    if (!mhwRaw) {
      qWarning("Error allocating memory.");
      return;
    };

    memcpy(mhwRaw->data, saveBlob.constData(), saveBlob.length());

    if (!IsBlowfishDecrypted(&mhwRaw->save)) {
      DecryptSave(mhwRaw->data, sizeof(MHWSaveRaw));
    }
  }
}

void MHWISaveEditor::Save()
{
  qInfo("Hello from Save");
}

void MHWISaveEditor::Slot(int slot)
{
  qInfo("Selected slot index %d.", slot);
}
