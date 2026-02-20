#pragma once

#include <QWidget>
#include <QSignalMapper>

#include "../../utility/common/saveloader.h"
#include "equipmentslotview.h"

namespace Ui { class EquipmentEditorTab; };

class EquipmentEditorTab : public QWidget, public SaveLoader
{
  Q_OBJECT

public:
  EquipmentEditorTab(QWidget* parent = Q_NULLPTR);
  ~EquipmentEditorTab();

  // Inherited via SaveLoader
  virtual void Load(mhw_ib_save* mhwSave, mhw_ps4_save* ps4, int slotIndex) override;
  virtual void LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB) override;

  void LoadSlotViews(mhw_ib_save* mhwSave, int slotIndex);

public slots:
  void Equip(int index);

private:
  Ui::EquipmentEditorTab* ui;

  QList<EquipmentSlotView*> slotViews;
};
