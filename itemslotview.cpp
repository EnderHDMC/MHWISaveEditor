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
  QString path = QString("res/ItemIcons/%1_%2.png").arg(info->icon_id).arg(info->icon_color);
  QIcon* bmp = bitmapDB->Pixmap(path);

  if (bmp)
    ui->toolButton->setIcon(*bmp);

  ui->toolButton->setText(QString::fromUtf8(info->name));
}

void ItemSlotView::UpdateMaxAmount(itemInfo* info, mhw_item_slot* item_slot)
{
  int max = (area->storage) ? 9999 : info->carry_limit;
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
      // TODO: More here
    }
  }
}