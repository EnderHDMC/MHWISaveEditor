#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "../types/types.h"
using json = nlohmann::json;

struct itemInfo {
  i32 id;
  i32 subType;
  i32 storageID;
  i32 rarity;
  i32 carryLimit;
  i32 unk;
  i32 sortOrder;
  i32 flags;
  i32 iconID;
  i32 iconColor;
  i32 carryItem;
  i32 sellPrice;
  i32 buyPrice;
  std::string name;
  std::string description;

  itemInfo() {
    id = 0;
    subType = 0;
    storageID = 0;
    rarity = 0;
    carryLimit = 0;
    unk = 0;
    sortOrder = 0;
    flags = 0;
    iconID = 0;
    iconColor = 0;
    carryItem = 0;
    sellPrice = 0;
    buyPrice = 0;
  }
};

static bool operator==(const itemInfo& lhs, const itemInfo& rhs) {
  return lhs.id == rhs.id; /*
      && lhs.subType == rhs.subType
      && lhs.storageID == rhs.storageID
      && lhs.rarity == rhs.rarity
      && lhs.carryLimit == rhs.carryLimit
      && lhs.unk == rhs.unk
      && lhs.sortOrder == rhs.sortOrder
      && lhs.flags == rhs.flags
      && lhs.iconID == rhs.iconID
      && lhs.iconColor == rhs.iconColor
      && lhs.carryItem == rhs.carryItem
      && lhs.sellPrice == rhs.sellPrice
      && lhs.buyPrice == rhs.buyPrice; */
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(itemInfo, id, subType, storageID, rarity, carryLimit, unk, sortOrder, flags, iconID, iconColor, carryItem, sellPrice, buyPrice, name, description)

class ItemDB
{
  static ItemDB* instance;
  std::vector<itemInfo> items;
  std::vector<std::string> names;

  ItemDB();

public:
  static ItemDB* GetInstance();
  void Free();

  itemInfo* GetItemById(u32 id);
  itemInfo* GetItemByIndex(int index);
  int count();

  std::vector<itemInfo> itemVector();
  std::vector<std::string> itemNamesVector();
};