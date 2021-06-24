#include "itemslotview.h"
#include "ui_itemslotview.h"

#include <QMessageBox>

ItemSlotView::ItemSlotView(const inventory_area* area, int slot, QWidget* parent)
  : QWidget(parent)
{
  ui = new Ui::ItemSlotView();
  ui->setupUi(this);

  ui->toolButton->setIconSize({ 64, 64 });
  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  bitmapDB = bitmapDB->GetInstance();
  itemDB = itemDB->GetInstance();

  this->area = area;
  this->invslot = slot;
}

ItemSlotView::~ItemSlotView()
{
  delete ui;
}

void ItemSlotView::Load(mhw_save_raw* mhwSave, int saveslot)
{
  loading = true;

  this->mhwSave = mhwSave;
  this->saveslot = saveslot;

  u8* slot = ((u8*)(&mhwSave->save.section3.Saves[saveslot])) + area->localoffset;
  mhw_item_slot* itemSlot = ((mhw_item_slot*)(slot)+invslot);
  itemInfo* info = itemDB->GetItemById(itemSlot->id);
  if (!info) {
    QMessageBox msgBox;
    msgBox.setText("Invalid item. This is probably an Iceborne item not in the database");
    msgBox.setInformativeText("You may still edit your save, you just won't be able to see the invalid item.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    return;
  }

  UpdateItemDisplay(info);
  UpdateMaxAmount(info, itemSlot);

  loading = false;
}

void ItemSlotView::UpdateItemDisplay(itemInfo* info)
{
  QIcon* icon = bitmapDB->ItemIcon(info);

  ui->toolButton->setIcon(icon ? *icon : QIcon());
  ui->toolButton->setText(QString::fromUtf8(info->name));
}

void ItemSlotView::UpdateMaxAmount(itemInfo* info, mhw_item_slot* item_slot)
{
  Q_ASSERT(info->id == item_slot->id);
  
  int max = (area->storage) ? 9999 : info->carry_limit;
  if (info->id == 0) max = 0;
  Q_ASSERT((item_slot->amount == 0 && item_slot->id == 0)
    || (item_slot->amount <= max && item_slot->amount > 0 && item_slot->id > 0));

  ui->spinBox->setMaximum(max);
  ui->spinBox->setValue(item_slot->amount);
}

void ItemSlotView::AmountChanged(int amount)
{
  if (!loading) {
    u8* slot = ((u8*)(&mhwSave->save.section3.Saves[saveslot])) + area->localoffset;
    mhw_item_slot* itemSlot = ((mhw_item_slot*)(slot)+invslot);

    itemSlot->amount = amount;
    if (amount == 0) {
      itemSlot->id = 0;

      itemInfo* info = itemDB->GetItemById(itemSlot->id);
      UpdateItemDisplay(info);
      UpdateMaxAmount(info, itemSlot);
    }
  }
}