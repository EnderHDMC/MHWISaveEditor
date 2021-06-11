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
  char* area;
};

const inventory_area areas[] = {
  {offsetof(mhw_save_slot, item_pouch) + offsetof(mhw_item_pouch, items), COUNTOF(mhw_item_pouch::items), item_type::Item, "Item Pouch"},
  {offsetof(mhw_save_slot, item_pouch) + offsetof(mhw_item_pouch, ammo), COUNTOF(mhw_item_pouch::ammo), item_type::Ammo, "Ammo Pouch"}
};