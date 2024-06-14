#pragma once
// This file is pretty much for save utility functions, much like `mhw_save_utils.h`.
// The difference is that this file is for stuff that depend on the databases.
// Or for things that require external libraries.

#include "mhw_save_utils.h"

#include "../data/SmithyDB.h"

#include "../types/inventory_areas.h"

#include <QList>

class MHWSaveOperations {
public:
  static const mhw_equipment* Uncraft(mhw_save_slot* save_slot, u32 slot, bool ignorePermanent, EquipmentDB* equipmentDB, SmithyDB* smithyDB, ItemDB* itemDB) {
    mhw_equipment* equipment = save_slot->equipment + slot;
    if (slot >= COUNTOF(save_slot->equipment))
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
    for (u32 i = 0; i < COUNTOF(save_slot->equipment); i++)
    {
      Uncraft(save_slot, i, ignorePermanent, equipmentDB, smithyDB, itemDB);
    }
  }

  static int RemoveUnobtainableItems(mhw_save_slot* save_slot, ItemDB* itemDB) {
    int removed = 0;

    for (int y = 0; y < COUNTOF(inventory_areas); ++y) {
      const inventory_area* area = &inventory_areas[y];
      mhw_item_slot* inventory = (mhw_item_slot*)((u8*)save_slot + area->localoffset);

      for (int i = 0; i < area->count; ++i) {
        mhw_item_slot* slot = &inventory[i];
        itm_entry* info = itemDB->GetItemByIdSafe(slot->id);

        bool remove = false;
        if (!info && itemDB->count()) {
          qCritical().nospace() << "Invalid item detected, item info: "
            << "storage = " << area->storage
            << ", storage type = " << (u32)area->type
            << ", index = " << i
            << ", id = " << slot->id
            << ", amount = " << slot->amount;
          remove = true;
        }
        else if (info && (!(info->flags & (u32)itemFlag::CustomObtainable) || info->type != (u32)area->type) && info->id) {
          qCritical().nospace() << "Illegal item detected, item info: "
            << "storage = " << area->storage
            << ", storage type = " << (u32)area->type
            << ", index = " << i
            << ", id = " << slot->id
            << ", amount = " << slot->amount
            << ", type = " << info->type
            << ", name = " << itemDB->ItemName(info);
          remove = true;
        }

        if (remove) {
          qInfo("Removing...");
          MHWSaveUtils::RemoveItem(slot);
          ++removed;
        }
      }
    }

    return removed;
  }

  static const void GiveAllItems(mhw_save_slot* save_slot, ItemDB* itemDB, int count) {
    for (u32 i = 0; i < itemDB->count(); i++) {
      itm_entry* info = itemDB->GetItemById(i);
      if (info->flags & (u32)itemFlag::CustomObtainable) {
        mhw_item_slot* item = MHWSaveUtils::FindCategoryItemOrEmpty(save_slot, info);
        if (!item) break;

        MHWSaveUtils::DiscoverItem(save_slot, info);
        MHWSaveUtils::GiveItem(item, info, count);
      }
    }
  }
};
