#pragma once

#include <QWidget>
#include <QSignalMapper>

#include "itemslotview.h"

#include "../common/saveloader.h"
#include "../../types/inventory_areas.h"

namespace Ui { class InventoryEditorTab; };

class InventoryEditorTab : public QWidget, public SaveLoader
{
  Q_OBJECT

public:
  InventoryEditorTab(const inventory_area* area, QWidget* parent = Q_NULLPTR);
  ~InventoryEditorTab();

  // Inherited via SaveLoader
  virtual void Load(mhw_save_raw* mhwSave, int mhwSaveSlot) override;

private:
  Ui::InventoryEditorTab* ui;

  QList<ItemSlotView*> slotViews;
};
