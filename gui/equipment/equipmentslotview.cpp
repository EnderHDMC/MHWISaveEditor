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

  equipmentIndex.setParent(this);
  equipmentIndex.setGeometry(4, -4, 32, 32);
  equipmentIndex.show();
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
  const mhw_equipment* equipment = mhwSaveSlot->equipment + equipIndex;
  if (equipIndex < 0 || equipIndex >= COUNTOF(mhwSaveSlot->equipment))
    equipment = &MHW_EQUIPMENT_EMPTY;
  equipment_info* info = equipmentDB->GetEquipment(equipment);

  bool referenced = CountEquipmentReferenced(mhwSaveSlot, equipment);
  bool empty = IsEquipmentEmpty(equipment);
  if (!info && !empty && equipmentDB->HasData()) {
    qCritical().nospace() << "Invalid equipment detected, equipment info: "
      << "index = " << equipslot
      << ", category = " << equipment->category
      << ", type = " << equipment->type
      << ", id = " << equipment->id;
  }

  UpdateEquipDisplay(equipment, equipIndex, !empty && !referenced);
  SaveLoader::FinishLoad();
}

void EquipmentSlotView::PrimeLoad(mhw_save_raw* mhwSave, int slotIndex, bool loadFull)
{
  SaveLoader::PrimeLoad(mhwSave, slotIndex, loadFull);
  SaveLoader::FinishLoad();
}

void EquipmentSlotView::Uncraft()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  SmithyDB* smithyDB = smithyDB->GetInstance();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  int equipIndex = mhwSaveSlot->equipment_index_table[equipslot];
  mhw_equipment emptyEquipment = MHW_EQUIPMENT_EMPTY;
  mhw_equipment* equipment = mhwSaveSlot->equipment + equipIndex;
  if (equipIndex < 0 || equipIndex >= COUNTOF(mhwSaveSlot->equipment))
    equipment = &emptyEquipment;

  // Disallow uncrafting for used equipment or empty equipment.
  bool referenced = CountEquipmentReferenced(mhwSaveSlot, equipment);
  bool empty = IsEquipmentEmpty(equipment);
  if (empty || referenced) return;

  qInfo().noquote() << "Uncrafting:" << equipmentDB->GetName(equipment);
  QList<mhw_item_slot> mats = smithyDB->GetLineCraftingMats(equipment);
  for (int i = 0; i < mats.count(); i++)
  {
    mhw_item_slot mat = mats[i];
    itm_entry* info = itemDB->GetItemByIdSafe(mat.id);
    mhw_item_slot* addSlot = FindCategoryItemOrEmpty(mhwSaveSlot, info);

    qInfo().noquote() << QString("\t%1 (%2) x %3").arg(itemDB->ItemName(info)).arg(mat.id).arg(mat.amount);
    if (addSlot) GiveItem(addSlot, &mat);
  }

  ClearEquipmentSlot(equipment);
  ui->btnUncraft->clearFocus(); // Prevent scrolling to the next widget.
  UpdateEquipDisplay(equipment, equipIndex, false);
}

void EquipmentSlotView::UpdateEquipDisplay(const mhw_equipment* slot, int position, bool uncraftable)
{
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  QIcon* icon = bitmapDB->EquipmentIcon(slot);
  QString name = equipmentDB->GetName(slot);

  equipmentIndex.setText(QString::number(position));
  ui->btnUncraft->setEnabled(uncraftable);

  ui->btnIcon->setIcon(*icon);
  ui->btnIcon->setText(name);
}
