#include "mhwisaveeditor.h"
#include "ui_mhwisaveeditor.h"

#include <QFileDialog>
#include <QSaveFile>
#include <QByteArray>
#include <QDesktopServices>
#include <QGridLayout>
#include <QStyleFactory>

// Encryption
#include "../crypto/iceborne_crypt.h"

// Save paths
#include "../utility/system/file_utils.h"
#include "../utility/mhw_save_utils.h"
#include "../utility/read_bin_file.h"

// Inventory Layout
#include "../types/inventory_areas.h"

// Item Data
#include "../data/ItemDB.h"
#include "../data/BitmapDB.h"

// Notification system
#include "common/Notification.h"

// Other forms
#include "settings/settingsui.h"

// The editors
#include "inventory/inventoryeditor.h"
#include "info/limitedunlocks.h"
#include "info/generalinfo.h"
#include "info/hunterinfo.h"
#include "equipment/equipmenteditortab.h"

MHWISaveEditor::MHWISaveEditor(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::MHWISaveEditor),
  SaveLoader()
{
  settings = settings->GetInstance();
  ui->setupUi(this);
  setWindowIcon(QIcon(Paths::GetResourcesPath("icon.ico")));

  Notification* notif = notif->GetInstance();
  notif->Register(ui->statusbar);
  notif->SetDefaultMode(NotificationMode::StatusBar);

  statusFile = new QLabel(tr("File: None", "Indicate the currently open file (no file)."), ui->statusbar);
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
  openSignalMapper->setMapping(ui->actionOpenGameLocation, Paths::GetGamePath());
  connect(ui->actionOpenSaveLocation, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenSaveLocation, Paths::GetDefaultSavePath());
  connect(ui->actionOpenEditorData, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenEditorData, Paths::GetDataPath());
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
    {new GeneralInfo(),        &generalInfo    },
    {new HunterInfo(),         &hunterInfo     },
    {new InventoryEditor(),    &inventoryEditor},
    {new LimitedUnlocks(),     &limitedUnlocks },
    {new EquipmentEditorTab(), &equipmentEditor},
  };
  int editorCount = COUNTOF(editorTabs);
  editors.resize(editorCount);

  for (int i = 0; i < editorCount; i++)
  {
    editor_tab* tab = &editorTabs[i];
    SaveLoader* loader = (SaveLoader*)tab->widget;
    QWidget* editor = dynamic_cast<QWidget*>(loader);

    ui->tabWidget->addTab(editor, editor->windowTitle());
    if (tab->binding)
      *tab->binding = loader;
    editors[i] = loader;
  }

  filters.append(tr("All Files", "Open/Save filter, show all files.") + " (*)");
  filters.append(tr("Encrypted Save", "Open/Save filter, show only *.raw files.") + " (*.raw)");
  filters.append(tr("Unencrypted Save", "Open/Save filter, show only *.bin files.") + " (*.bin)");

  ext_map.insert(filters[0], "");
  ext_map.insert(filters[1], ".raw");
  ext_map.insert(filters[2], ".bin");

  encrypt_map.insert("", true);
  encrypt_map.insert(".raw", true);
  encrypt_map.insert(".bin", false);

  SetupDarkMode();
}

MHWISaveEditor::~MHWISaveEditor()
{
  delete ui;

  SaveLoader::Unload(true);
}

void MHWISaveEditor::closeEvent(QCloseEvent* event)
{
  settings->Free();
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
  if (success) {
    notif->ShowMessage(tr("Saved file: %1", "Indicate sucessful file save, %1 is the path where the file was saved to.").arg(path));
    statusFile->setText(tr("File: %1", "Indicate the currently open file.").arg(EditorFile()));
  }
  else
    notif->ShowMessage(tr("Could not save file: %1", "Indicate failed file save, %1 is the path where the file was supposed to be saved to.").arg(path));
}

bool MHWISaveEditor::LoadFile(const QString& path, mhw_save_raw** save)
{
  qInfo("Loading %s", qUtf8Printable(path));
  mhw_save_raw* savep = *save;

  savep = (mhw_save_raw*)ReadEntireFile(path, (u8*)savep, sizeof(mhw_save_raw));
  if (!savep) {
    qWarning("Save: %s, cannot be read.", qUtf8Printable(path));
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
  QString path = Paths::GetDefaultDumpPath(number);
  bool success = true;

  if (MHW_SAVE_GUARD_CHECK) {
    buffer = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));

    if (!buffer) {
      success = false;
    }
    else {
      LoadFile(Paths::GetDefaultSavePath(), &buffer);
      saveWrite = buffer;
    }
  }

  if (success) {
    success = SaveFileEncrypt(path, saveWrite, false);
  }

  Notification* notif = notif->GetInstance();
  if (success)
    notif->ShowMessage(tr("Dumped file: %1", "Indicate a successful file dump, %1 is the path where the file was dumped to.").arg(path));
  else
    notif->ShowMessage(tr("Could not dump file: %1", "Indicate a failed file dump, %1 is the path where the file was supposed to be dumped to.").arg(path));

  if (buffer) free(buffer);
}

void MHWISaveEditor::Open()
{
  QString path = Paths::GetDefaultSaveDir();
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
  QString path = Paths::GetDefaultSavePath();
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

  QString path = Paths::GetDefaultSaveDir();

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

    SaveLoader::LoadFile(files[0]);
    SaveFile(files[0]);
  }
}

void MHWISaveEditor::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  int mhwSaveIndex = MHW_SaveIndex();

  game_language language = settings->GetItemLanguage();
  LoadItemLanguage(language);

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
  notif->ShowMessage(tr("Loaded character slot %1: %2", "Indicate a character slot change, %1 is the slot number, %2 is the character name.").arg(mhwSaveIndex + 1).arg(character));

  statusFile->setText(tr("File: %1", "Indicate the currently open file.").arg(EditorFile()));

  SaveLoader::FinishLoad();
}

void MHWISaveEditor::LoadFile(const QString& file)
{
  bool load = LoadFile(file, MHWS_SavePtr());

  if (load) {
    SaveLoader::LoadFile(file);
    Load(MHW_Save());

    if (settings->GetDoAutoBackups()) {
      Notification* notif = notif->GetInstance();
      notif->Silence(1);
      Backup();
    }
  }
  else {
    Notification* notif = notif->GetInstance();
    notif->ShowMessage(tr("Failed to load file: %1", "Indicate a failed file load, %1 is the path to the file that was to be loaded.").arg(file));
  }
}

void MHWISaveEditor::LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB)
{
  SaveLoader::LoadResources(itemDB, bitmapDB);
  inventoryEditor->LoadResources(itemDB, bitmapDB);
  equipmentEditor->LoadResources(itemDB, bitmapDB);
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

bool MHWISaveEditor::WriteFile(const QString& path, u8* data, u64 size)
{
  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning("File: %s, cannot be written.", qUtf8Printable(path));
    return false;
  }

  u64 bytesToWrite = size;
  u64 bytesWritten = 0;
  char* writeData = (char*)data;
  while (bytesWritten < bytesToWrite)
  {
    u64 writeSize = bytesToWrite - bytesWritten;
    i64 bytesWrite = file.write(writeData, writeSize);
    if (bytesWrite < 0) {
      qWarning("File: %s, cannot be written.", qUtf8Printable(path));
      file.cancelWriting();
      return false;
    }
    bytesWritten += bytesWrite;
    writeData += bytesWrite;
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
  notif->ShowMessage(tr("Loaded character slot %1: %2", "Indicate a character slot change, %1 is the slot number, %2 is the character name.").arg(mhwSaveIndex + 1).arg(character));

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
  notif->ShowMessage(tr("Switched slots: %1 to slot: %2.", "Indicate a successful slot switch, %1 is the current slot, %2 is the target slot.").arg(mhwSaveIndex + 1).arg(slot + 1));

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
  notif->ShowMessage(tr("Cloned slot: %1 to slot: %2.", "Indicate a slot was cloned, %1 is the current slot, %2 is the target slot.").arg(mhwSaveIndex + 1).arg(slot + 1));
}

void MHWISaveEditor::OpenLocation(const QString& location)
{
  qInfo("Opening: %s", qUtf8Printable(location));
  FileUtils::showInGraphicalShell(location);
}

void MHWISaveEditor::OpenSettings()
{
  SettingsUI* settingsUI = new SettingsUI();
  settingsUI->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
  settingsUI->exec();

  if (settings->GetRequireRestart()) {
    Notification* notif = notif->GetInstance();
    NotificationMode notifMode = notif->GetDefaultMode();
    notif->SetDefaultMode(NotificationMode::MessageBox);
    notif->ShowMessage(tr("Some settings you have changed will apply on restart.", "Tell the user they have some settings that will only be applied on restart."));
    notif->SetDefaultMode(notifMode);
  }

  game_language language = settings->GetItemLanguage();
  LoadItemLanguage(language, true);
}

void MHWISaveEditor::LoadItemLanguage(game_language language, bool doReload) {
  game_language current = itemDB->CurrentLanguage();

  if (language == game_language::GameLanguage && MHW_SAVE_CHECK)
    language = (game_language)MHW_Section1()->text_language;

  if (language != current) {
    itemDB->LoadGMD(language);
    inventoryEditor->LoadResources(itemDB, bitmapDB);
    LoadSaveSlot();
  }
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
      success = LoadFile(Paths::GetDefaultSavePath(), &buffer);
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
  QString writeFile = basename + ext + '_' + datatime + ".zlib";
  QString path = Paths::GetDataPathBackups() + writeFile;

  if (success) {
    QByteArray compressed = qCompress((u8*)saveWrite, sizeof(mhw_save_raw), 9);
    int compressedSize = compressed.size();

    success = WriteFile(path, (u8*)compressed.constData(), compressedSize);
  }

  Notification* notif = notif->GetInstance();
  if (success) {
    notif->ShowMessage(tr("Backup made: %1", "Notify of a backup being made, %1 is the path to where it is.").arg(writeFile));
  }
  else {
    notif->ShowMessage(tr("Failed to create backup: %1", "Notify of a backup failure, %1 is the path to where it was supposed to be.").arg(writeFile));
  }
  TrimBackups();
}

void MHWISaveEditor::Restore() {
  QString path = Paths::GetDataPathBackups();
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
  SaveLoader::LoadFile(Paths::GetDefaultSavePath());
  Load(savep, -1);
}

void MHWISaveEditor::TrimBackups()
{
  int maxBackups = settings->GetMaxBackups();
  if (!maxBackups) return;
  QString path = Paths::GetDataPathBackups();
  QDir dir(path);

  QStringList nameFilters;
  nameFilters << "*.zlib";
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
      qDebug().noquote() << QString("Delete [%1/%2]: %3")
        .arg(deleteTotal - deleteCount + 1).arg(deleteTotal).arg(fileName);
      file.remove();
      deleteCount--;
    }
    else break;
  }
}

void MHWISaveEditor::SetupDarkMode()
{
  bool darkMode = settings->GetDarkMode();
  if (darkMode) {
    QPalette p;
    qApp->setStyle("Fusion");
    p.setColor(QPalette::Window, QColor(53, 53, 53));
    p.setColor(QPalette::WindowText, Qt::white);
    p.setColor(QPalette::Base, QColor(25, 25, 25));
    p.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    p.setColor(QPalette::ToolTipBase, Qt::black);
    p.setColor(QPalette::ToolTipText, Qt::white);
    p.setColor(QPalette::Text, Qt::white);
    p.setColor(QPalette::Button, QColor(53, 53, 53));
    p.setColor(QPalette::ButtonText, Qt::white);
    p.setColor(QPalette::BrightText, Qt::red);
    p.setColor(QPalette::Link, QColor(42, 130, 218));
    p.setColor(QPalette::Highlight, QColor(42, 130, 218));
    p.setColor(QPalette::HighlightedText, Qt::black);
    p.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, QColor(23, 23, 23));
    p.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, Qt::gray);
    p.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, Qt::gray);
    p.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, Qt::gray);
    qApp->setPalette(p);
  }
}
