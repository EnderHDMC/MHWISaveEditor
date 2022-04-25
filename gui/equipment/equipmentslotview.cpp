#include "equipmentslotview.h"
#include "ui_equipmentslotview.h"

EquipmentSlotView::EquipmentSlotView(int index, QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::EquipmentSlotView();
  ui->setupUi(this);

  equipslot = index;
}

EquipmentSlotView::~EquipmentSlotView()
{
  delete ui;
}

void EquipmentSlotView::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  int equipIndex = mhwSaveSlot->equipment_index_table[equipslot];
  mhw_equipment* equipment = mhwSaveSlot->equipment;
  mhw_equipment* equipmentSlot = equipment + equipIndex;
  equipment_info* info = equipmentDB->GetEquipment(equipmentSlot);
  if (!info) {
    // TODO: Think about what we want to do here.
  }

  UpdateEquipDisplay(equipmentSlot);

  SaveLoader::FinishLoad();
}

void EquipmentSlotView::UpdateEquipDisplay(mhw_equipment* equipment)
{
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  QIcon* icon = bitmapDB->EquipmentIcon(equipment);
  QString name = equipmentDB->GetName(equipment);

  ui->btnIcon->setIcon(icon ? *icon : QIcon());
  ui->btnIcon->setText(name);
}
