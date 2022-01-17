#pragma once

#include <QWidget>

#include "../../types/inventory_areas.h"
#include "../../data/ItemDB.h"
#include "../../data/BitmapDB.h"
#include "../common/saveloader.h"

namespace Ui { class ItemSlotView; };

class ItemSlotView : public QWidget, public SaveLoader
{
  Q_OBJECT

public:
  ItemSlotView(const inventory_area* area, int slot, QWidget* parent = Q_NULLPTR);
  ~ItemSlotView();

  // Inherited via SaveLoader
  virtual void Load(mhw_save_raw* mhwSave, int mhwSaveSlot) override;

  void UpdateItemDisplay(itemInfo *info);
  void UpdateMaxAmount(itemInfo* info, mhw_item_slot* item_slot);

public slots:
  void AmountChanged(int amount);

private:
  Ui::ItemSlotView* ui;

  const inventory_area* area;
  int invslot = 0;

  bool loading = false;

  BitmapDB* bitmapDB = nullptr;
  ItemDB* itemDB = nullptr;
};