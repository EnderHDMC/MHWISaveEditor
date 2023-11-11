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

  static inline u32 ExpToRank(u32 xp, const u32 exp_table[1000]) {
    u32 rank = -1;

    for (int i = 0; i < 1000; i++) {
      u32 exp = exp_table[i];
      if (xp >= exp) {
        rank = i;
      }
      else break;
    }

    return rank;
  }

  static inline u32 RankToExp(u32 rank, const u32 exp_table[1000]) {
    Q_ASSERT(rank < 1000);
    return exp_table[rank];
  }

  static inline bool RankIsLock(u32 rank, const u32 exp_table[1000]) {
    Q_ASSERT(rank < 1000);
    u32 next = rank + 1;
    if (next > 999) next = 999;

    u32 exp = exp_table[rank];
    u32 exp_next = exp_table[next];
    return exp == exp_next || exp == 0;
  }

  static inline u32 RankNextLock(u32 rank, const u32 exp_table[1000]) {
    u32 next = -1;
    for (int check = rank; check < 1000; check++) {
      if (RankIsLock(check, exp_table)) {
        next = check;
        break;
      }
    }
    return next;
  }

  static inline u32 RankPreviousMilestone(u32 rank, const u32 exp_table[1000]) {
    u32 milestone = 0;
    for (int check = rank - 1; check > -1; check--) {
      if (RankIsLock(check, exp_table)) {
        milestone = check + 1;
        break;
      }
    }
    return milestone;
  }

  static inline u32 ExpToRankWithHint(u32 xp, u32 hint, const u32 exp_table[1000]) {
    bool hintLock = RankIsLock(hint, exp_table);
    Q_ASSERT(hintLock);

    u32 guess = ExpToRank(xp, exp_table);
    if (guess != hint && hintLock) {
      guess = hint;
    }

    return guess;
  }

  static inline u32 HRExpToRank(u32 xp) { return ExpToRank(xp, hr_exp_table); }
  static inline u32 HRRankToEXP(u32 rank) { return RankToExp(rank, hr_exp_table); }
  static inline bool HRIsLock(u32 level) { return RankIsLock(level, hr_exp_table); }
  static inline u32 HRNextLock(u32 level) { return RankNextLock(level, hr_exp_table); }
  static inline u32 HRPreviousMilestone(u32 level) { return RankPreviousMilestone(level, hr_exp_table); }
  static inline u32 HRExpToRankHint(u32 xp, u32 hint) { return ExpToRankWithHint(xp, hint, hr_exp_table); }

  static inline u32 MRExpToRank(u32 xp) { return ExpToRank(xp, mr_exp_table); }
  static inline u32 MRRankToEXP(u32 rank) { return RankToExp(rank, mr_exp_table); }
  static inline bool MRIsLock(u32 level) { return RankIsLock(level, mr_exp_table); }
  static inline u32 MRNextLock(u32 level) { return RankNextLock(level, mr_exp_table); }
  static inline u32 MRPreviousMilestone(u32 level) { return RankPreviousMilestone(level, mr_exp_table); }
  static inline u32 MRExpToRankHint(u32 xp, u32 hint) { return ExpToRankWithHint(xp, hint, mr_exp_table); }

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

  static bool GiveEquipment(mhw_save_slot* save_slot, mhw_equipment* equipment) {
    mhw_equipment* slot = MHWSaveUtils::FindEquipment(save_slot, -1, 0);
    if (!slot) return false;

    equipment->sort_index = slot->sort_index;
    memcpy_s(slot, sizeof(mhw_equipment), equipment, sizeof(mhw_equipment));
    return true;
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

    if (ref->category == mhw_equip_category::Kinsect) {
      const int count = COUNTOF(save_slot->equipment);
      for (int i = 0; i < count; ++i) {
        bool isWeapon = equipment[i].category == mhw_equip_category::Weapon;

        // TODO: Probably want an enum for this
        bool isGlaive = isWeapon && equipment[i].type == 10;
        // TODO: This should be a union
        referenced += isGlaive && equipment[i].bowgun_mods[0] == srcIndex;
      }
    }

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
      if (ref->category == mhw_equip_category::Kinsect) {
        referenced += loadout->bowgun_mods[0] == srcIndex;
      }
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
#define SyncIndex(ptr, member, arr, index) if(ptr->member > -1) ptr->member = arr[ptr->member].index
    SyncIndex(current_equipment, weapon, equipment, sort_index);
    SyncIndex(current_equipment, helmet, equipment, sort_index);
    SyncIndex(current_equipment, torso, equipment, sort_index);
    SyncIndex(current_equipment, arms, equipment, sort_index);
    SyncIndex(current_equipment, coil, equipment, sort_index);
    SyncIndex(current_equipment, feet, equipment, sort_index);
    SyncIndex(current_equipment, charm, equipment, sort_index);
    SyncIndex(current_equipment, kinsect, equipment, sort_index);

    // Update indices of loadout gear
    const int count = COUNTOF(save_slot->equipment_loadouts);
    for (int i = 0; i < count; ++i) {
      mhw_equipment_loadout* loadout = save_slot->equipment_loadouts + i;
      bool hasWeapon = loadout->weapon_index > -1;
      bool isWeapon = hasWeapon && equipment[loadout->weapon_index].category == mhw_equip_category::Weapon;

      bool isGlaive = isWeapon && equipment[loadout->weapon_index].type == 10;
      SyncIndex(loadout, weapon_index, equipment, sort_index);
      SyncIndex(loadout, helmet_index, equipment, sort_index);
      SyncIndex(loadout, torso_index, equipment, sort_index);
      SyncIndex(loadout, arms_index, equipment, sort_index);
      SyncIndex(loadout, coil_index, equipment, sort_index);
      SyncIndex(loadout, feet_index, equipment, sort_index);
      SyncIndex(loadout, charm_index, equipment, sort_index);
      if (isGlaive) {
        SyncIndex(loadout, bowgun_mods[0], equipment, sort_index);
      }
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
