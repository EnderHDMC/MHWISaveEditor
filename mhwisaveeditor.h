#ifndef MHWISAVEEDITOR_H
#define MHWISAVEEDITOR_H

#include <QMainWindow>
#include <QSignalMapper>
#include "types/mhwSave.h"

#include "itemslotview.h"
#include "inventoryeditor.h"

constexpr char* ALL_SAVE = "All Files (*)";
constexpr char* ENCRYPTED_SAVE = "Encrypted Save (*.raw)";
constexpr char* UNENCRYPTED_SAVE = "Unencrypted Save(*.bin)";

QT_BEGIN_NAMESPACE
namespace Ui { class MHWISaveEditor; }
QT_END_NAMESPACE

class MHWISaveEditor : public QMainWindow
{
  Q_OBJECT

public slots:
  void Open();
  void Save();
  void Slot(int slot);
  void OpenLocation(const QString& location);
  void Backup();
  void Restore();

public:
  MHWISaveEditor(QWidget* parent = nullptr);
  ~MHWISaveEditor();

  void closeEvent(QCloseEvent* event);

private:
  Ui::MHWISaveEditor* ui;
  MHWSaveRaw* mhwRaw;

  QSignalMapper* slotSignalMapper;
  QSignalMapper* openSignalMapper;

  QList<InventoryEditor*> inventoryEditors;
};
#endif // MHWISAVEEDITOR_H
