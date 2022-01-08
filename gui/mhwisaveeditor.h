#ifndef MHWISAVEEDITOR_H
#define MHWISAVEEDITOR_H

#include <QMainWindow>
#include <QSignalMapper>
#include "../types/mhw_save.h"

#include "itemslotview.h"
#include "inventoryeditor.h"

constexpr const char* ALL_SAVE = "All Files (*)";
constexpr const char* ENCRYPTED_SAVE = "Encrypted Save (*.raw)";
constexpr const char* UNENCRYPTED_SAVE = "Unencrypted Save(*.bin)";

QT_BEGIN_NAMESPACE
namespace Ui { class MHWISaveEditor; }
QT_END_NAMESPACE

class MHWISaveEditor : public QMainWindow
{
  Q_OBJECT

public slots:
  void Open();
  void OpenSAVEDATA1000();
  void Save();
  void Slot(int slot);
  void OpenLocation(const QString& location);
  void Backup();
  void Restore();
  void Dump(int number);

public:
  MHWISaveEditor(QWidget* parent = nullptr);
  ~MHWISaveEditor();

  void closeEvent(QCloseEvent* event);

private:
  void SaveFile(const QString& path, mhw_save_raw** save, bool encrypt);
  void LoadFile(const QString& path, mhw_save_raw** save);

  Ui::MHWISaveEditor* ui;
  mhw_save_raw* mhwRaw = nullptr;
  int saveslot = 0;

  QSignalMapper* slotSignalMapper;
  QSignalMapper* openSignalMapper;
  QSignalMapper* dumpSignalMapper;

  QList<QAction*> slotActions;
  QList<InventoryEditor*> inventoryEditors;

  void LoadSaveSlot();
};
#endif // MHWISAVEEDITOR_H
