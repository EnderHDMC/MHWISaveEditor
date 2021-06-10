#pragma once

#include <QWidget>

#include "itemslotview.h"
#include "types/inventoryAreas.h"
#include <QSignalMapper>

namespace Ui { class InventoryEditor; };

class InventoryEditor : public QWidget
{
  Q_OBJECT

public:
  InventoryEditor(const inventory_area* area, QWidget* parent = Q_NULLPTR);
  ~InventoryEditor();

  void Load(MHWSaveRaw *save, int saveslot);

private:
  Ui::InventoryEditor* ui;

  QList<ItemSlotView*> slotViews;
};
