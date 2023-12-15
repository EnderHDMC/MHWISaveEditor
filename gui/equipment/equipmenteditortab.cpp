#include "equipmenteditortab.h"
#include "ui_equipmenteditortab.h"

EquipmentEditorTab::EquipmentEditorTab(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::EquipmentEditorTab();
  ui->setupUi(this);

  u32 count = COUNTOF(mhw_save_slot::equipment);
  slotViews.resize(count);
  for (i64 i = 0; i < slotViews.size(); i++)
  {
    slotViews[i] = new EquipmentSlotView((i32)i, this);
    slotViews[i]->setFixedSize(128, 128);
    ui->gridLayoutScroll->addWidget(slotViews[i], (i32)(i / 8), (i32)(i % 8));
  }
}

EquipmentEditorTab::~EquipmentEditorTab()
{
  delete ui;
}

void EquipmentEditorTab::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  for (i64 i = 0; i < slotViews.count(); i++)
  {
    EquipmentSlotView* slotView = slotViews[i];
    slotView->Load(mhwSave, slotIndex);
  }

  SaveLoader::FinishLoad();
}

void EquipmentEditorTab::LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB)
{
  for (i64 i = 0; i < slotViews.count(); i++)
  {
    EquipmentSlotView* slotView = slotViews[i];
    slotView->LoadResources(itemDB, bitmapDB);
  }
}
