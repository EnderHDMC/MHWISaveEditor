#pragma once

#include "ItemDB.h"
#include "EquipmentDB.h"

#include "../utility/read_bin_file.h"

class SmithyDB
{
private:
  static SmithyDB* instance;
  SmithyDB();

  eq_crt_meta eq_crt_armor = {};
  eq_crt_meta eq_crt_pendants = {};
  eq_crt_meta eq_crt_ot_equip = {};
  eq_crt_meta eq_crt_weapon = {};

  eq_cus_meta eq_cus_charms = {};
  eq_cus_meta eq_cus_insect = {};
  eq_cus_meta eq_cus_insect_element = {};
  eq_cus_meta eq_cus_weapon = {};

  mhw_item_slot* AddMaterial(QList<mhw_item_slot>& mats, mhw_item_slot mat);

public:
  static SmithyDB* GetInstance();
  void Free();

  eq_crt_entry* GetForgeEntry(eq_crt_meta* eq_crt, i32 type, u16 raw_index);
  eq_cus_entry* GetUpgradeEntry(eq_cus_meta* eq_cus, i32 type, u16 raw_index);
  eq_cus_entry* GetUpgradeParent(eq_cus_meta* upgrades, eq_cus_entry* child);

  QList<mhw_item_slot> GetLineCraftingMats(equipment_info* info, mhw_equipment* equipment);
};
