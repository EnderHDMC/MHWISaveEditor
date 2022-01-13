#pragma once

#include "mhw_save.h"
#include <cstddef>

enum class item_type
{
  Item = 0,
  Material = 1,

  Ammo = 3,
  Decoration = 4
};

struct inventory_area {
  int localoffset;
  int count;
  item_type type;
  bool storage;
  const char* area;
};

const inventory_area inventory_areas[] = {
  {offsetof(mhw_save_slot, item_pouch) + offsetof(mhw_item_pouch, items), COUNTOF(mhw_item_pouch::items), item_type::Item, false, "Item Pouch"},
  {offsetof(mhw_save_slot, item_pouch) + offsetof(mhw_item_pouch, ammo), COUNTOF(mhw_item_pouch::ammo), item_type::Ammo, false , "Ammo Pouch"},
  {offsetof(mhw_save_slot, storage) + offsetof(mhw_storage, items), COUNTOF(mhw_storage::items), item_type::Item, true, "Item Box" },
  {offsetof(mhw_save_slot, storage) + offsetof(mhw_storage, ammo), COUNTOF(mhw_storage::ammo), item_type::Ammo, true, "Ammo Box"},
  {offsetof(mhw_save_slot, storage) + offsetof(mhw_storage, materials), COUNTOF(mhw_storage::materials), item_type::Material, true, "Material Box"},
  {offsetof(mhw_save_slot, storage) + offsetof(mhw_storage, decorations), COUNTOF(mhw_storage::decorations), item_type::Decoration, true, "Deco Box"}
};