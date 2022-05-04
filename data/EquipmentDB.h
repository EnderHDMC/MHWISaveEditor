#pragma once

#include <QString>
#include <QMap>

#include "../types/editor_enums.h"
#include "../types/mhw_save.h"

#include "../utility/read_bin_file.h"

union equipment_info {
  am_dat_entry am_dat;
  wp_dat_entry wp_dat;
  wp_dat_g_entry wp_dat_g;
  rod_inse_entry rod_inse;
};

// Required by EquipmentDB::GetRawIndex, we assume the index field is in the same position in all the members of equipment_info
static_assert(offsetof(am_dat_entry, index) == offsetof(wp_dat_entry, index)
            && offsetof(wp_dat_entry, index) == offsetof(wp_dat_g_entry, index)
            && offsetof(wp_dat_g_entry, index) == offsetof(rod_inse_entry, index));

class EquipmentDB
{
private:
  static EquipmentDB* instance;
  EquipmentDB();

  bool hasData = false;
  am_dat_meta am_dat = {};         gmd_meta gmd_armor = {};
  rod_inse_meta rod_inse = {};     gmd_meta gmd_kinsect = {};

  wp_dat_meta wp_dat_c_axe = {};   gmd_meta gmd_c_axe = {};
  wp_dat_meta wp_dat_g_lance = {}; gmd_meta gmd_g_lance = {};
  wp_dat_meta wp_dat_hammer = {};  gmd_meta gmd_hammer = {};
  wp_dat_meta wp_dat_lance = {};   gmd_meta gmd_lance = {};
  wp_dat_meta wp_dat_l_sword = {}; gmd_meta gmd_l_sword = {};
  wp_dat_meta wp_dat_rod = {};     gmd_meta gmd_rod = {};
  wp_dat_meta wp_dat_s_axe = {};   gmd_meta gmd_sword = {};
  wp_dat_meta wp_dat_sword = {};   gmd_meta gmd_s_axe = {};
  wp_dat_meta wp_dat_tachi = {};   gmd_meta gmd_tachi = {};
  wp_dat_meta wp_dat_whistle = {}; gmd_meta gmd_whistle = {};
  wp_dat_meta wp_dat_w_sword = {}; gmd_meta gmd_w_sword = {};

  wp_dat_g_meta wp_dat_g_bow = {}; gmd_meta gmd_bow = {};
  wp_dat_g_meta wp_dat_g_hbg = {}; gmd_meta gmd_hbg = {};
  wp_dat_g_meta wp_dat_g_lbg = {}; gmd_meta gmd_lbg = {};

  QMap<i32, wp_dat_meta*> map_wp_dat;
  QMap<i32, wp_dat_g_meta*> map_wp_dat_g;

  QMap<i32, QString> mapBasenames;
  QMap<i32, gmd_meta*> map_gmd;

  void BindMapping(i32 type, wp_dat_meta* dat, gmd_meta* gmd, const QString& filename);
  void BindMapping(i32 type, wp_dat_g_meta* dat, gmd_meta* gmd, const QString& filename);

public:
  static EquipmentDB* GetInstance();
  void Free();

  equipment_info* GetEquipment(const mhw_equipment* equipment);
  i32 GetRawIndex(const mhw_equipment* equipment);

  // Database management interface
  void LoadGMD(game_language language);
  game_language CurrentLanguage();
  bool HasData();

  // Armor
  am_dat_entry* GetEntryArmor(i32 type, i32 id);
  QString GetNameArmor(i32 type, i32 id);

  int CountArmor();
  am_dat_entry* IndexArmor(i32 index);

  // Weapons
  wp_dat_entry* GetEntryWeaponMelee(i32 type, i32 id);
  wp_dat_g_entry* GetEntryWeaponRanged(i32 type, i32 id);
  equipment_info* GetEntryWeapon(i32 type, i32 id);

  QString GetNameWeaponMelee(i32 type, i32 id);
  QString GetNameWeaponRanged(i32 type, i32 id);
  QString GetNameWeapon(i32 type, i32 id);

  int CountWeaponMelee(i32 type);
  int CountWeaponRanged(i32 type);
  wp_dat_entry* IndexWeaponMelee(i32 type, i32 index);
  wp_dat_g_entry* IndexWeaponRanged(i32 type, i32 index);

  // Kinsects
  rod_inse_entry* GetEntryKinsect(i32 type, i32 id);
  QString GetNameKinsect(i32 type, i32 id);

  int CountKinsect();
  rod_inse_entry* IndexKinsect(i32 index);

  // All
  QString GetName(const mhw_equipment* equipment);

  // Debug
  void DumpWeaponInfo();
  void DumpArmorInfo();
  void DumpKinsectInfo();
};
