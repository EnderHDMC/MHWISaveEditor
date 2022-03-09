#pragma once
#include <stdlib.h>

#include "../types.h"

enum class itemFlag : u32 {
  PermanentItem = 1 << 0,
  SupplyItem = 1 << 1,
  ItemBar = 1 << 3,
  Feystone = 1 << 4,
  LoadoutItem = 1 << 5,
  PalicoGadget = 1 << 7,
  AmmoLvl1 = 1 << 8,
  AmmoLvl2 = 1 << 9,
  AmmoLvl3 = 1 << 10,
  GlowEffect = 1 << 11,

  UnknownA = 1 << 2,
  UnknownB = 1 << 6,
  UnknownC = 1 << 13,
  Unknown = UnknownA | UnknownB | UnknownC,

  Unused = 0xFFFFFFFF
  & ~(PermanentItem | SupplyItem | ItemBar
    | Feystone | LoadoutItem | PalicoGadget
    | AmmoLvl1 | AmmoLvl2 | AmmoLvl3 | GlowEffect
    | Unknown),

  CustomTripleQ = 1 << 25,
  CustomHARDUMMY = 1 << 26,
  CustomUnavailable = 1 << 27,
  CustomAppraisal = 1 << 28,

  CustomSlingerAmmo = 1 << 29,
  CustomObtainable = 1 << 30,
  CustomDiscoverable = ((u32)1) << 31,

  CustomExclude = 0
  | CustomTripleQ
  | CustomHARDUMMY
  | CustomUnavailable
  | CustomAppraisal,

  RejectFlag = 0
  | PermanentItem
  | SupplyItem
  | LoadoutItem
  | Feystone
  | CustomExclude,
};

enum class itemCategory : u32
{
  Item = 0,
  Material = 1,
  Account = 2,
  Ammo = 3,
  Decoration = 4,
  Furniture = 5
};

#pragma pack(push, 1)
typedef struct {
  u32 id;
  u8 subtype;
  u32 type;
  u8 rarity;
  u8 carry_limit;
  u8 carry_limit_twisted;
  u16 sort_order;
  u32 flags;
  u32 icon_id;
  u16 icon_color;
  u32 sell_price;
  u32 buy_price;
} itm_entry;

typedef struct {
  u32 magic1;
  u16 magic2;
  u32 entry_count;
} itm_header;

typedef struct {
  itm_header* header;
  itm_entry* items;
} itm_meta;
#pragma pack(pop)

static bool InitMeta_itm(itm_meta* meta, itm_header* header) {
  u8* base = (u8*)header;
  if (!base) return false;

  meta->header = (itm_header*)base;
  meta->items = (itm_entry*)(base + sizeof(itm_header));
  return true;
}

static void FreeMeta_itm(itm_meta* meta) {
  if (meta->header) free(meta->header);
  meta->header = nullptr;
  meta->items = nullptr;
}
