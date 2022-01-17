#pragma once

#include <QWidget>
#include <QSignalMapper>

#include "itemslotview.h"

#include "../../types/inventory_areas.h"

namespace Ui { class InventoryEditorTab; };

class InventoryEditorTab : public QWidget
{
  Q_OBJECT

public:
  InventoryEditorTab(const inventory_area* area, QWidget* parent = Q_NULLPTR);
  ~InventoryEditorTab();

  void Load(mhw_save_raw *save, int saveslot);

private:
  Ui::InventoryEditorTab* ui;

  QList<ItemSlotView*> slotViews;
};
