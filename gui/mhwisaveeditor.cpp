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

  Notification* notif = notif->GetInstance();
  notif->Register(ui->statusbar);
  notif->SetDefaultMode(NotificationMode::StatusBar);

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
  openSignalMapper->setMapping(ui->actionOpenSaveLocation, GetDefaultSavePath());
  connect(ui->actionOpenEditorData, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenEditorData, GetDataPath());
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

  filters.append(tr(ALL_SAVE));
  filters.append(tr(ENCRYPTED_SAVE));
  filters.append(tr(UNENCRYPTED_SAVE));

  ext_map.insert(tr(ALL_SAVE), "");
  ext_map.insert(tr(ENCRYPTED_SAVE), ".raw");
  ext_map.insert(tr(UNENCRYPTED_SAVE), ".bin");

  encrypt_map.insert("", true);
  encrypt_map.insert(".raw", true);
  encrypt_map.insert(".bin", false);

  ReadSettings();
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

  WriteSettings();
}

bool MHWISaveEditor::SaveFileEncrypt(const QString& path, mhw_save_raw* save, bool encrypt, bool validate)
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

  bool written = WriteFile(path, saveWrite->data, sizeof(mhw_save_raw));

  if (saveWrite != save) free(saveWrite);
  return written;
}

void MHWISaveEditor::SaveFile(const QString& path)
{
  QFileInfo fi(path);
  QString ext = fi.completeSuffix();
  ext = ext.isEmpty() ? "" : '.' + ext;
  QString filepath = fi.absoluteFilePath();

  bool encrypt = encrypt_map.value(ext, false);
  bool success = SaveFileEncrypt(filepath, MHW_Save(), encrypt, true);

  Notification* notif = notif->GetInstance();
  if (success)
    notif->ShowMessage("Saved file: " + path);
  else
    notif->ShowMessage("Could not save file: " + path);
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
  file.close();

  mhw_save_raw* newsavep = (mhw_save_raw*)QByteArrayToU8(saveBlob, (u8*)savep, sizeof(mhw_save_raw));
  if (savep != newsavep && savep) free(savep);
  savep = newsavep;

  if (!savep) {
    qWarning("File: %s, cannot be read.", qUtf8Printable(path));
    return false;
  }

  if (!IsBlowfishDecrypted(&savep->save)) {
    DecryptSave(savep->data, sizeof(mhw_save_raw));
  }
  *save = savep;
  return true;
}

void MHWISaveEditor::Dump(int number)
{
  mhw_save_raw* saveWrite = MHWS_Save();
  mhw_save_raw* buffer = nullptr;
  QString path = GetDefaultDumpPath(number);
  bool success = true;

  if (MHW_SAVE_GUARD_CHECK) {
    buffer = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));

    if (!buffer) {
      success = false;
    }
    else {
      LoadFile(GetDefaultSavePath(), &buffer);
      saveWrite = buffer;
    }
  }

  if (success) {
    success = SaveFileEncrypt(path, saveWrite, false);
  }

  Notification* notif = notif->GetInstance();
  if (success)
    notif->ShowMessage("Dumped file: " + path);
  else
    notif->ShowMessage("Could not dump file: " + path);

  if (buffer) free(buffer);
}

void MHWISaveEditor::Open()
{
  QString path = GetDefaultSaveDir();
  QString filepath = QString();

  QFileDialog dialog(nullptr);
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

  QFileInfo fi(EditorFile());
  QString ext = fi.completeSuffix();
  ext = ext.isEmpty() ? "" : '.' + ext;

  QString filepath = fi.absoluteFilePath();
  bool encrypt = encrypt_map.value(ext, false);
  SaveFile(filepath);
}

void MHWISaveEditor::SaveAs()
{
  MHW_SAVE_GUARD;

  QString path = GetDefaultSaveDir();

  QFileDialog dialog(this);
  dialog.setDirectory(path);
  dialog.setNameFilters(filters);
  dialog.setAcceptMode(QFileDialog::AcceptSave);

  if (dialog.exec()) {
    QString selectedFilter = dialog.selectedNameFilter();
    QStringList files = dialog.selectedFiles();

    QFileInfo fi(path);
    QString ext = fi.completeSuffix();
    if (ext.isEmpty()) {
      fi.setFile(fi.filePath() + ext_map.value(selectedFilter));
    }

    SaveFile(files[0]);
  }
}

void MHWISaveEditor::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  int mhwSaveIndex = MHW_SaveIndex();

  // Load the save into the inventory slots
  LoadSaveSlot();

  ui->tabWidget->setEnabled(true);
  ui->actionSave->setEnabled(true);
  ui->actionSaveAs->setEnabled(true);
  ui->menuSwitchWith->setEnabled(true);
  ui->menuCloneTo->setEnabled(true);
  for (int i = 0; i < selectSlotActions.size(); i++) {
    selectSlotActions[i]->setEnabled(true);
  }

  char* hunterName = (char*)&mhwSaveSlot->hunter.name;
  QString character = QString::fromUtf8(hunterName);
  Notification* notif = notif->GetInstance();
  notif->ShowMessage(QString("Loaded character slot %1: %2").arg(mhwSaveIndex + 1).arg(character));

  statusFile->setText("File: " + EditorFile());

  SaveLoader::FinishLoad();
}

void MHWISaveEditor::LoadFile(const QString& file)
{
  SaveLoader::LoadFile(file);
  bool load = LoadFile(file, MHWS_SavePtr());

  if (load) {
    Load(MHW_Save());

    if (doAutoBackups) {
      Notification* notif = notif->GetInstance();
      notif->Silence(1);
      Backup();
    }
  }
  else {
    Notification* notif = notif->GetInstance();
    notif->ShowMessage("Failed to load file: " + file);
  }
}

void MHWISaveEditor::LoadSaveSlot()
{
  MHW_SAVE_GUARD;
  mhw_save_raw* mhwSave = MHW_Save();
  int mhwSaveIndex = MHW_SaveIndex();

  for (size_t i = 0; i < selectSlotActions.count(); i++)
  {
    selectSlotActions[i]->setChecked(i == mhwSaveIndex);
    switchSlotActions[i]->setEnabled(i != mhwSaveIndex);
    cloneSlotActions[i]->setEnabled(i != mhwSaveIndex);
  }

  int index = ui->tabWidget->currentIndex();
  SaveLoader* loader = editors[index];
  loader->Load(mhwSave, mhwSaveIndex);
}

bool MHWISaveEditor::WriteFile(const QString& path, u8* data, u32 size)
{
  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning("File: %s, cannot be written.", qUtf8Printable(path));
    return false;
  }

  int length = file.write((char*)data, size);
  if (length != size) {
    qWarning("File: %s, cannot be written.", qUtf8Printable(path));
    return false;
  }
  file.commit();
  return true;
}

void MHWISaveEditor::EditorTabChange(int editorIndex)
{
  MHW_SAVE_GUARD;
  mhw_save_raw* mhwSave = MHW_Save();
  int mhwSaveIndex = MHW_SaveIndex();

  int index = editorIndex;
  SaveLoader* loader = editors[index];
  loader->Load(mhwSave, mhwSaveIndex);
}

void MHWISaveEditor::SelectSlot(int slot)
{
  SaveLoader::LoadSlot(slot);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  int mhwSaveIndex = MHW_SaveIndex();

  qInfo("Selected slot index %d.", slot);

  char* hunterName = (char*)&mhwSaveSlot->hunter.name;
  QString character = QString::fromUtf8(hunterName);
  Notification* notif = notif->GetInstance();
  notif->ShowMessage(QString("Loaded character slot %1: %2").arg(mhwSaveIndex + 1).arg(character));

  LoadSaveSlot();
  SaveLoader::FinishLoad();
}

void MHWISaveEditor::SwitchSlot(int slot)
{
  MHW_SAVE_GUARD;
  mhw_section3* mhwSection3 = MHW_Section3();
  int mhwSaveIndex = MHW_SaveIndex();

  mhw_save_slot* temp = (mhw_save_slot*)malloc(sizeof(mhw_save_slot));
  mhw_save_slot* saveA = &mhwSection3->saves[mhwSaveIndex];
  mhw_save_slot* saveB = &mhwSection3->saves[slot];
  if (!temp) {
    qWarning("Error allocating memory.");
    return;
  };

  memcpy_s(temp, sizeof(mhw_save_slot), saveA, sizeof(mhw_save_slot));
  memcpy_s(saveA, sizeof(mhw_save_slot), saveB, sizeof(mhw_save_slot));
  memcpy_s(saveB, sizeof(mhw_save_slot), temp, sizeof(mhw_save_slot));
  free(temp);

  Notification* notif = notif->GetInstance();
  notif->ShowMessage(QString("Switched slots: %1 to slot: %2.").arg(mhwSaveIndex + 1).arg(slot + 1));

  LoadSaveSlot();
}

void MHWISaveEditor::CloneSlot(int slot)
{
  MHW_SAVE_GUARD;
  mhw_section3* mhwSection3 = MHW_Section3();
  int mhwSaveIndex = MHW_SaveIndex();

  mhw_save_slot* saveA = &mhwSection3->saves[mhwSaveIndex];
  mhw_save_slot* saveB = &mhwSection3->saves[slot];

  memcpy_s(saveB, sizeof(mhw_save_slot), saveA, sizeof(mhw_save_slot));

  Notification* notif = notif->GetInstance();
  notif->ShowMessage(QString("Cloned slot: %1 to slot: %2.").arg(mhwSaveIndex + 1).arg(slot + 1));
}

void MHWISaveEditor::OpenLocation(const QString& location)
{
  qInfo("Opening: %s", qUtf8Printable(location));
  FileUtils::showInGraphicalShell(location);
}

void MHWISaveEditor::Backup() {
  mhw_save_raw* saveWrite = MHWS_Save();
  mhw_save_raw* buffer = nullptr;
  bool success = true;

  if (MHW_SAVE_GUARD_CHECK) {
    buffer = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));

    if (!buffer) {
      success = false;
    }
    else {
      success = LoadFile(GetDefaultSavePath(), &buffer);
      saveWrite = buffer;
    }
  }

  QFileInfo fi = QFileInfo(EditorFile());
  QString basename = fi.baseName();
  QString ext = fi.completeSuffix();

  if (basename.isEmpty()) basename = QString::fromUtf8(SAVE_NAME);
  if (ext.isEmpty()) ext = "";
  else ext = "_" + ext.replace('.', '_');

  QDateTime date = QDateTime::currentDateTime();
  QString datatime = date.toString("yyyy-MM-dd_hh-mm-ss");
  QString writeFile = basename + ext + '_' + datatime + ".bak";
  QString path = GetDataPathBackups() + writeFile;

  if (success) {
    QByteArray compressed = qCompress((u8*)saveWrite, sizeof(mhw_save_raw), 9);
    int compressedSize = compressed.size();

    success = WriteFile(path, (u8*)compressed.constData(), compressedSize);
  }

  Notification* notif = notif->GetInstance();
  if (success) {
    notif->ShowMessage("Backup made: " + writeFile);
  }
  else {
    notif->ShowMessage("Failed to create backup: " + writeFile);
  }
  TrimBackups();
}

void MHWISaveEditor::Restore() {
  QString path = GetDataPathBackups();
  QFileDialog dialog(this);
  dialog.setDirectory(path);
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (!dialog.exec()) return;
  QString filepath = dialog.selectedFiles()[0];

  QFile file(filepath, this);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning("File: %s, cannot be read.", qUtf8Printable(path));
    return;
  }

  QByteArray compressed = file.readAll();
  QByteArray saveBlob = qUncompress(compressed);
  file.close();

  mhw_save_raw* savep = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
  if (!savep) {
    qWarning("Error allocating memory.");
    return;
  };

  memcpy(savep->data, saveBlob.constData(), saveBlob.length());
  SaveLoader::LoadFile(GetDefaultSavePath());
  Load(savep, -1);
}

void MHWISaveEditor::TrimBackups()
{
  if (!maxBackups) return;
  QString path = GetDataPathBackups();
  QDir dir(path);

  QStringList nameFilters;
  nameFilters << "*.bak";
  dir.setNameFilters(nameFilters);
  QDir::Filters filters = QDir::Filter::Files;
  QDir::SortFlags sort = QDir::SortFlag::Time | QDir::SortFlag::Reversed;

  QFileInfoList files = dir.entryInfoList(filters, sort);
  int deleteTotal = files.size() - maxBackups;
  int deleteCount = deleteTotal;
  for (int i = 0; i < files.size(); i++) {
    QFileInfo fileInfo = files.at(i);
    QFile file = fileInfo.absoluteFilePath();
    
    if (deleteCount > 0) {
      QString fileName = fileInfo.fileName();
      qDebug() << QString("Delete [%1/%2]: %3").arg(deleteTotal - deleteCount + 1).arg(deleteTotal).arg(fileName);
      file.remove();
      deleteCount--;
    }
    else break;
  }
}

void MHWISaveEditor::ReadSettings()
{
  QString path = GetDataPath() + "/settings.ini";
  QSettings::Format format = QSettings::Format::IniFormat;
  QSettings::setDefaultFormat(format);
  QSettings settings(path, format, this);

  settings.beginGroup("backups");
  doAutoBackups = settings.value("doAutoBackups", doAutoBackups).toBool();
  maxBackups = settings.value("maxBackups", maxBackups).toInt();
  settings.endGroup();

  qDebug() << "Read settings file: " + settings.fileName();
}

void MHWISaveEditor::WriteSettings()
{
  QString path = GetDataPath() + "/settings.ini";
  QSettings::Format format = QSettings::Format::IniFormat;
  QSettings::setDefaultFormat(format);
  QSettings settings(path, format, this);

  settings.beginGroup("backups");
  settings.setValue("doAutoBackups", doAutoBackups);
  settings.setValue("maxBackups", maxBackups);
  settings.endGroup();

  qDebug() << "Wrote settings file: " + settings.fileName();
}
