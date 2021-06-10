#pragma once

#include "mhwSave.h"
#include <cstddef>

enum item_type
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
  {offsetof(MHWSaveSlot, ItemPouch) + offsetof(MHWItemPouch, items), COUNTOF(MHWItemPouch::items), item_type::Item, "Item Pouch"}
};