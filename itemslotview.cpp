#include "itemslotview.h"
#include "ui_itemslotview.h"
#include "data/BitmapDB.h"
#include "data/ItemDB.h"
#include <QMessageBox>

ItemSlotView::ItemSlotView(const inventory_area* area, int slot, QWidget* parent)
  : QWidget(parent)
{
  ui = new Ui::ItemSlotView();
  ui->setupUi(this);

  ui->toolButton->setIconSize({ 64, 64 });
  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  this->area = area;
  this->invslot = slot;
}

ItemSlotView::~ItemSlotView()
{
  delete ui;
}

void ItemSlotView::Load(MHWSaveRaw* mhwSave, int saveslot)
{
  this->mhwSave = mhwSave;
  this->saveslot = saveslot;

  BitmapDB* bitmapDB = bitmapDB->GetInstance();
  ItemDB* itemDB = itemDB->GetInstance();

  u8* slot = ((u8*)(&mhwSave->save.Saves[saveslot])) + area->localoffset;
  MHWItemSlot* itemSlot = ((MHWItemSlot*)(slot)+invslot);
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

  QString path = QString("res/ItemIcons/%1_%2.png").arg(info->iconID).arg(info->iconColor);
  QPixmap* bmp = bitmapDB->Pixmap(path);

  ui->toolButton->setIcon(QIcon(*bmp));
  ui->toolButton->setText(QString::fromUtf8(info->name));
  ui->spinBox->setValue(itemSlot->amount);
}
