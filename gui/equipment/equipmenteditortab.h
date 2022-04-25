#pragma once

#include <QWidget>

#include "../../utility/common/saveloader.h"
#include <equipmentslotview.h>

namespace Ui { class EquipmentEditorTab; };

class EquipmentEditorTab : public QWidget, public SaveLoader
{
  Q_OBJECT

public:
  EquipmentEditorTab(QWidget* parent = Q_NULLPTR);
  ~EquipmentEditorTab();

  // Inherited via SaveLoader
  virtual void Load(mhw_save_raw* mhwSave, int slotIndex) override;
  virtual void LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB) override;

private:
  Ui::EquipmentEditorTab* ui;

  QList<EquipmentSlotView*> slotViews;
};
