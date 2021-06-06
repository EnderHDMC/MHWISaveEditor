#include "mhwisaveeditor.h"
#include "ui_mhwisaveeditor.h"

#include <QFileDialog>
#include <QSaveFile>
#include <QByteArray>

#include "crypto/iceborne_crypt.h"
#include "utility/paths.h"

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

  mhwRaw = nullptr;
}

MHWISaveEditor::~MHWISaveEditor()
{
  delete ui;

  free(mhwRaw);
  mhwRaw = nullptr;
}

void MHWISaveEditor::Open()
{
  qInfo("Hello from Open");

  QString path = GetDefaultSavePath();
  QString filepath = QString();

  QFileDialog dialog(this);
  dialog.setDirectory(path);
  dialog.selectFile("SAVEDATA1000");
  dialog.setFileMode(QFileDialog::ExistingFile);

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
    file.close();

    if (mhwRaw) free(mhwRaw);
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

  if (!mhwRaw) return;
  QString path = GetDefaultSavePath();
  QString filepath = QString();

  const QStringList filters({ tr(ALL_SAVE),
                              tr(ENCRYPTED_SAVE),
                              tr(UNENCRYPTED_SAVE) });

  QFileDialog dialog(this);
  dialog.setDirectory(path);
  dialog.setNameFilters(filters);
  dialog.setAcceptMode(QFileDialog::AcceptSave);

  if (dialog.exec()) {
    QString selectedFilter = dialog.selectedNameFilter();
    QStringList files = dialog.selectedFiles();
    filepath = files[0];
    if (selectedFilter == tr(ALL_SAVE)) {
      QFileInfo fileInfo(filepath);
      filepath = fileInfo.absolutePath() + "/" + fileInfo.baseName();
    }
    qInfo("%s", qUtf8Printable(filepath));

    QSaveFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning("File: %s, cannot be written.", qUtf8Printable(filepath));
      return;
    }

    MHWSaveRaw* saveWrite = nullptr;
    if (selectedFilter == tr(ENCRYPTED_SAVE) || selectedFilter == tr(ALL_SAVE)) {
      saveWrite = (MHWSaveRaw*)malloc(sizeof(MHWSaveRaw));
      if (!saveWrite) {
        qInfo("Error allocating memory.");
        return;
      }
      memcpy(saveWrite, mhwRaw, sizeof(MHWSaveRaw));
      EncryptSave(saveWrite->data, sizeof(MHWSaveRaw));
    }
    else if (selectedFilter == tr(UNENCRYPTED_SAVE)) {
      saveWrite = mhwRaw;
    }

    assert(saveWrite);
    int length = file.write((char*)saveWrite->data, sizeof(MHWSaveRaw));
    if (length != sizeof(MHWSaveRaw)) {
      qWarning("File: %s, cannot be written.", qUtf8Printable(filepath));
      return;
    }
    file.commit();

    if (saveWrite != mhwRaw) free(saveWrite);
  }
}

void MHWISaveEditor::Slot(int slot)
{
  qInfo("Selected slot index %d.", slot);
}
