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
  itemView->SetFocus();
}

void InventoryEditorTab::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  for (size_t i = 0; i < slotViews.count(); i++)
  {
    ItemSlotView* slotView = slotViews[i];
    slotView->Load(mhwSave, slotIndex);
  }

  SaveLoader::FinishLoad();
}

void InventoryEditorTab::LoadIndex(int index)
{
  SaveLoader::LoadIndex(index);
  mhw_save_raw* mhwSave = MHW_Save();
  int slotIndex = MHW_SaveIndex();

  ItemSlotView* slotView = slotViews[index];
  slotView->Load(mhwSave, slotIndex);

  SaveLoader::FinishLoad();
}

void InventoryEditorTab::LoadResources(ItemDB* itemDB)
{
  for (size_t i = 0; i < slotViews.count(); i++)
  {
    ItemSlotView* slotView = slotViews[i];
    slotView->LoadResources(itemDB);
  }
}
