#include "inventoryeditortab.h"
#include "ui_inventoryeditortab.h"

#include <QScrollBar>

InventoryEditorTab::InventoryEditorTab(const inventory_area* area, QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::InventoryEditorTab();
  ui->setupUi(this);

  slotViews.resize(area->count);
  for (size_t i = 0; i < slotViews.size(); i++)
  {
    slotViews[i] = new ItemSlotView(area, i, this);
    slotViews[i]->setFixedSize(128, 128);
    ui->gridLayoutScroll->addWidget(slotViews[i], i / 8, i % 8);
  }
}

InventoryEditorTab::~InventoryEditorTab()
{
  delete ui;
}

void InventoryEditorTab::ScrollToIndex(int index)
{
  ItemSlotView *itemView = slotViews[index];
  ui->scrollArea->ensureWidgetVisible(itemView);
}

void InventoryEditorTab::Load(mhw_save_raw* mhwSave, int mhwSaveSlot)
{
  SaveLoader::Load(mhwSave, mhwSaveSlot);

  for (size_t i = 0; i < slotViews.count(); i++)
  {
    ItemSlotView* slotView = slotViews[i];
    slotView->Load(mhwSave, mhwSaveSlot);
  }
}
