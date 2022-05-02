#include "itemslotview.h"
#include "ui_itemslotview.h"

#include "../common/WheelGuard.h"

ItemSlotView::ItemSlotView(const inventory_area* area, int slot, QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::ItemSlotView();
  ui->setupUi(this);

  WheelGuard* guard = guard->GetInstance();
  ui->spnCount->installEventFilter(guard);

  this->area = area;
  this->invslot = slot;
}

ItemSlotView::~ItemSlotView()
{
  delete ui;
}

void ItemSlotView::SetFocus()
{
  ui->spnCount->setFocus(Qt::FocusReason::ShortcutFocusReason);
}

void ItemSlotView::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  u8* slot = ((u8*)mhwSaveSlot) + area->localoffset;
  mhw_item_slot* itemSlot = ((mhw_item_slot*)(slot)+invslot);
  itm_entry* info = itemDB->GetItemByIdSafe(itemSlot->id);
  if (!info) {
    qCritical().nospace() << "Invalid item detected, item info: "
      << "index = " << invslot
      << ", id = " << itemSlot->id
      << ", amount = " << itemSlot->amount;
  }

  UpdateItemDisplay(info);
  UpdateMaxAmount(info, itemSlot);

  SaveLoader::FinishLoad();
}

void ItemSlotView::UpdateItemDisplay(itm_entry* info)
{
  QIcon* icon = bitmapDB->ItemIcon(itemDB, info);
  QString itemName = itemDB->ItemName(info);

  ui->btnIcon->setIcon(*icon);
  ui->btnIcon->setText(itemName);
}

void ItemSlotView::UpdateMaxAmount(itm_entry* info, mhw_item_slot* item_slot)
{
  const int maxStorage = 9999;
  if (!info) {
    ui->spnCount->setEnabled(false);
    ui->spnCount->setMaximum(maxStorage);
    ui->spnCount->setValue(item_slot->amount);
    return;
  };

  Q_ASSERT(info->id == item_slot->id);
  int max = (area->storage) ? maxStorage : info->carry_limit;

  ui->spnCount->setMaximum(max);
  ui->spnCount->setValue(item_slot->amount);
}

void ItemSlotView::AmountChanged(int amount)
{
  if (MHW_SAVE_GUARD_CHECK)
  {
    ui->spnCount->setValue(0);
    return;
  }
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  if (MHW_LOADING_GUARD_CHECK) {
    u8* slot = ((u8*)mhwSaveSlot) + area->localoffset;
    mhw_item_slot* itemSlot = ((mhw_item_slot*)(slot)+invslot);
    mhw_item_slot dummy = { itemSlot->id, itemSlot->amount };

    bool update = amount == 0 || (itemSlot->amount == 0 && amount != 0);
    itemSlot->amount = amount;
    if (amount == 0) dummy.id = 0;

    if (update) {
      itm_entry* dummyInfo = itemDB->GetItemById(dummy.id);
      itm_entry* info = itemDB->GetItemById(itemSlot->id);

      UpdateItemDisplay(dummyInfo);
      UpdateMaxAmount(info, itemSlot);
    }
  }
}
