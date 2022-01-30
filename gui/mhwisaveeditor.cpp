#include "mhwisaveeditor.h"
#include "ui_mhwisaveeditor.h"

#include <QFileDialog>
#include <QSaveFile>
#include <QByteArray>
#include <QDesktopServices>
#include <QGridLayout>

// Encryption
#include "../crypto/iceborne_crypt.h"
// Save paths
#include "../utility/paths.h"
#include "../utility/system/FileUtils.h"
#include "../utility/mhw_save_utils.h"
// Inventory Layout
#include "../types/inventory_areas.h"
// Item Data
#include "../data/ItemDB.h"
#include "../data/BitmapDB.h"
#include "common/Notification.h"

MHWISaveEditor::MHWISaveEditor(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::MHWISaveEditor),
  SaveLoader()
{
  ui->setupUi(this);
  setWindowIcon(QIcon("res/icon.ico"));

  Notification* notification = notification->GetInstance();
  notification->Register(ui->statusbar);
  notification->SetDefaultMode(NotificationMode::StatusBar);

  statusFile = new QLabel("File: None", ui->statusbar);
  ui->statusbar->addPermanentWidget(statusFile);

  slotSignalMapper = new QSignalMapper(this);
  switchSignalMapper = new QSignalMapper(this);
  cloneSignalMapper = new QSignalMapper(this);
  selectSlotActions = { ui->actionSlot1 , ui->actionSlot2, ui->actionSlot3 };
  switchSlotActions = { ui->actionSwitchSlot1 , ui->actionSwitchSlot2, ui->actionSwitchSlot3 };
  cloneSlotActions = { ui->actionCloneSlot1, ui->actionCloneSlot2, ui->actionCloneSlot3 };
  for (int i = 0; i < selectSlotActions.size(); i++) {
    connect(selectSlotActions[i], SIGNAL(triggered()), slotSignalMapper, SLOT(map()));
    slotSignalMapper->setMapping(selectSlotActions[i], i);

    connect(switchSlotActions[i], SIGNAL(triggered()), switchSignalMapper, SLOT(map()));
    switchSignalMapper->setMapping(switchSlotActions[i], i);

    connect(cloneSlotActions[i], SIGNAL(triggered()), cloneSignalMapper, SLOT(map()));
    cloneSignalMapper->setMapping(cloneSlotActions[i], i);
  }
  connect(slotSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(SelectSlot(int)));
  connect(switchSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(SwitchSlot(int)));
  connect(cloneSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(CloneSlot(int)));

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

  for (int i = 0; i < slotDump.size(); i++) {
    connect(slotDump[i], SIGNAL(triggered()), dumpSignalMapper, SLOT(map()));
    dumpSignalMapper->setMapping(slotDump[i], i);
  }
  connect(dumpSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(Dump(int)));

  editor_tab editorTabs[] = {
    {new GeneralInfo(), (SaveLoader**)&generalInfo, tr("General")},
    {new HunterInfo(), (SaveLoader**)&hunterInfo, tr("Information")},
    {new InventoryEditor(), (SaveLoader**)&inventoryEditor, tr("Inventory Editor")},
    {new LimitedUnlocks(), (SaveLoader**)&limitedUnlocks, tr("Limited Unlocks")},
  };
  int editorCount = COUNTOF(editorTabs);
  editors.resize(editorCount);

  for (int i = 0; i < editorCount; i++)
  {
    editor_tab* tab = &editorTabs[i];
    SaveLoader* loader = (SaveLoader*)tab->widget;
    QWidget* editor = dynamic_cast<QWidget*>(loader);

    ui->tabWidget->addTab(editor, tab->name);
    if (tab->binding)
      *tab->binding = loader;
    editors[i] = loader;
  }
}

MHWISaveEditor::~MHWISaveEditor()
{
  delete ui;

  SaveLoader::Unload(true);
}

void MHWISaveEditor::closeEvent(QCloseEvent* event)
{
  ItemDB* itemDB = itemDB->GetInstance();
  itemDB->Free();
  BitmapDB* bitmapDB = bitmapDB->GetInstance();
  bitmapDB->Free();
}

bool MHWISaveEditor::SaveFile(const QString& path, mhw_save_raw* save, bool encrypt, bool validate)
{
  qInfo("Saving: %s", qUtf8Printable(path));
  qInfo("Validate: %s", validate ? "True" : "False");
  qInfo("Encrypted: %s", encrypt ? "True" : "False");

  mhw_save_raw* saveWrite = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
  if (!saveWrite) {
    qWarning("Error allocating memory.");
    return false;
  }

  memcpy(saveWrite, save, sizeof(mhw_save_raw));
  if (validate) ValidateSaveFile(&saveWrite->save);
  if (encrypt) EncryptSave(saveWrite->data, sizeof(mhw_save_raw));

  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning("File: %s, cannot be written.", qUtf8Printable(path));
    return false;
  }

  int length = file.write((char*)saveWrite->data, sizeof(mhw_save_raw));
  if (length != sizeof(mhw_save_raw)) {
    qWarning("File: %s, cannot be written.", qUtf8Printable(path));
    return false;
  }
  file.commit();

  if (saveWrite != save) free(saveWrite);
  return true;
}

bool MHWISaveEditor::LoadFile(const QString& path, mhw_save_raw** save)
{
  qInfo("Loading %s", qUtf8Printable(path));
  mhw_save_raw* savep = *save;

  QFile file(path, this);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning("File: %s, cannot be read.", qUtf8Printable(path));
    return false;
  }

  QByteArray saveBlob = file.readAll();
  if (saveBlob.length() != sizeof(mhw_save_raw)) {
    qWarning("File: %s, cannot be read.", qUtf8Printable(path));
    return false;
  }
  file.close();

  if (savep) free(savep);
  savep = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
  if (!savep) {
    qWarning("Error allocating memory.");
    return false;
  };

  memcpy(savep->data, saveBlob.constData(), saveBlob.length());
  if (!IsBlowfishDecrypted(&savep->save)) {
    DecryptSave(savep->data, sizeof(mhw_save_raw));
  }
  *save = savep;
  return true;
}

void MHWISaveEditor::Dump(int number)
{
  mhw_save_raw* savePtr = nullptr;
  mhw_save_raw* buffer = nullptr;
  QString path = GetDefaultDumpPath(number);
  bool success = true;

  if (MHW_SAVE_CHECK) {
    savePtr = this->_mhwSave;
  }
  else {
    buffer = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));

    if (!buffer) {
      success = false;
    }
    else {
      LoadFile(GetDefaultSavePath(), &buffer);
      savePtr = buffer;
    }
  }

  if (success) {
    success = SaveFile(path, savePtr, false);
  }

  Notification* notification = notification->GetInstance();
  if (success)
    notification->ShowMessage(QString("Dumped file: %1").arg(path));
  else
    notification->ShowMessage(QString("Could not dump file: %1").arg(path));

  if (buffer) free(buffer);
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

    LoadFile(filepath);
  }
}

void MHWISaveEditor::OpenSAVEDATA1000()
{
  QString path = GetDefaultSavePath();
  LoadFile(path);
}

void MHWISaveEditor::Save()
{
  MHW_SAVE_GUARD;

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
    bool success = SaveFile(filepath, _mhwSave, encrypt, true);

    Notification* notification = notification->GetInstance();
    if (success)
      notification->ShowMessage(QString("Saved file: %1").arg(path));
    else
      notification->ShowMessage(QString("Could not save file: %1").arg(path));
  }
}

void MHWISaveEditor::LoadFile(const QString& file)
{
  SaveLoader::LoadFile(file);
  mhw_save_raw* save = nullptr;
  LoadFile(file, &save);
  SaveLoader::Load(save, -1);

  // Load the save into the inventory slots
  LoadSaveSlot();

  ui->tabWidget->setEnabled(true);
  ui->actionSave->setEnabled(true);
  ui->menuSwitchWith->setEnabled(true);
  ui->menuCloneTo->setEnabled(true);
  for (int i = 0; i < selectSlotActions.size(); i++) {
    selectSlotActions[i]->setEnabled(true);
  }

  statusFile->setText(QString("File: %1").arg(file));

  char* hunterName = (char*)&mhwSaveSlot->hunter.name;
  QString character = QString::fromUtf8(hunterName);
  Notification* notification = notification->GetInstance();
  notification->ShowMessage(QString("Loaded character slot %1: %2").arg(_mhwSaveIndex + 1).arg(character));

  SaveLoader::FinishLoad();
}

void MHWISaveEditor::LoadSaveSlot()
{
  MHW_SAVE_GUARD;

  for (size_t i = 0; i < selectSlotActions.count(); i++)
  {
    selectSlotActions[i]->setChecked(i == _mhwSaveIndex);
    switchSlotActions[i]->setEnabled(i != _mhwSaveIndex);
    cloneSlotActions[i]->setEnabled(i != _mhwSaveIndex);
  }

  int index = ui->tabWidget->currentIndex();
  SaveLoader* loader = editors[index];
  loader->Load(this->_mhwSave, _mhwSaveIndex);
}

void MHWISaveEditor::EditorTabChange(int editorIndex)
{
  MHW_SAVE_GUARD;

  int index = editorIndex;
  SaveLoader* loader = editors[index];
  loader->Load(this->_mhwSave, _mhwSaveIndex);
}

void MHWISaveEditor::SelectSlot(int slot)
{
  SaveLoader::LoadSlot(slot);
  qInfo("Selected slot index %d.", slot);

  char* hunterName = (char*)&mhwSaveSlot->hunter.name;
  QString character = QString::fromUtf8(hunterName);
  Notification* notification = notification->GetInstance();
  notification->ShowMessage(QString("Loaded character slot %1: %2").arg(_mhwSaveIndex + 1).arg(character));

  LoadSaveSlot();
  SaveLoader::FinishLoad();
}

void MHWISaveEditor::SwitchSlot(int slot)
{
  MHW_SAVE_GUARD;

  mhw_save_slot* temp = (mhw_save_slot*)malloc(sizeof(mhw_save_slot));
  mhw_save_slot* saveA = &mhwSaveIB->section3.saves[_mhwSaveIndex];
  mhw_save_slot* saveB = &mhwSaveIB->section3.saves[slot];
  if (!temp) {
    qWarning("Error allocating memory.");
    return;
  };

  memcpy_s(temp, sizeof(mhw_save_slot), saveA, sizeof(mhw_save_slot));
  memcpy_s(saveA, sizeof(mhw_save_slot), saveB, sizeof(mhw_save_slot));
  memcpy_s(saveB, sizeof(mhw_save_slot), temp, sizeof(mhw_save_slot));
  free(temp);

  Notification* notification = notification->GetInstance();
  notification->ShowMessage(QString("Switched slots: %1 to slot: %2.").arg(_mhwSaveIndex + 1).arg(slot + 1));

  LoadSaveSlot();
}

void MHWISaveEditor::CloneSlot(int slot)
{
  MHW_SAVE_GUARD;

  mhw_save_slot* saveA = &mhwSaveIB->section3.saves[_mhwSaveIndex];
  mhw_save_slot* saveB = &mhwSaveIB->section3.saves[slot];

  memcpy_s(saveB, sizeof(mhw_save_slot), saveA, sizeof(mhw_save_slot));

  Notification* notification = notification->GetInstance();
  notification->ShowMessage(QString("Cloned slot: %1 to slot: %2.").arg(_mhwSaveIndex + 1).arg(slot + 1));
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
