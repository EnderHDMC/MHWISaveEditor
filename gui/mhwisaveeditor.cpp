#include "mhwisaveeditor.h"
#include "ui_mhwisaveeditor.h"

#include <QFileDialog>
#include <QSaveFile>
#include <QByteArray>
#include <QDesktopServices>
#include <QGridLayout>

#include "inventoryeditor.h"

// Encryption
#include "../crypto/iceborne_crypt.h"
// Save paths
#include "../utility/paths.h"
#include "../utility/system/FileUtils.h"
// Inventory Layout
#include "../types/inventory_areas.h"
// Item Data
#include "../data/ItemDB.h"
#include "../data/BitmapDB.h"

MHWISaveEditor::MHWISaveEditor(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MHWISaveEditor)
{
  ui->setupUi(this);

  slotSignalMapper = new QSignalMapper(this);
  slotActions = { ui->actionSlot1 , ui->actionSlot2, ui->actionSlot3 };
  for (int i = 0; i < slotActions.size(); ++i) {
    slotActions[i]->setChecked(i == saveslot);

    connect(slotActions[i], SIGNAL(triggered()), slotSignalMapper, SLOT(map()));
    slotSignalMapper->setMapping(slotActions[i], i);
  }
  connect(slotSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(Slot(int)));

  openSignalMapper = new QSignalMapper(this);
  connect(ui->actionOpenGameLocation, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenGameLocation, GetGamePath());
  connect(ui->actionOpenSaveLocation, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenSaveLocation, GetDefaultSaveDir() + "/" + QString::fromUtf8(SAVE_NAME));
  connect(openSignalMapper, SIGNAL(mappedString(const QString&)), this, SLOT(OpenLocation(const QString&)));

  dumpSignalMapper = new QSignalMapper(this);
  QList<QAction*> slotDump = {
    ui->actionSAVEDATA1000bin, ui->actionSAVEDATA1001bin,
    ui->actionSAVEDATA1002bin, ui->actionSAVEDATA1003bin,
    ui->actionSAVEDATA1004bin, ui->actionSAVEDATA1005bin,
    ui->actionSAVEDATA1006bin, ui->actionSAVEDATA1007bin,
    ui->actionSAVEDATA1008bin, ui->actionSAVEDATA1009bin
  };

  for (int i = 0; i < slotDump.size(); ++i) {
    connect(slotDump[i], SIGNAL(triggered()), dumpSignalMapper, SLOT(map()));
    dumpSignalMapper->setMapping(slotDump[i], i);
  }
  connect(dumpSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(Dump(int)));

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

void MHWISaveEditor::SaveFile(const QString& path, mhw_save_raw** save, bool encrypt = true)
{
  qInfo("Saving: %s", qUtf8Printable(path));
  qInfo("Encrypted: %s", encrypt ? "True" : "False");

  mhw_save_raw* savep = *save;
  mhw_save_raw* saveWrite = savep;
  if (encrypt) {
    saveWrite = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
    if (!saveWrite) {
      qInfo("Error allocating memory.");
      return;
    }

    memcpy(saveWrite, savep, sizeof(mhw_save_raw));
    EncryptSave(saveWrite->data, sizeof(mhw_save_raw));
  }
  assert(saveWrite);

  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning("File: %s, cannot be written.", qUtf8Printable(path));
    return;
  }

  int length = file.write((char*)saveWrite->data, sizeof(mhw_save_raw));
  if (length != sizeof(mhw_save_raw)) {
    qWarning("File: %s, cannot be written.", qUtf8Printable(path));
    return;
  }
  file.commit();

  if (saveWrite != savep) free(saveWrite);
}

void MHWISaveEditor::LoadFile(const QString& path, mhw_save_raw** save)
{
  qInfo("Loading %s", qUtf8Printable(path));
  mhw_save_raw* savep = *save;

  QFile file(path, this);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning("File: %s, cannot be read.", qUtf8Printable(path));
    return;
  }

  QByteArray saveBlob = file.readAll();
  if (saveBlob.length() != sizeof(mhw_save_raw)) {
    qWarning("File: %s, cannot be read.", qUtf8Printable(path));
    return;
  }
  file.close();

  if (savep) free(savep);
  savep = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
  if (!savep) {
    qWarning("Error allocating memory.");
    return;
  };

  memcpy(savep->data, saveBlob.constData(), saveBlob.length());
  if (!IsBlowfishDecrypted(&savep->save)) {
    DecryptSave(savep->data, sizeof(mhw_save_raw));
  }
  *save = savep;
}

void MHWISaveEditor::Dump(int number)
{
  if (mhwRaw) {
    SaveFile(GetDefaultDumpPath(number), &mhwRaw, false);
  }
  else {
    mhw_save_raw* buffer = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
    LoadFile(GetDefaultSavePath(), &buffer);
    SaveFile(GetDefaultDumpPath(number), &buffer, false);
    free(buffer);
  }
}

void MHWISaveEditor::Open()
{
  QString path = GetDefaultSaveDir();
  QString filepath = QString();

  QFileDialog dialog(this);
  dialog.setDirectory(path);
  dialog.selectFile(QString::fromUtf8(SAVE_NAME));
  dialog.setFileMode(QFileDialog::ExistingFile);

  if (dialog.exec()) {
    QStringList files = dialog.selectedFiles();
    filepath = files[0];

    LoadFile(filepath, &mhwRaw);
  }

  // Load the save into the inventory slots
  LoadSaveSlot();
}

void MHWISaveEditor::OpenSAVEDATA1000()
{
  QString path = GetDefaultSavePath();
  LoadFile(path, &mhwRaw);

  // Load the save into the inventory slots
  LoadSaveSlot();
}

void MHWISaveEditor::Save()
{
  if (!mhwRaw) return;
  QString path = GetDefaultSaveDir();

  const QStringList filters({ tr(ALL_SAVE),
                              tr(ENCRYPTED_SAVE),
                              tr(UNENCRYPTED_SAVE) });

  QMap<QString, QString> ext_map;
  ext_map.insert(tr(ALL_SAVE), "");
  ext_map.insert(tr(ENCRYPTED_SAVE), ".raw");
  ext_map.insert(tr(UNENCRYPTED_SAVE), ".bin");

  QMap<QString, bool> encrypt_map;
  encrypt_map.insert("", true);
  encrypt_map.insert(".raw", true);
  encrypt_map.insert(".bin", false);

  QFileDialog dialog(this);
  dialog.setDirectory(path);
  dialog.setNameFilters(filters);
  dialog.setAcceptMode(QFileDialog::AcceptSave);

  if (dialog.exec()) {
    QString selectedFilter = dialog.selectedNameFilter();
    QStringList files = dialog.selectedFiles();

    QFileInfo fi(files[0]);
    QString ext = fi.completeSuffix();
    if (ext.isEmpty()) {
      fi.setFile(fi.filePath() + ext_map.value(selectedFilter));
    }
    else {
      ext = '.' + ext;
    }

    QString filepath = fi.absoluteFilePath();

    bool encrypt = encrypt_map.value(ext, false);
    SaveFile(filepath, &mhwRaw, encrypt);
  }
}

void MHWISaveEditor::LoadSaveSlot()
{
  if (!mhwRaw) return;

  for (size_t i = 0; i < inventoryEditors.count(); i++)
  {
    InventoryEditor* editor = inventoryEditors[i];
    editor->Load(mhwRaw, saveslot);
  }
}

void MHWISaveEditor::Slot(int slot)
{
  qInfo("Selected slot index %d.", slot);
  saveslot = slot;

  for (size_t i = 0; i < slotActions.count(); i++)
  {
    slotActions[i]->setChecked(i == saveslot);
  }

  LoadSaveSlot();
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
