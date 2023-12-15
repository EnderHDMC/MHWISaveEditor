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

  bool referenced = MHWSaveUtils::CountEquipmentReferenced(mhwSaveSlot, equipment);
  bool empty = MHWSaveUtils::IsEquipmentEmpty(equipment);
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

void EquipmentSlotView::Uncraft()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  SmithyDB* smithyDB = smithyDB->GetInstance();
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  bool confirm = true;
  u32 index = mhwSaveSlot->equipment_index_table[equipslot];
  const mhw_equipment* equipment = mhwSaveSlot->equipment + index;
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
    ui->btnUncraft->clearFocus(); // Prevent scrolling to the next widget.
    UpdateEquipDisplay(equipment, index, false);
  }
}

void EquipmentSlotView::UpdateEquipDisplay(const mhw_equipment* slot, int position, bool uncraftable)
{
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  QIcon* icon = bitmapDB->EquipmentIcon(slot);
  QString name = equipmentDB->GetName(slot);

  equipmentIndex.setText(QString::number(position));
  ui->btnUncraft->setEnabled(uncraftable);

  ui->btnIcon->setIcon(*icon);
  ui->btnIcon->setText(name);
}
