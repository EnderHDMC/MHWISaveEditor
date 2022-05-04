#pragma once
// This file is pretty much for save utility functions, much lie `mhw_save_utils.h`.
// The difference is that this file is for stuff that depend on the databases.
// Or for things that require external libraries.

#include "mhw_save_utils.h"

#include "../data/SmithyDB.h"

#include <QList>

class MHWSaveOperations {
public:
  static const mhw_equipment* Uncraft(mhw_save_slot* save_slot, int slot, bool ignorePermanent, EquipmentDB* equipmentDB, SmithyDB* smithyDB, ItemDB* itemDB) {
    mhw_equipment* equipment = save_slot->equipment + slot;
    if (slot < 0 || slot >= COUNTOF(save_slot->equipment))
      return &MHW_EQUIPMENT_EMPTY;

    // Disallow uncrafting for used equipment or empty equipment.
    bool referenced = MHWSaveUtils::CountEquipmentReferenced(save_slot, equipment);
    bool empty = MHWSaveUtils::IsEquipmentEmpty(equipment);
    if (empty || referenced) return equipment;

    bool isPermanent = false;
    equipment_info* info = equipmentDB->GetEquipment(equipment);
    if (equipmentDB->IsType(info, EquipmentInfoType::AM_DAT)) {
      isPermanent = EquipmentDB::IsPermanent(&info->am_dat);
    }

    if (!isPermanent || ignorePermanent) {
      qInfo().noquote() << "Uncrafting:" << equipmentDB->GetName(equipment);
      QList<mhw_item_slot> mats = smithyDB->GetLineCraftingMats(info, equipment);
      for (int i = 0; i < mats.count(); i++)
      {
        mhw_item_slot mat = mats[i];
        itm_entry* info = itemDB->GetItemByIdSafe(mat.id);
        mhw_item_slot* addSlot = MHWSaveUtils::FindCategoryItemOrEmpty(save_slot, info);

        qInfo().noquote() << QString("\t%1 (%2) x %3").arg(itemDB->ItemName(info)).arg(mat.id).arg(mat.amount);
        if (addSlot) MHWSaveUtils::GiveItem(addSlot, &mat);
      }
      MHWSaveUtils::ClearEquipmentSlot(equipment);
    }
    else {
      qInfo().noquote() << "Skipping permanent equipment: " << equipmentDB->GetName(equipment);;
    }

    return equipment;
  }

  static void UncraftAllUnusedEquipment(mhw_save_slot* save_slot, bool ignorePermanent, EquipmentDB* equipmentDB, SmithyDB* smithyDB, ItemDB* itemDB) {
    for (size_t i = 0; i < COUNTOF(save_slot->equipment); i++)
    {
      Uncraft(save_slot, i, ignorePermanent, equipmentDB, smithyDB, itemDB);
    }
  }
};
