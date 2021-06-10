#include "inventoryeditor.h"
#include "ui_inventoryeditor.h"

InventoryEditor::InventoryEditor(const inventory_area* area, QWidget* parent)
  : QWidget(parent)
{
  ui = new Ui::InventoryEditor();
  ui->setupUi(this);

  slotViews.resize(area->count);
  
  for (size_t i = 0; i < slotViews.size(); i++)
  {
    slotViews[i] = new ItemSlotView(this);
    ui->gridLayout->addWidget(slotViews[i], i / 8, i % 8);
  }
}

InventoryEditor::~InventoryEditor()
{
  delete ui;
}
