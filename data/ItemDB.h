#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include <QString>

#include "../types/mhw_save.h"

using json = nlohmann::json;

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

struct itemInfo {
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
  std::string name;
  std::string description;

  itemInfo() {
    id = 0;
    subtype = 0;
    type = 0;
    rarity = 0;
    carry_limit = 0;
    carry_limit_twisted = 0;
    sort_order = 0;
    flags = 0;
    icon_id = 0;
    icon_color = 0;
    sell_price = 0;
    buy_price = 0;
  }
};

static bool operator==(const itemInfo& lhs, const itemInfo& rhs) {
  return lhs.id == rhs.id;
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(itemInfo,
  id, subtype, type, rarity, carry_limit, carry_limit_twisted,
  sort_order, flags, icon_id, icon_color, sell_price, buy_price,
  name, description);

class ItemDB
{
  static ItemDB* instance;
  std::vector<itemInfo> items;

  bool loadError = false;
  bool fatalError = false;

  ItemDB();

public:
  static ItemDB* GetInstance();
  void Free();

  itemInfo* GetItemById(u32 id);
  itemInfo* GetItemByIndex(int index);
  int count();

  QString ItemName(itemInfo* info);

  // These two need special treatment.
  // Survival jewel [2270] is unobtainable.
  // Smoke jewel is survival jewel but is obtainable.
  // But smoke jewel uses survival jewels name, so needs to be adjusted.
  // Their names basically just need to be swapped.
  static const u32 SurvivalJewelID = 2270;
  static const u32 SmokeJewelID = 819;
};
