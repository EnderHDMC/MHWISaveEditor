#include "itemslotview.h"
#include "ui_itemslotview.h"

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
