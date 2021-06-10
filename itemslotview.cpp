#include "itemslotview.h"
#include "ui_itemslotview.h"
#include "data/BitmapDB.h"
#include "data/ItemDB.h"

ItemSlotView::ItemSlotView(QWidget *parent)
    : QWidget(parent)
{
    ui = new Ui::ItemSlotView();
    ui->setupUi(this);
}

ItemSlotView::~ItemSlotView()
{
    delete ui;
}

void ItemSlotView::Load(MHWSaveRaw* mhwSave, int saveslot, int slot)
{
  this->mhwSave = mhwSave;
  this->saveslot = saveslot;
  this->slot = slot;

  BitmapDB* bitmapDB = bitmapDB->GetInstance();
  ItemDB* itemDB = itemDB->GetInstance();
  itemInfo* info = itemDB->GetItemById(mhwSave->save.Saves[this->saveslot].ItemPouch.items[this->slot].id);

  QString path = QString("res/ItemIcons/%1_%2.png").arg(info->iconID).arg(info->iconColor);
  QPixmap* bmp = bitmapDB->Pixmap(path);

  ui->toolButton->setIcon(QIcon(*bmp));
  ui->toolButton->setIconSize({ 64, 64 });
  ui->toolButton->setText(QString::fromUtf8(info->name));
  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  ui->spinBox->setValue(mhwSave->save.Saves[this->saveslot].ItemPouch.items[this->slot].amount);
}
