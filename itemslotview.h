#pragma once

#include <QWidget>
#include "types/inventory_areas.h"
#include "data/ItemDB.h"
#include "data/BitmapDB.h"

namespace Ui { class ItemSlotView; };

class ItemSlotView : public QWidget
{
  Q_OBJECT

public:
  ItemSlotView(const inventory_area* area, int slot, QWidget* parent = Q_NULLPTR);
  ~ItemSlotView();

  void Load(mhw_save_raw* mhwSave, int saveslot);

  void UpdateItemDisplay(itemInfo *info);
  void UpdateMaxAmount(itemInfo* info, mhw_item_slot* item_slot);

public slots:
  void AmountChanged(int amount);

private:
  Ui::ItemSlotView* ui;

  const inventory_area* area;
  mhw_save_raw* mhwSave = nullptr;
  int saveslot = 0;
  int invslot = 0;

  bool loading = false;

  BitmapDB* bitmapDB = nullptr;
  ItemDB* itemDB = nullptr;
};
