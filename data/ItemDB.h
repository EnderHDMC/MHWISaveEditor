#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#include <QString>

#include "../types/mhw_save.h"
#include "../types/file/itm.h"
#include "../types/file/gmd.h"
#include "../types/editor_enums.h"

class ItemDB
{
private:
  itm_meta itm = {};
  gmd_meta gmd = {};

  bool success_itm = false;
  bool success_gmd = false;
  bool success_flags = false;

  bool ReadCustomFlags(itm_meta* itm);

public:
  ItemDB();
  ~ItemDB();

  itm_entry* GetItemByIdSafe(u32 id);
  itm_entry* GetItemById(u32 id);
  int count();

  void LoadGMD(game_language language);
  game_language CurrentLanguage();

  QString ItemName(u32 id);
  QString ItemName(itm_entry* info);

  //////////////////////////////////////////////
  //////// Items with special treatment ////////
  //////////////////////////////////////////////

  // Survival jewel [2270] is unobtainable.
  // Smoke jewel is survival jewel but is obtainable.
  // But smoke jewel uses survival jewels name, so needs to be adjusted.
  // Their names basically just need to be swapped, did you enjoy this rant?
  static const u32 SurvivalJewelID = 2270;
  static const u32 SmokeJewelID = 819;

  // Mega Dash Juice is a material, but needs to be an item.
  // Exhaust Coating is a material, but needs to be an ammo.
  static const u32 MegaDashJuiceID = 19;
  static const u32 ExhaustCoatingID = 179;
};
