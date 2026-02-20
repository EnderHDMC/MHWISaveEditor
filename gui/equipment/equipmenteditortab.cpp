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

    connect(slotViews[i], SIGNAL(Equipped(int)), this, SLOT(Equip(int)));

    ui->gridLayoutScroll->addWidget(slotViews[i], (i32)(i / 8), (i32)(i % 8));
  }
}

EquipmentEditorTab::~EquipmentEditorTab()
{
  delete ui;
}

void EquipmentEditorTab::Load(mhw_ib_save* mhwSave, mhw_ps4_save* ps4, int slotIndex)
{
  SaveLoader::Load(mhwSave, ps4, slotIndex);

  LoadSlotViews(mhwSave, slotIndex);

  SaveLoader::FinishLoad();
}

void EquipmentEditorTab::LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB)
{
  SaveLoader::LoadResources(itemDB, bitmapDB);
  for (i64 i = 0; i < slotViews.count(); i++)
  {
    EquipmentSlotView* slotView = slotViews[i];
    slotView->LoadResources(itemDB, bitmapDB);
  }
}

void EquipmentEditorTab::LoadSlotViews(mhw_ib_save* mhwSave, int slotIndex)
{
  for (i64 i = 0; i < slotViews.count(); i++)
  {
    EquipmentSlotView* slotView = slotViews[i];
    slotView->Load(mhwSave, nullptr, slotIndex);
  }
}

void EquipmentEditorTab::Equip(int index) {
  mhw_ib_save* mhwSave = MHW_SaveIB();
  int slotIndex = MHW_SaveIndex();

  LoadSlotViews(mhwSave, slotIndex);
}
