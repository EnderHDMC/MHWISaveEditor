#include "itemslotview.h"
#include "ui_itemslotview.h"

#include "../common/WheelGuard.h"

ItemSlotView::ItemSlotView(const inventory_area* area, int slot, QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::ItemSlotView();
  ui->setupUi(this);

  bitmapDB = bitmapDB->GetInstance();
  itemDB = itemDB->GetInstance();

  WheelGuard* guard = new WheelGuard(ui->spnCount);
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
  itemInfo* info = itemDB->GetItemById(itemSlot->id);
  if (!info) {
    // TODO: Think about what we want to do here.
  }

  UpdateItemDisplay(info);
  UpdateMaxAmount(info, itemSlot);

  SaveLoader::FinishLoad();
}

void ItemSlotView::UpdateItemDisplay(itemInfo* info)
{
  QIcon* icon = bitmapDB->ItemIcon(info);
  QString itemName = itemDB->ItemName(info);

  ui->btnIcon->setIcon(icon ? *icon : QIcon());
  ui->btnIcon->setText(itemName);
}

void ItemSlotView::UpdateMaxAmount(itemInfo* info, mhw_item_slot* item_slot)
{
  Q_ASSERT(info->id == item_slot->id);
  int max = (area->storage) ? 9999 : info->carry_limit;

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
      itemInfo* dummyInfo = itemDB->GetItemById(dummy.id);
      itemInfo* info = itemDB->GetItemById(itemSlot->id);

      UpdateItemDisplay(dummyInfo);
      UpdateMaxAmount(info, itemSlot);
    }
  }
}