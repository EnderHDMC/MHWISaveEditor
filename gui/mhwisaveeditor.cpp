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
  setWindowIcon(QIcon("res/icon.ico"));

  slotSignalMapper = new QSignalMapper(this);
  switchSignalMapper = new QSignalMapper(this);
  cloneSignalMapper = new QSignalMapper(this);
  slotActions = { ui->actionSlot1 , ui->actionSlot2, ui->actionSlot3 };
  switchActions = { ui->actionSwitchSlot1 , ui->actionSwitchSlot2, ui->actionSwitchSlot3 };
  cloneActions = { ui->actionCloneSlot1, ui->actionCloneSlot2, ui->actionCloneSlot3 };
  for (int i = 0; i < slotActions.size(); i++) {
    slotActions[i]->setChecked(i == saveslot);

    connect(slotActions[i], SIGNAL(triggered()), slotSignalMapper, SLOT(map()));
    slotSignalMapper->setMapping(slotActions[i], i);

    connect(switchActions[i], SIGNAL(triggered()), switchSignalMapper, SLOT(map()));
    switchSignalMapper->setMapping(switchActions[i], i);

    connect(cloneActions[i], SIGNAL(triggered()), cloneSignalMapper, SLOT(map()));
    cloneSignalMapper->setMapping(cloneActions[i], i);

    switchActions[i]->setEnabled(i != saveslot);
    cloneActions[i]->setEnabled(i != saveslot);
  }
  connect(slotSignalMapper, SIGNAL(mappedInt(int)), this, SLOT(Slot(int)));
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
    {new InventoryEditor(), (SaveLoader**)&inventoryEditor, tr("Inventory Editor")},
    {new LimitedUnlocks(), (SaveLoader**)&limitedUnlocks, tr("Limited Unlocks")}
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

void MHWISaveEditor::SaveFile(const QString& path, mhw_save_raw** save, bool encrypt, bool validate)
{
  qInfo("Saving: %s", qUtf8Printable(path));
  qInfo("Validate: %s", validate ? "True" : "False");
  qInfo("Encrypted: %s", encrypt ? "True" : "False");

  mhw_save_raw* savep = *save;
  mhw_save_raw* saveWrite = (mhw_save_raw*)malloc(sizeof(mhw_save_raw));
  if (!saveWrite) {
    qInfo("Error allocating memory.");
    return;
  }

  memcpy(saveWrite, savep, sizeof(mhw_save_raw));
  if (validate) ValidateSaveFile(&saveWrite->save);
  if (encrypt) EncryptSave(saveWrite->data, sizeof(mhw_save_raw));

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

  ui->tabWidget->setEnabled(true);
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
    SaveFile(filepath, &mhwRaw, encrypt, true);
  }
}

void MHWISaveEditor::LoadSaveSlot()
{
  if (!mhwRaw) return;

  int index = ui->tabWidget->currentIndex();
  SaveLoader* loader = editors[index];
  loader->Load(mhwRaw, saveslot);
}

void MHWISaveEditor::EditorTabChange(int editorIndex)
{
  if (!mhwRaw) return;

  int index = editorIndex;
  SaveLoader* loader = editors[index];
  loader->Load(mhwRaw, saveslot);
}

void MHWISaveEditor::Slot(int slot)
{
  qInfo("Selected slot index %d.", slot);
  saveslot = slot;

  for (size_t i = 0; i < slotActions.count(); i++)
  {
    slotActions[i]->setChecked(i == saveslot);
    switchActions[i]->setEnabled(i != saveslot);
    cloneActions[i]->setEnabled(i != saveslot);
  }

  LoadSaveSlot();
}

void MHWISaveEditor::SwitchSlot(int slot)
{
  if (!mhwRaw) return;

  mhw_save_slot* temp = (mhw_save_slot*)malloc(sizeof(mhw_save_slot));
  mhw_save_slot* saveA = &mhwRaw->save.section3.Saves[saveslot];
  mhw_save_slot* saveB = &mhwRaw->save.section3.Saves[slot];
  if (!temp) {
    qWarning("Error allocating memory.");
    return;
  };

  memcpy_s(temp, sizeof(mhw_save_slot), saveA, sizeof(mhw_save_slot));
  memcpy_s(saveA, sizeof(mhw_save_slot), saveB, sizeof(mhw_save_slot));
  memcpy_s(saveB, sizeof(mhw_save_slot), temp, sizeof(mhw_save_slot));
  free(temp);

  LoadSaveSlot();
}

void MHWISaveEditor::CloneSlot(int slot)
{
  if (!mhwRaw) return;

  mhw_save_slot* saveA = &mhwRaw->save.section3.Saves[saveslot];
  mhw_save_slot* saveB = &mhwRaw->save.section3.Saves[slot];

  memcpy_s(saveB, sizeof(mhw_save_slot), saveA, sizeof(mhw_save_slot));
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
