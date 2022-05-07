#pragma once

#include "mhw_save.h"
#include "file/itm.h"

struct inventory_area {
  int localoffset;
  int count;
  itemCategory type;
  bool storage;
};

const inventory_area inventory_areas[] = {
  {offsetof(mhw_save_slot, item_pouch) + offsetof(mhw_item_pouch, items      ), COUNTOF(mhw_item_pouch::items   ), itemCategory::Item,       false },
  {offsetof(mhw_save_slot, item_pouch) + offsetof(mhw_item_pouch, ammo       ), COUNTOF(mhw_item_pouch::ammo    ), itemCategory::Ammo,       false },
  {offsetof(mhw_save_slot, storage   ) + offsetof(mhw_storage,    items      ), COUNTOF(mhw_storage::items      ), itemCategory::Item,       true  },
  {offsetof(mhw_save_slot, storage   ) + offsetof(mhw_storage,    ammo       ), COUNTOF(mhw_storage::ammo       ), itemCategory::Ammo,       true  },
  {offsetof(mhw_save_slot, storage   ) + offsetof(mhw_storage,    materials  ), COUNTOF(mhw_storage::materials  ), itemCategory::Material,   true  },
  {offsetof(mhw_save_slot, storage   ) + offsetof(mhw_storage,    decorations), COUNTOF(mhw_storage::decorations), itemCategory::Decoration, true  }
};
