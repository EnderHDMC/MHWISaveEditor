#pragma once

#include <QWidget>
#include "types/inventoryAreas.h"

namespace Ui { class ItemSlotView; };

class ItemSlotView : public QWidget
{
  Q_OBJECT

public:
  ItemSlotView(const inventory_area* area, int slot, QWidget* parent = Q_NULLPTR);
  ~ItemSlotView();

  void Load(MHWSaveRaw* mhwSave, int saveslot);

private:
  Ui::ItemSlotView* ui;

  MHWSaveRaw* mhwSave = nullptr;
  const inventory_area* area;
  int saveslot = 0;
  int invslot = 0;
};
