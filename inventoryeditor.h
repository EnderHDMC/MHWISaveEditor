#pragma once

#include <QWidget>

#include "itemslotview.h"
#include "types/inventoryAreas.h"

namespace Ui { class InventoryEditor; };

class InventoryEditor : public QWidget
{
  Q_OBJECT

public:
  InventoryEditor(const inventory_area* area, QWidget* parent = Q_NULLPTR);
  ~InventoryEditor();

private:
  Ui::InventoryEditor* ui;

  QList<ItemSlotView*> slotViews;
};
