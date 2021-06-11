#pragma once

#include <QWidget>
#include <QSignalMapper>

#include "itemslotview.h"

#include "types/inventory_areas.h"

namespace Ui { class InventoryEditor; };

class InventoryEditor : public QWidget
{
  Q_OBJECT

public:
  InventoryEditor(const inventory_area* area, QWidget* parent = Q_NULLPTR);
  ~InventoryEditor();

  void Load(mhw_save_raw *save, int saveslot);

private:
  Ui::InventoryEditor* ui;

  QList<ItemSlotView*> slotViews;
};
