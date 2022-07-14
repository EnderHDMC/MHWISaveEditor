#pragma once

#include "../types/mhw_save.h"
#include "../types/mhw_struct_constants.h"
#include "../types/file/itm.h"
#include "settype.h"

class MHWSaveUtils {
public:
  static bool IsBlowfishDecrypted(mhw_ib_save* save) {
    return save->header.magic == 0x00000001;
  }

  static mhw_item_slot* FindItem(mhw_item_slot* items, int count, u32 id) {
    mhw_item_slot* result = nullptr;
    for (int i = 0; i < count; i++)
    {
      if (items->id == id) {
        result = items;
        break;
      }
      items++;
    }

    return result;
  }

  static mhw_item_slot* FindItemOrEmpty(mhw_item_slot* items, int count, u32 id) {
    mhw_item_slot* result = nullptr;
    mhw_item_slot* empty = nullptr;
    for (int i = 0; i < count; i++)
    {
      if (!empty && !items->id) empty = items;

      if (items->id == id) {
        result = items;
        break;
      }
      items++;
    }

    if (!result) result = empty;
    return result;
  }

  static mhw_item_slot* FindCategoryItemOrEmpty(mhw_save_slot* save_slot, itm_entry* info) {
    mhw_storage* storage = &save_slot->storage;
    mhw_item_slot* items = nullptr;
    int count = 0;

    if (info) {
      switch (info->type) {
      case (u32)itemCategory::Item: {
        items = storage->items;
        count = COUNTOF(storage->items);
      } break;

      case (u32)itemCategory::Ammo: {
        items = storage->ammo;
        count = COUNTOF(storage->ammo);
      } break;

      case (u32)itemCategory::Material: {
        items = storage->materials;
        count = COUNTOF(storage->materials);
      } break;

      case (u32)itemCategory::Decoration: {
        items = storage->decorations;
        count = COUNTOF(storage->decorations);
      } break;

      case (u32)itemCategory::Account:
      case (u32)itemCategory::Furniture:
        break;

      default:
        break;
      }
    }

    mhw_item_slot* result = nullptr;
    if (items) result = FindItemOrEmpty(items, count, info->id);;
    return result;
  }

  static bool GiveItem(mhw_item_slot* item_slot, mhw_item_slot* add) {
    bool replace = item_slot->id != add->id;
    item_slot->id = add->id;
    item_slot->amount += add->amount;
    if (item_slot->amount > 9999) item_slot->amount = 9999;

    return replace;
  }

  static bool GiveItem(mhw_item_slot* item_slot, itm_entry* info, int count = 1) {
    bool replace = item_slot->id != info->id;
    item_slot->id = info->id;
    item_slot->amount += count;
    if (item_slot->amount > 9999) item_slot->amount = 9999;

    return replace;
  }

  static bool RemoveItem(mhw_item_slot* item_slot) {
    bool removed = item_slot->id;
    item_slot->id = 0;
    item_slot->amount = 0;

    return removed;
  }

  static inline void DiscoverItem(mhw_save_slot* save_slot, itm_entry* info) {
    u32 is = info->id / 8;
    u32 isi = info->id % 8;
    save_slot->discovered_items.items[is] |= 1 << isi;
  }

  static mhw_equipment* FindEquipment(mhw_save_slot* save_slot, i32 type, u32 id) {
    mhw_equipment* equipment = save_slot->equipment;
    u32* index_table = save_slot->equipment_index_table;
    int count = COUNTOF(save_slot->equipment);

    mhw_equipment* result = nullptr;
    for (int i = 0; i < count; i++)
    {
      int lookup = index_table[i];
      mhw_equipment* slot = &equipment[lookup];
      if (slot->type == type && slot->id == id) {
        result = slot;
        break;
      }
    }

    return result;
  }

  static inline void ClearEquipmentSlot(mhw_equipment* equipment) {
    i32 sort_index = equipment->sort_index;
    memcpy_s(equipment, sizeof(*equipment), &MHW_EQUIPMENT_EMPTY, sizeof(MHW_EQUIPMENT_EMPTY));
    equipment->sort_index = sort_index;
  }

  static inline bool IsEquipmentEmpty(const mhw_equipment* equipment) {
    return equipment->category == mhw_equip_category::Empty || equipment->type == -1;
  }

  static u32 CountEquipmentReferenced(const mhw_save_slot* save_slot, const mhw_equipment* ref) {
    mhw_equip_category category = ref->category;
    i32 type = ref->type;
    u32 id = ref->id;

    const mhw_equipment* equipment = save_slot->equipment;
    int srcIndex = ref - equipment;
    u32 referenced = false;

    // Check for references from equipped gear
    const mhw_current_equipment* current_equipment = &save_slot->current_equipment;
    referenced += current_equipment->weapon == srcIndex;
    referenced += current_equipment->helmet == srcIndex;
    referenced += current_equipment->torso == srcIndex;
    referenced += current_equipment->arms == srcIndex;
    referenced += current_equipment->coil == srcIndex;
    referenced += current_equipment->feet == srcIndex;
    referenced += current_equipment->charm == srcIndex;
    referenced += current_equipment->kinsect == srcIndex;

    // Check for references from loadouts
    const int count = COUNTOF(save_slot->equipment_loadouts);
    for (int i = 0; i < count; ++i) {
      const mhw_equipment_loadout* loadout = save_slot->equipment_loadouts + i;
      referenced += loadout->weapon_index == srcIndex;
      referenced += loadout->helmet_index == srcIndex;
      referenced += loadout->torso_index == srcIndex;
      referenced += loadout->arms_index == srcIndex;
      referenced += loadout->coil_index == srcIndex;
      referenced += loadout->feet_index == srcIndex;
      referenced += loadout->charm_index == srcIndex;
    }
    return referenced;
  }

  static mhw_layered_loadout* FindEmptyLayeredLoadout(mhw_save_slot* save_slot) {
    mhw_layered_loadout* loadouts = save_slot->layered_loadouts;
    int count = COUNTOF(save_slot->layered_loadouts);
    mhw_layered_loadout* result = nullptr;

    for (int i = 0; i < count; i++) {
      mhw_layered_loadout* current = &loadouts[i];

      if (current->name[0] == '\0') {
        result = current;
        break;
      }
    }

    return result;
  }

  static u32 SetLayeredLoadout(mhw_layered_loadout* loadout, i32 layered, const QString& name) {
    u32 index = loadout->slot_id;
    memcpy_s(loadout, sizeof(mhw_layered_loadout), &MHW_LAYERED_LOADOUT_EMPTY, sizeof(mhw_layered_loadout));

    loadout->slot_id = index;
    SetStr(&loadout->name, name);
    loadout->layered.head = layered;
    loadout->layered.torso = layered;
    loadout->layered.arms = layered;
    loadout->layered.coil = layered;
    loadout->layered.feet = layered;
    return index;
  }

  /// <summary>
  /// Updates all the references of current equipment &amp; loadouts to reference
  /// the equipment's sort_index instead, this will very likely crash the game.
  /// Should be followed by a call to 'void DefragEquipment(mhw_save_slot* save_slot)'
  /// to fix the crash that would otherwise happen.
  /// Assumes all the references are valid; -1 &lt;= reference &lt; equipment count
  /// </summary>
  /// <param name="save_slot">The save slot to defrag references for.</param>
  static inline void DefragEquipmentReferences(mhw_save_slot* save_slot) {
    const mhw_equipment* equipment = save_slot->equipment;

    // Assumes all the indices are valid
    // Update indices of currently equiped gear
    mhw_current_equipment* current_equipment = &save_slot->current_equipment;
    if (current_equipment->weapon > -1) current_equipment->weapon = equipment[current_equipment->weapon].sort_index;
    if (current_equipment->helmet > -1) current_equipment->helmet = equipment[current_equipment->helmet].sort_index;
    if (current_equipment->torso > -1) current_equipment->torso = equipment[current_equipment->torso].sort_index;
    if (current_equipment->arms > -1) current_equipment->arms = equipment[current_equipment->arms].sort_index;
    if (current_equipment->coil > -1) current_equipment->coil = equipment[current_equipment->coil].sort_index;
    if (current_equipment->feet > -1) current_equipment->feet = equipment[current_equipment->feet].sort_index;
    if (current_equipment->charm > -1) current_equipment->charm = equipment[current_equipment->charm].sort_index;
    if (current_equipment->kinsect > -1) current_equipment->kinsect = equipment[current_equipment->kinsect].sort_index;

    // Update indices of loadout gear
    const int count = COUNTOF(save_slot->equipment_loadouts);
    for (int i = 0; i < count; ++i) {
      mhw_equipment_loadout* loadout = save_slot->equipment_loadouts + i;
      if (loadout->weapon_index > -1) loadout->weapon_index = equipment[loadout->weapon_index].sort_index;
      if (loadout->helmet_index > -1) loadout->helmet_index = equipment[loadout->helmet_index].sort_index;
      if (loadout->torso_index > -1) loadout->torso_index = equipment[loadout->torso_index].sort_index;
      if (loadout->arms_index > -1) loadout->arms_index = equipment[loadout->arms_index].sort_index;
      if (loadout->coil_index > -1) loadout->coil_index = equipment[loadout->coil_index].sort_index;
      if (loadout->feet_index > -1) loadout->feet_index = equipment[loadout->feet_index].sort_index;
      if (loadout->charm_index > -1) loadout->charm_index = equipment[loadout->charm_index].sort_index;
    }
  }

  static inline void SwapEquipmentEntries(u32* index_table, mhw_equipment* a, mhw_equipment* b) {
    mhw_equipment temp;
    memcpy_s(&temp, sizeof(temp), a, sizeof(*a));
    memcpy_s(a, sizeof(*a), b, sizeof(*b));
    memcpy_s(b, sizeof(*b), &temp, sizeof(temp));
    index_table[a->sort_index] = index_table[b->sort_index];
    index_table[b->sort_index] = temp.sort_index;
  }

  /// <summary>
  /// Moves all equipment to the slot equal to their sort_index, this will break the
  /// equipment references in current equipment and the loadouts.
  /// Should be preceded by a call to 'void DefragEquipmentReferences(mhw_save_slot* save_slot)'
  /// to fix adjust the references before-hand, else the game will very likely crash.
  /// 
  /// Assumes all the sort indices are valid; 0 &lt;= sort index &lt; equipment count
  /// </summary>
  /// <param name="save_slot">The save slot to defrag references for.</param>
  static int DefragEquipment(mhw_save_slot* save_slot) {
    mhw_equipment* equipment = save_slot->equipment;
    u32* equipment_index_table = save_slot->equipment_index_table;

    int switchCount = 0;
    i32 count = COUNTOF(save_slot->equipment);
    for (int i = 0; i < count; ++i) {
      int index = equipment_index_table[i];
      mhw_equipment* src = equipment + index;
      mhw_equipment* dst = equipment + i;

      if (src != dst) {
        SwapEquipmentEntries(equipment_index_table, src, dst);
        ++switchCount;
      }
    }
    return switchCount;
  }

  static bool ValidateEquipmentBox(mhw_save_slot* save_slot, bool dry = false) {
    // Issue types
    enum SaveEquipmentIssue : u32 {
      EquipIssue_Fine = 0,
      EquipIssue_SortIndex = 1 << 0,
      EquipIssue_TableIndex = 1 << 1,
      EquipIssue_Both = EquipIssue_SortIndex | EquipIssue_TableIndex
    };
    u32 issues = EquipIssue_Fine;

    // Detection
    const i32 count = COUNTOF(save_slot->equipment);
    for (int i = 0; i < count; ++i) {
      int sort_index = save_slot->equipment[i].sort_index;
      int table_index = save_slot->equipment_index_table[i];

      if (sort_index < 0 || sort_index >= count || save_slot->equipment_index_table[sort_index] != i) {
        issues |= EquipIssue_SortIndex;
      }

      if (table_index < 0 || table_index >= count || save_slot->equipment[table_index].sort_index != i) {
        issues |= EquipIssue_TableIndex;
      }
      if (issues == EquipIssue_Both) break;
    }

    // Handling
    if (!dry) {
      switch (issues) {
      case EquipIssue_Both: {
        for (int i = 0; i < count; ++i) {
          save_slot->equipment_index_table[i] = i;
          save_slot->equipment[i].sort_index = i;
        }
        issues = EquipIssue_Fine;
      } break;

      case EquipIssue_SortIndex: {
        for (int i = 0; i < count; ++i)
          save_slot->equipment[i].sort_index = save_slot->equipment_index_table[i];
        issues = EquipIssue_Fine;
      } break;

      case EquipIssue_TableIndex: {
        for (int i = 0; i < count; ++i)
          save_slot->equipment_index_table[i] = save_slot->equipment[i].sort_index;
        issues = EquipIssue_Fine;
      } break;

      case EquipIssue_Fine: break;
      default: Q_ASSERT(false); break; // All issues should be handled somehow
      }
    }

    return issues == EquipIssue_Fine;
  }

  static bool ValidateItemSlots(mhw_item_slot* items, int count) {
    for (int i = 0; i < count; i++) {
      if (items[i].amount == 0) items[i].id = 0;
      if (items[i].id == 0) items[i].amount = 0;
    }
    return true;
  }

  static void ValidateSaveFile(mhw_ib_save* save) {
    for (int slot = 0; slot < COUNTOF(save->section3.saves); slot++)
    {
      mhw_save_slot* save_slot = &save->section3.saves[slot];
      mhw_item_pouch* pouch = &save_slot->item_pouch;
      mhw_storage* storage = &save_slot->storage;

      ValidateItemSlots(pouch->ammo, COUNTOF(pouch->ammo));
      ValidateItemSlots(pouch->items, COUNTOF(pouch->items));
      ValidateItemSlots(storage->ammo, COUNTOF(storage->ammo));
      ValidateItemSlots(storage->decorations, COUNTOF(storage->decorations));
      ValidateItemSlots(storage->items, COUNTOF(storage->items));
      ValidateItemSlots(storage->materials, COUNTOF(storage->materials));
      ValidateEquipmentBox(save_slot);
    }
  }
};
