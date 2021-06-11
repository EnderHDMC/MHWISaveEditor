#include "mhwisaveeditor.h"
#include "ui_mhwisaveeditor.h"

#include <QFileDialog>
#include <QSaveFile>
#include <QByteArray>
#include <QDesktopServices>
#include <QGridLayout>

// Encryption
#include "crypto/iceborne_crypt.h"
// Save paths
#include "utility/paths.h"
#include "utility/system/FileUtils.h"
// Inventory Layout
#include "types/inventory_areas.h"
// Item Data
#include "data/ItemDB.h"
#include "data/BitmapDB.h"
#include "inventoryeditor.h"

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
  connect(slotSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(Slot(int)));

  openSignalMapper = new QSignalMapper(this);
  connect(ui->actionOpenGameLocation, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenGameLocation, GetGamePath());
  connect(ui->actionOpenSaveLocation, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenSaveLocation, GetDefaultSavePath() + "/" + QString::fromUtf8(SAVE_NAME));

  connect(openSignalMapper, SIGNAL(mappedString(const QString&)), this, SLOT(OpenLocation(const QString&)));

  int count = COUNTOF(areas);
  inventoryEditors.resize(count);
  for (size_t i = 0; i < count; i++)
  {
    QWidget* newTab = new QWidget(ui->tabWidget);

    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    InventoryEditor* editor = new InventoryEditor(&areas[i], newTab);
    layout->addWidget(editor);
    newTab->setLayout(layout);
    inventoryEditors[i] = editor;

    ui->tabWidget->addTab(newTab, tr(areas[i].area));
  }

  mhwRaw = nullptr;
}

MHWISaveEditor::~MHWISaveEditor()
{
  delete ui;

  free(mhwRaw);
  mhwRaw = nullptr;
}

void MHWISaveEditor::closeEvent(QCloseEvent* event)
{
  ItemDB* itemDB = itemDB->GetInstance();
  itemDB->Free();
  BitmapDB* bitmapDB = bitmapDB->GetInstance();
  bitmapDB->Free();
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
  int result = dialog.exec();

  if (result) {
    QStringList files = dialog.selectedFiles();
    filepath = files[0];
    qInfo("%s", qUtf8Printable(filepath));

    QFile file(filepath, this);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning("File: %s, cannot be read.", qUtf8Printable(filepath));
      return;
    }

    QByteArray saveBlob = file.readAll();
    if (saveBlob.length() != sizeof(mhw_save_raw)) {
      qWarning("File: %s, cannot be read.", qUtf8Printable(filepath));
      return;
    }
    file.close();

    if (mhwRaw) free(mhwRaw);
    mhwRaw = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
    if (!mhwRaw) {
      qWarning("Error allocating memory.");
      return;
    };

    memcpy(mhwRaw->data, saveBlob.constData(), saveBlob.length());
    if (!IsBlowfishDecrypted(&mhwRaw->save)) {
      DecryptSave(mhwRaw->data, sizeof(mhw_save_raw));
    }
  }

  // Load the save into the inventory slots
  if (result) {
    for (size_t i = 0; i < inventoryEditors.count(); i++)
    {
      InventoryEditor* editor = inventoryEditors[i];
      editor->Load(mhwRaw, 0);
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

    mhw_save_raw* saveWrite = nullptr;
    if (selectedFilter == tr(ENCRYPTED_SAVE) || selectedFilter == tr(ALL_SAVE)) {
      saveWrite = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
      if (!saveWrite) {
        qInfo("Error allocating memory.");
        return;
      }
      memcpy(saveWrite, mhwRaw, sizeof(mhw_save_raw));
      EncryptSave(saveWrite->data, sizeof(mhw_save_raw));
    }
    else if (selectedFilter == tr(UNENCRYPTED_SAVE)) {
      saveWrite = mhwRaw;
    }

    assert(saveWrite);
    int length = file.write((char*)saveWrite->data, sizeof(mhw_save_raw));
    if (length != sizeof(mhw_save_raw)) {
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

void MHWISaveEditor::OpenLocation(const QString& location)
{
  qInfo("Opening: %s", qUtf8Printable(location));
  FileUtils::showInGraphicalShell(location);
}

void MHWISaveEditor::Backup() {

}

void MHWISaveEditor::Restore() {

}