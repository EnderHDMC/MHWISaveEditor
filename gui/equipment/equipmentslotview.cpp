#include "equipmentslotview.h"
#include "ui_equipmentslotview.h"

#include "../../data/SmithyDB.h"
#include "../../utility/mhw_save_utils.h"
#include "../../types/mhw_struct_constants.h"

#include "../common/WheelGuard.h"

EquipmentSlotView::EquipmentSlotView(int index, QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::EquipmentSlotView();
  ui->setupUi(this);

  WheelGuard* guard = guard->GetInstance();
  ui->btnUncraft->installEventFilter(guard);

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

  bool referenced = CountEquipmentReferenced(mhwSaveSlot, equipmentSlot);
  bool empty = IsEquipmentEmpty(equipmentSlot);
  if (!info && !empty) {
    qCritical().nospace() << "Invalid equipment detected, equipment info: "
      << "index = " << equipslot
      << ", category = " << equipment->category
      << ", type = " << equipment->type
      << ", id = " << equipment->id;
  }

  UpdateEquipDisplay(equipmentSlot, !empty && !referenced);
  SaveLoader::FinishLoad();
}

void EquipmentSlotView::Uncraft()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  SmithyDB* smithyDB = smithyDB->GetInstance();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  int equipIndex = mhwSaveSlot->equipment_index_table[equipslot];
  mhw_equipment* equipment = mhwSaveSlot->equipment;
  mhw_equipment* equipmentSlot = equipment + equipIndex;

  // Disallow uncrafting for used equipment or empty equipment.
  bool referenced = CountEquipmentReferenced(mhwSaveSlot, equipmentSlot);
  bool empty = IsEquipmentEmpty(equipmentSlot);
  if (empty || referenced) return;

  qInfo().noquote() << "Uncrafting:" << equipmentDB->GetName(equipmentSlot);
  QList<mhw_item_slot> mats = smithyDB->GetLineCraftingMats(equipmentSlot);
  for (int i = 0; i < mats.count(); i++)
  {
    mhw_item_slot mat = mats[i];
    itm_entry* info = itemDB->GetItemById(mat.id);
    mhw_item_slot* addSlot = FindCategoryItemOrEmpty(mhwSaveSlot, info);

    qInfo().noquote() << QString("\t%1 (%2) x %3").arg(itemDB->ItemName(info)).arg(mat.id).arg(mat.amount);
    if (addSlot) GiveItem(addSlot, &mat);
  }

  ClearEquipmentSlot(equipmentSlot);
  ui->btnUncraft->clearFocus(); // Prevent scrolling to the next widget.
  UpdateEquipDisplay(equipmentSlot, false);
}

void EquipmentSlotView::UpdateEquipDisplay(mhw_equipment* equipment, bool uncraftable)
{
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  QIcon* icon = bitmapDB->EquipmentIcon(equipment);
  QString name = equipmentDB->GetName(equipment);

  ui->btnUncraft->setEnabled(uncraftable);

  ui->btnIcon->setIcon(*icon);
  ui->btnIcon->setText(name);
}
