#include "equipmentslotview.h"
#include "ui_equipmentslotview.h"

#include <QMessageBox>

#include "../../data/SmithyDB.h"
#include "../../utility/mhw_save_operations.h"
#include "../../types/mhw_struct_constants.h"

#include "../common/WheelGuard.h"

EquipmentSlotView::EquipmentSlotView(int index, QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::EquipmentSlotView();
  ui->setupUi(this);

  WheelGuard* guard = guard->GetInstance();
  ui->btnEquip->installEventFilter(guard);
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

void EquipmentSlotView::Load(mhw_ib_save* mhwSave, mhw_ps4_save* ps4, int slotIndex)
{
  SaveLoader::Load(mhwSave, ps4, slotIndex);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  int index = 0;
  const mhw_equipment* equipment = MHWSaveUtils::GetEquipmentAtSlot(mhwSaveSlot, equipslot, index, &MHW_EQUIPMENT_EMPTY);
  equipment_info* info = equipmentDB->GetEquipment(equipment);

  bool empty = MHWSaveUtils::IsEquipmentEmpty(equipment);
  if (!info && !empty && equipmentDB->HasData()) {
    qCritical().nospace() << "Invalid equipment detected, equipment info: "
      << "index = " << equipslot
      << ", category = " << equipment->category
      << ", type = " << equipment->type
      << ", id = " << equipment->id;
  }

  bool referenced = MHWSaveUtils::CountEquipmentReferenced(mhwSaveSlot, index);
  bool equipped = MHWSaveUtils::CountEquipped(mhwSaveSlot, index);
  UpdateEquipDisplay(equipment, index, !referenced, equipped, empty);
  SaveLoader::FinishLoad();
}

void EquipmentSlotView::Equip()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  int index = 0;
  const mhw_equipment* equipment = MHWSaveUtils::GetEquipmentAtSlot(mhwSaveSlot, equipslot, index, &MHW_EQUIPMENT_EMPTY);

  MHWSaveUtils::EquipEquipment(mhwSaveSlot, equipment);
  emit Equipped(equipslot);
}

void EquipmentSlotView::Uncraft()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  SmithyDB* smithyDB = smithyDB->GetInstance();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  int index = 0;
  const mhw_equipment* equipment = MHWSaveUtils::GetEquipmentAtSlot(mhwSaveSlot, equipslot, index, &MHW_EQUIPMENT_EMPTY);
  
  bool confirm = true;
  equipment_info* info = equipmentDB->GetEquipment(equipment);
  if (equipmentDB->IsType(info, EquipmentInfoType::AM_DAT) && equipmentDB->IsPermanent(&info->am_dat)) {
    QMessageBox::StandardButton permanentConfirm =
      QMessageBox::question(this, "Confirm",
        tr("Are you sure you want to delete this permanent equipment item?\n\n%1",
          "Ask confirmation to delete a permanent equipment item. %1 is the equipment's name.")
        .arg(equipmentDB->GetNameArmor(equipment->type, equipment->id)));
    confirm &= permanentConfirm == QMessageBox::StandardButton::Yes;
  }

  if (confirm) {
    equipment = MHWSaveOperations::Uncraft(mhwSaveSlot, index, true, equipmentDB, smithyDB, itemDB);
    UpdateEquipDisplay(equipment, index, false, false, true);
  }
}

void EquipmentSlotView::UpdateEquipDisplay(const mhw_equipment* slot, int position, bool uncraftable, bool equipped, bool empty)
{
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  QIcon* icon = bitmapDB->EquipmentIcon(slot);
  QString name = equipmentDB->GetName(slot);

  equipmentIndex.setText(QString::number(position));

  ui->btnEquip->clearFocus();
  ui->btnEquip->setEnabled(!equipped && !empty);

  ui->btnUncraft->clearFocus(); // Prevent scrolling to the next widget.
  ui->btnUncraft->setEnabled(uncraftable && !empty);

  ui->btnIcon->setIcon(*icon);
  ui->btnIcon->setText(name);
}
