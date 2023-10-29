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
#include "../utility/read_bin_file.h"

// Save functions
#include "../utility/mhw_save_operations.h"

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

#include "../utility/chunk/chunk.h"

MHWISaveEditor::MHWISaveEditor(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::MHWISaveEditor),
  SaveLoader()
{
  settings = settings->GetInstance();
  ui->setupUi(this);
  QIcon windowIcon = QIcon(Paths::GetResourcesPath("icon.ico"));
  setWindowIcon(windowIcon);

  msgNotification = new QMessageBox(
    QMessageBox::Icon::Information,
    tr("Notification", "Notification popup title. Usually appears when there something the user should be aware of."),
    NULL, QMessageBox::StandardButton::Ok, this);
  msgNotification->setWindowIcon(windowIcon);

  Notification* notif = notif->GetInstance();
  notif->Register(msgNotification);
  notif->Register(ui->statusbar);
  notif->SetDefaultMode(NotificationMode::NotifModeStatusBar);

  scrollGuard = scrollGuard->GetInstance();

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

  const QString gamePath = Paths::GetGamePath();
  const QString gameSaveFilePath = Paths::GetGameSaveFilePath();
  const QString dataPath = Paths::GetDataPath();
  ui->actionOpenGameLocation->setEnabled(!gamePath.isNull());
  ui->actionOpenSaveLocation->setEnabled(!gameSaveFilePath.isNull());
  ui->actionOpenSAVEDATA1000->setEnabled(!gameSaveFilePath.isNull());
  ui->actionOpenEditorData->setEnabled(!dataPath.isNull());
  qInfo().noquote() << "Game path: " << gamePath;
  qInfo().noquote() << "Game save file path: " << gameSaveFilePath;
  qInfo().noquote() << "Data path: " << dataPath;

  openSignalMapper = new QSignalMapper(this);
  connect(ui->actionOpenGameLocation, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenGameLocation, gamePath);
  connect(ui->actionOpenSaveLocation, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenSaveLocation, gameSaveFilePath);
  connect(ui->actionOpenEditorData, SIGNAL(triggered()), openSignalMapper, SLOT(map()));
  openSignalMapper->setMapping(ui->actionOpenEditorData, dataPath);
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

#ifdef QT_DEBUG
  {
    Chunk* chunk = new Chunk();
    QList<FileNode*> analysis;
    analysis = chunk->AnalyseChunk("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Monster Hunter World\\chunk\\chunkG0.bin", analysis);
    chunk->ExtractSelected(analysis, "A:/chunk");
    FileNode::DeleteList(analysis);
    delete chunk;
  }
#endif
}

MHWISaveEditor::~MHWISaveEditor()
{
  delete ui;

  SaveLoader::Unload(true);
}

void MHWISaveEditor::closeEvent(QCloseEvent* event)
{
  settings->Free();
  scrollGuard->Free();
}

bool MHWISaveEditor::SaveFileEncrypt(const QString& path, mhw_save_raw* save, bool encrypt, bool validate)
{
  qInfo("Saving: %s", qUtf8Printable(path));
  qInfo() << "Validate:" << validate;
  qInfo() << "Encrypted:" << encrypt;

  mhw_save_raw* saveWrite = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
  if (!saveWrite) {
    qWarning("Error allocating memory.");
    return false;
  }

  memcpy(saveWrite, save, sizeof(mhw_save_raw));
  if (validate) MHWSaveUtils::ValidateSaveFile(&saveWrite->save);
  if (encrypt) EncryptSave(saveWrite->data, sizeof(mhw_save_raw));

  bool written = FileUtils::WriteFileSafe(path, saveWrite->data, sizeof(mhw_save_raw));

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

  if (!MHWSaveUtils::IsBlowfishDecrypted(&savep->save)) {
    DecryptSave(savep->data, sizeof(mhw_save_raw));
  }
  *save = savep;
  return true;
}

void MHWISaveEditor::Dump(int number)
{
  mhw_save_raw* saveWrite = MHWS_Save();
  mhw_save_raw* buffer = nullptr;
  QString path = Paths::GetSaveDumpPath(EditorFile(), number);
  bool success = true;

  if (MHW_SAVE_GUARD_CHECK) {
    buffer = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));

    if (!buffer) {
      success = false;
    }
    else {
      success = LoadFile(Paths::GetGameSaveFilePath(), &buffer);
      saveWrite = buffer;
    }
  }

  if (success) {
    success = SaveFileEncrypt(path, saveWrite, false);
  }

  Notification* notif = notif->GetInstance();
  if (success) {
    qInfo().noquote() << "Succesfully dumped file:" << path;
    notif->ShowMessage(tr("Dumped file: %1", "Indicate a successful file dump, %1 is the path where the file was dumped to.").arg(path));
  }
  else {
    notif->ShowMessage(tr("Could not dump file: %1", "Indicate a failed file dump, %1 is the path where the file was supposed to be dumped to.").arg(path));
    qInfo().noquote() << "Failed to dump file:" << path;
  }

  if (buffer) free(buffer);
}

void MHWISaveEditor::Open()
{
  QString path = Paths::GetGameSavePath();
  QString filepath = QString();

  QFileDialog dialog(nullptr);
  dialog.setDirectory(path);
  dialog.selectFile(QString::fromUtf8(MHW_SAVE_NAME));
  dialog.setFileMode(QFileDialog::ExistingFile);

  if (dialog.exec()) {
    QStringList files = dialog.selectedFiles();
    filepath = files[0];

    LoadFile(filepath);

    QString editorFile = EditorFile();
    QDir fileInfo(editorFile);

    if (fileInfo.exists(MHW_EXE_REL_BACKUP.c_str())) {
      QString theoryPath = Paths::GetTheoreticalGameSaveFilePath();
      qWarning() << "Opened game backup save:" << editorFile;

      Notification* notif = notif->GetInstance();
      notif->PushMode(NotificationMode::NotifModeMessageBox);
      notif->ShowMessage(tr("You have likely opened a backup of your save file.\nChanges won't be seen in-game.\nYour save file is usually in the following place:\n%1\n\nYour Steam Account ID can be found online.",
        "Tell the user that they've loaded a backup, and instruct them on where to find their save. %s is the path to where the save is usually found.")
        .arg(theoryPath));
      notif->PopMode();
    }
  }
}

void MHWISaveEditor::OpenSAVEDATA1000()
{
  QString path = Paths::GetGameSaveFilePath();
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
  QString path = Paths::GetGameSavePath();

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
  ui->actionUncraftEquipment->setEnabled(true);
  ui->menuFixes->setEnabled(true);
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
      notif->PushMode(NotificationMode::NotifModeNone);
      Backup();
      notif->PopMode();
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

  SaveLoader* loader = GetActiveEditorTab();
  loader->Load(mhwSave, mhwSaveIndex);
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
  notif->ShowMessage(tr("Switched slots: %1 and %2", "Indicate a successful slot switch, %1 is the current slot, %2 is the target slot.").arg(mhwSaveIndex + 1).arg(slot + 1));
  qInfo().nospace() << "Switched slots: " << mhwSaveIndex << " and " << slot;

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
  notif->ShowMessage(tr("Cloned slot: %1 to %2",
    "Indicate a slot was cloned, %1 is the current slot, %2 is the target slot.")
    .arg(mhwSaveIndex + 1).arg(slot + 1));
  qInfo().nospace() << "Cloned slot: " << mhwSaveIndex << " to " << slot;
}

void MHWISaveEditor::UncraftUnusedEquipment()
{
  MHW_SAVE_GUARD;
  mhw_save_raw* mhwSave = MHW_Save();
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  int mhwSaveIndex = MHW_SaveIndex();

  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  SmithyDB* smithyDB = smithyDB->GetInstance();

  qInfo("Uncrafting all unused, non-permanent equipment...");
  MHWSaveOperations::UncraftAllUnusedEquipment(mhwSaveSlot, false, equipmentDB, smithyDB, itemDB);
  qInfo("Unused equipment has been uncrafted.");

  SaveLoader* loader = GetActiveEditorTab();
  if (loader == equipmentEditor) loader->Load(mhwSave, mhwSaveIndex);
  else if (loader == inventoryEditor) loader->Load(mhwSave, mhwSaveIndex);

  Notification* notif = notif->GetInstance();
  notif->ShowMessage(tr("Unused equipment uncrafted, permanent equipment items are skipped.",
    "Tell the user the unused equipment has been uncrafted, and that permanent equipment (such as Defender Gear) is skipped."));
}

void MHWISaveEditor::OpenLocation(const QString& location)
{
  qInfo("Opening: %s", qUtf8Printable(location));
  FileUtils::showInGraphicalShell(location);
}

void MHWISaveEditor::OpenSettings()
{
  bool oldShowUnobtainable = settings->GetShowUnobtainable();
  SettingsUI* settingsUI = new SettingsUI();
  settingsUI->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
  settingsUI->exec();

  if (settings->GetRequireRestart()) {
    Notification* notif = notif->GetInstance();
    NotificationMode notifMode = notif->GetDefaultMode();
    notif->PushMode(NotificationMode::NotifModeMessageBox);
    notif->ShowMessage(tr("Some settings you have changed will apply on restart.", "Tell the user they have some settings that will only be applied on restart."));
    notif->PopMode();
  }

  game_language language = settings->GetItemLanguage();
  LoadItemLanguage(language, oldShowUnobtainable != settings->GetShowUnobtainable());
}

void MHWISaveEditor::LoadItemLanguage(game_language language, bool reloadItemSearch) {
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  game_language currentItem = itemDB->CurrentLanguage();
  game_language currentEquipment = equipmentDB->CurrentLanguage();

  if (language == game_language::GameLanguage && MHW_SAVE_CHECK)
    language = (game_language)MHW_Section1()->text_language;

  bool reload = false;
  if (language != currentItem) { itemDB->LoadGMD(language); reload = true; }
  if (language != currentEquipment) { equipmentDB->LoadGMD(language); reload = true; }

  if (reloadItemSearch || reload) inventoryEditor->LoadResources(itemDB, bitmapDB);
  if (reload) LoadSaveSlot();
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
      success = LoadFile(Paths::GetGameSaveFilePath(), &buffer);
      saveWrite = buffer;
    }
  }

  QString path = Paths::GetBackupPath(EditorFile());
  if (success) {
    QByteArray compressed = qCompress((u8*)saveWrite, sizeof(mhw_save_raw), 9);
    int compressedSize = compressed.size();

    success = FileUtils::WriteFileSafe(path, (u8*)compressed.constData(), compressedSize);
  }

  Notification* notif = notif->GetInstance();
  if (success) {
    notif->ShowMessage(tr("Backup made: %1", "Notify of a backup being made, %1 is the path to where it is.").arg(path));
  }
  else {
    notif->ShowMessage(tr("Failed to create backup: %1", "Notify of a backup failure, %1 is the path to where it was supposed to be.").arg(path));
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
  SaveLoader::LoadFile(Paths::GetGameSaveFilePath());
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

SaveLoader* MHWISaveEditor::GetActiveEditorTab()
{
  int index = ui->tabWidget->currentIndex();
  return editors[index];
}

void MHWISaveEditor::DebugDumpIconsAll()
{
  QString dumpPath = Paths::GetIconDumpPath();

  bool success = true;
  success &= bitmapDB->DebugDumpAtlas(dumpPath);
  success &= bitmapDB->DebugDumpIcons(dumpPath);
  success &= bitmapDB->DebugDumpUsedMask(dumpPath);

  Notification* notif = notif->GetInstance();
  if (success) {
    notif->ShowMessage(tr("Dumped icons: %1",
      "Indicate sucessful icon dump, %1 is the path where the icons were dumped to.").arg(dumpPath));
    qInfo().noquote() << "Dumped all icons: " << dumpPath;
  }
  else {
    notif->ShowMessage(tr("Could not dump icons: %1",
      "Indicate failed icon dump, %1 is the path where the icons were supposed to be dumped to.").arg(dumpPath));
    qWarning().noquote() << "Failed to dump all icons: " << dumpPath;
  }
}

void MHWISaveEditor::DebugDefragEquipment()
{
  MHW_SAVE_GUARD;
  mhw_save_raw* mhwSave = MHW_Save();
  int mhwSaveIndex = MHW_SaveIndex();
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  qInfo("Checking equipment box references...");
  bool check = MHWSaveUtils::ValidateEquipmentBox(mhwSaveSlot, true);

  Notification* notif = notif->GetInstance();
  if (check) {
    qInfo("Defragging equipment references...");
    MHWSaveUtils::DefragEquipmentReferences(mhwSaveSlot);
    qInfo("Defragging equipment...");
    int defragged = MHWSaveUtils::DefragEquipment(mhwSaveSlot);
    qInfo("Defragging finished successfully.");
    qInfo().nospace() << defragged << " equipment items were moved.";

    SaveLoader* loader = GetActiveEditorTab();
    if (loader == equipmentEditor) loader->Load(mhwSave, mhwSaveIndex);

    notif->ShowMessage(tr("Equipment has been defragged.", "Tell the user that the equipment has been defragged."));
  }
  else {
    qCritical("Defragging equipment failed, equipment box references are corrupt.");

    notif->PushMode(NotificationMode::NotifModeMessageBox);
    notif->ShowMessage(tr("Could not defrag equipment, there are issues with the equipment references.\n\n"
      "Try running the Equipment Box Reference Fix:\n"
      "Debug -> Fixes -> Equipment Box Reference Fix",
      "Tell the user that equipment couldn't be defragged, because there are issues."));
    notif->PopMode();
  }
}

void MHWISaveEditor::DebugFixEquipmentBoxRef()
{
  MHW_SAVE_GUARD;
  mhw_save_raw* mhwSave = MHW_Save();
  int mhwSaveIndex = MHW_SaveIndex();
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  qInfo("Checking equipment box references...");
  bool check = MHWSaveUtils::ValidateEquipmentBox(mhwSaveSlot, true);

  Notification* notif = notif->GetInstance();
  if (!check) {
    qInfo("Fixing equipment box references...");
    check = MHWSaveUtils::ValidateEquipmentBox(mhwSaveSlot, false);

    if (check) {
      SaveLoader* loader = GetActiveEditorTab();
      if (loader == equipmentEditor) loader->Load(mhwSave, mhwSaveIndex);

      qInfo("Equipment box references have been fixed successfully.");

      notif->ShowMessage(tr("Equipment box references have been fixed successfully.",
        "Tell the user the equipment box references were fixed."));
    }
    else {
      qCritical("Fixing equipment box references failed.");

      notif->PushMode(NotificationMode::NotifModeMessageBox);
      notif->ShowMessage(tr("Could not fix equipment references.\n\n"
        "Please consider filing a bug report and sending your save.",
        "Tell the user that equipment references couldn't be fixed, and give further instructions."));
      notif->PopMode();
    }
  }
  else {
    qInfo("No issues found in equipment box references.");

    notif->ShowMessage(tr("No issues found in equipment box references.",
      "Tell the user the there was no issues found in equipment box references."));
  }
}

void MHWISaveEditor::DebugRemoveUnobtainableItems()
{
  MHW_SAVE_GUARD;
  mhw_save_raw* mhwSave = MHW_Save();
  int mhwSaveIndex = MHW_SaveIndex();
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  qInfo("Removing unobtainable items...");
  int removed = MHWSaveOperations::RemoveUnobtainableItems(mhwSaveSlot, itemDB);
  qInfo().nospace() << removed << " unobtainable items were removed.";

  SaveLoader* loader = GetActiveEditorTab();
  if (loader == inventoryEditor) loader->Load(mhwSave, mhwSaveIndex);

  Notification* notif = notif->GetInstance();
  notif->ShowMessage(tr("Unobtainable items removed: %1.",
    "Tell the user how many unobtainable items have been removed, %1 is how many were removed.").arg(removed));
}
