#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QMessageBox>
#include <QSignalMapper>
#include <QFileSystemWatcher>

#include "../types/mhw_save.h"

#include "../utility/common/saveloader.h"
#include "../utility/common/Settings.h"

// Scroll wheel guard
#include "common/WheelGuard.h"

struct editor_tab {
  SaveLoader* widget;
  SaveLoader** binding;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MHWISaveEditor; }
QT_END_NAMESPACE

class MHWISaveEditor : public QMainWindow, public SaveLoader
{
  Q_OBJECT

public slots:
  // File menu
  void Open();
  void OpenSAVEDATA1000();
  void Save();
  void SaveAs();
  void Dump(int number);
  void ExportDecoList();

  void Backup();
  void Restore();

  // Slot functions
  void SelectSlot(int slot);
  void SwitchSlot(int slot);
  void CloneSlot(int slot);

  // Tools
  void UncraftUnusedEquipment();
  void GiveAllItems();
  
  // Management
  void OpenSettings();
  void EditorTabChange(int editorIndex);

  // Utility
  void OpenLocation(const QString& location);

  // Fixes
  void DebugDefragEquipment();
  void DebugFixEquipmentBoxRef();
  void DebugRemoveUnobtainableItems();
  
  // Debug
  void DebugDumpIconsAll();
  void DebugUtility();

public:
  MHWISaveEditor(QWidget* parent = nullptr);
  ~MHWISaveEditor();

  void closeEvent(QCloseEvent* event);

  // Inherited via SaveLoader
  virtual void LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB) override;

private:
  bool SaveFileEncrypt(const QString& path, mhw_save_raw* save, bool encrypt = true, bool validate = false);
  bool LoadFile(const QString& path, mhw_save_raw** save);
  void SaveFile(const QString& path);

  void LoadSaveSlot();

  void TrimBackups();

  void SetupDarkMode();

  void LoadItemLanguage(game_language language, bool reloadItemSearch = false);

  SaveLoader* GetActiveEditorTab();

  // Inherited via SaveLoader
  virtual void Load(mhw_save_raw* mhwSave, int slotIndex = -1) override;
  virtual void LoadFile(const QString& file) override;

  Ui::MHWISaveEditor* ui;
  QLabel* statusFile = nullptr;
  QMessageBox* msgNotification = nullptr;

  SaveLoader* inventoryEditor = nullptr;
  SaveLoader* hunterInfo = nullptr;
  SaveLoader* limitedUnlocks = nullptr;
  SaveLoader* generalInfo = nullptr;
  SaveLoader* equipmentEditor = nullptr;
  QList<SaveLoader*> editors;

  QSignalMapper* slotSignalMapper;
  QSignalMapper* switchSignalMapper;
  QSignalMapper* cloneSignalMapper;
  QSignalMapper* openSignalMapper;
  QSignalMapper* dumpSignalMapper;

  QList<QAction*> selectSlotActions;
  QList<QAction*> switchSlotActions;
  QList<QAction*> cloneSlotActions;

  QMap<QString, QString> ext_map;
  QMap<QString, bool> encrypt_map;
  QStringList filters;

  WheelGuard* scrollGuard = nullptr;

  QString steamUser = NULL;

  //////// Settings ////////
  Settings *settings = nullptr;
};
