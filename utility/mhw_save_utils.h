#pragma once

#include "../types/mhw_save.h"
#include "../types/mhw_struct_constants.h"
#include "../types/file/itm.h"
#include "settype.h"

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

  if (!items) return nullptr;

  mhw_item_slot* result = FindItemOrEmpty(items, count, info->id);
  return result;
}

static bool GiveItem(mhw_item_slot* item_slot, itm_entry* info) {
  bool replace = item_slot->id != info->id;
  item_slot->id = info->id;
  item_slot->amount++;
  if (item_slot->amount > 9999) item_slot->amount = 9999;

  return replace;
}

static void DiscoverItem(mhw_save_slot* save_slot, itm_entry* info) {
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

static void ValidateItemSlots(mhw_item_slot* items, int count) {
  for (int i = 0; i < count; i++) {
    if (items[i].amount == 0) {
      items[i].id = 0;
    }

    if (items[i].id == 0) {
      items[i].amount = 0;
    }
  }
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
  }
}
