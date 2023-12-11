#pragma once

#include <fstream>
#include <iostream>

#include <QString>

#include "../types/mhw_save.h"
#include "../types/file/itm.h"
#include "../types/file/gmd.h"
#include "../types/file/sgpa.h"
#include "../types/editor_enums.h"

class ItemDB
{
private:
  itm_meta itm = {};
  sgpa_meta sgpa = {};
  gmd_meta gmd = {};

  bool success_itm = false;
  bool success_gmd = false;
  bool success_sgpa = false;
  bool success_flags = false;

  bool ReadCustomFlags(itm_meta* itm);

public:
  ItemDB();
  ~ItemDB();

  itm_entry* GetItemByIdSafe(u32 id);
  itm_entry* GetItemById(u32 id);
  itm_entry* GetItemByDecoIndex(i32 index);
  int count();

  void LoadGMD(game_language language);
  game_language CurrentLanguage();

  QString ItemName(u32 id);
  QString ItemName(itm_entry* info);

  static u32 AdjustItemID(u32 id);
  itm_entry* AdjustItemPtr(itm_entry* entry);

  //////////////////////////////////////////////
  //////// Items with special treatment ////////
  //////////////////////////////////////////////

  // Survival jewel [2270] is unobtainable.
  // Smoke jewel is survival jewel but is obtainable.
  // But smoke jewel uses survival jewels name, so needs to be adjusted.
  // Their names basically just need to be swapped, did you enjoy this rant?
  static const u32 SurvivalJewelID = 2270;
  static const u32 SmokeJewelID = 819;

  // Igni Sign [956] and Hunter Runestone [957] have a similar story.
  // Except both Igni Sign (Hunter Runestone) counts as a loadout item.
  static const u32 IgniSignID = 956;
  static const u32 HunterRunestoneID = 957;

  // Mega Dash Juice is a material, but needs to be an item.
  // Exhaust Coating is a material, but needs to be an ammo.
  static const u32 MegaDashJuiceID = 19;
  static const u32 ExhaustCoatingID = 179;
};
