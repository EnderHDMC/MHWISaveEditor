#include "EquipmentDB.h"

#include "../utility/common/Settings.h"
#include "../utility/read_bin_file.h"

EquipmentDB* EquipmentDB::instance = nullptr;

EquipmentDB::EquipmentDB()
{
  Settings* settings = settings->GetInstance();
  game_language itemLanguage = settings->GetItemLanguage();

  hasData |= ReadMetaFile(&am_dat, Paths::GetResourcesPath("chunk/common/equip/armor.am_dat"));
  hasData |= ReadMetaFile(&rod_inse, Paths::GetResourcesPath("chunk/common/equip/rod_insect.rod_inse"));

  BindMapping(0, &wp_dat_l_sword, &gmd_l_sword, "l_sword"); // Great Sword
  BindMapping(1, &wp_dat_sword, &gmd_sword, "sword");       // Sword And Shield
  BindMapping(2, &wp_dat_w_sword, &gmd_w_sword, "w_sword"); // Dual Blades
  BindMapping(3, &wp_dat_tachi, &gmd_tachi, "tachi");       // Longsword
  BindMapping(4, &wp_dat_hammer, &gmd_hammer, "hammer");    // Hammer
  BindMapping(5, &wp_dat_whistle, &gmd_whistle, "whistle"); // Hunting Horn
  BindMapping(6, &wp_dat_lance, &gmd_lance, "lance");       // Lance
  BindMapping(7, &wp_dat_g_lance, &gmd_g_lance, "g_lance"); // Gunlance
  BindMapping(8, &wp_dat_s_axe, &gmd_s_axe, "s_axe");       // Switch Axe
  BindMapping(9, &wp_dat_c_axe, &gmd_c_axe, "c_axe");       // Charge Blade
  BindMapping(10, &wp_dat_rod, &gmd_rod, "rod");            // Insect Glaive
  BindMapping(11, &wp_dat_g_bow, &gmd_bow, "bow");          // Bow
  BindMapping(12, &wp_dat_g_hbg, &gmd_hbg, "hbg");          // Heavy Bowgun
  BindMapping(13, &wp_dat_g_lbg, &gmd_lbg, "lbg");          // Light Bowgun
  LoadGMD(itemLanguage);

  QMapIterator<i32, QString> wp(mapBasenames);
  while (wp.hasNext()) {
    wp.next();
    i32 type = wp.key();

    QString basename = mapBasenames.value(type);
    wp_dat_meta* wp_dat = map_wp_dat.value(type);
    wp_dat_g_meta* wp_dat_g = map_wp_dat_g.value(type);

    QString file_dat = QString("chunk/common/equip/%1.wp_dat").arg(basename);
    QString file_dat_g = QString("chunk/common/equip/%1.wp_dat_g").arg(basename);

    if (wp_dat) hasData |= ReadMetaFile(wp_dat, Paths::GetResourcesPath(file_dat));
    if (wp_dat_g) hasData |= ReadMetaFile(wp_dat_g, Paths::GetResourcesPath(file_dat_g));
  }
}

void EquipmentDB::BindMapping(i32 type, wp_dat_meta* dat, gmd_meta* gmd, const QString& filename)
{
  mapBasenames.insert(type, filename);
  map_wp_dat.insert(type, dat);
  map_gmd.insert(type, gmd);
}

void EquipmentDB::BindMapping(i32 type, wp_dat_g_meta* dat, gmd_meta* gmd, const QString& filename)
{
  mapBasenames.insert(type, filename);
  map_wp_dat_g.insert(type, dat);
  map_gmd.insert(type, gmd);
}

EquipmentDB* EquipmentDB::GetInstance()
{
  if (!instance) instance = new EquipmentDB();
  return instance;
}

void EquipmentDB::Free()
{
  FreeMetaFile(&am_dat);         FreeMetaFile(&gmd_armor);
  FreeMetaFile(&rod_inse);       FreeMetaFile(&gmd_kinsect);

  FreeMetaFile(&wp_dat_c_axe);   FreeMetaFile(&gmd_c_axe);
  FreeMetaFile(&wp_dat_g_lance); FreeMetaFile(&gmd_g_lance);
  FreeMetaFile(&wp_dat_hammer);  FreeMetaFile(&gmd_hammer);
  FreeMetaFile(&wp_dat_lance);   FreeMetaFile(&gmd_lance);
  FreeMetaFile(&wp_dat_l_sword); FreeMetaFile(&gmd_l_sword);
  FreeMetaFile(&wp_dat_rod);     FreeMetaFile(&gmd_rod);
  FreeMetaFile(&wp_dat_s_axe);   FreeMetaFile(&gmd_sword);
  FreeMetaFile(&wp_dat_sword);   FreeMetaFile(&gmd_s_axe);
  FreeMetaFile(&wp_dat_tachi);   FreeMetaFile(&gmd_tachi);
  FreeMetaFile(&wp_dat_whistle); FreeMetaFile(&gmd_whistle);
  FreeMetaFile(&wp_dat_w_sword); FreeMetaFile(&gmd_w_sword);

  FreeMetaFile(&wp_dat_g_bow);   FreeMetaFile(&gmd_bow);
  FreeMetaFile(&wp_dat_g_hbg);   FreeMetaFile(&gmd_hbg);
  FreeMetaFile(&wp_dat_g_lbg);   FreeMetaFile(&gmd_lbg);

  delete instance;
  instance = nullptr;
}

equipment_info* EquipmentDB::GetEquipment(const mhw_equipment* equipment)
{
  mhw_equip_category category = equipment->category;
  i32 type = equipment->type;
  u32 id = equipment->id;

  equipment_info* info = nullptr;

  switch (category) {
  case mhw_equip_category::Armor:
  case mhw_equip_category::Charm:
    info = (equipment_info*)GetEntryArmor(type, id); break;

  case mhw_equip_category::Weapon:
    info = (equipment_info*)GetEntryWeapon(type, id); break;

  case mhw_equip_category::Kinsect:
    info = (equipment_info*)GetEntryKinsect(type, id); break;
  }

  return info;
}

i32 EquipmentDB::GetRawIndex(const mhw_equipment* equipment)
{
  i32 index = -1;
  equipment_info* info = GetEquipment(equipment);

  if (info) index = info->am_dat.index;
  return index;
}

void EquipmentDB::LoadGMD(game_language language)
{
  ReadMetaFileLanguage(&gmd_armor, "chunk/common/text/steam/armor_%1.gmd", language);
  ReadMetaFileLanguage(&gmd_kinsect, "chunk/common/text/vfont/rod_insect_%1.gmd", language);

  QMapIterator<i32, QString> wp(mapBasenames);
  while (wp.hasNext()) {
    wp.next();
    i32 type = wp.key();

    QString basename = mapBasenames.value(type);
    gmd_meta* gmd = map_gmd.value(type);

    QString file_gmd = QString("chunk/common/text/steam/%1_%2.gmd").arg(basename).arg("%1");
    ReadMetaFileLanguage(gmd, file_gmd, language);
  }
}

game_language EquipmentDB::CurrentLanguage()
{
  if (gmd_armor.header) return (game_language)gmd_armor.header->language_id;
  else if (gmd_kinsect.header) return (game_language)gmd_kinsect.header->language_id;
  else {
    QMapIterator<i32, QString> wp(mapBasenames);
    while (wp.hasNext()) {
      wp.next();
      i32 type = wp.key();

      gmd_meta* gmd = map_gmd.value(type);
      if (gmd->header) return (game_language)gmd->header->language_id;
    }
  }
  return game_language::InvalidLanguage;
}

bool EquipmentDB::HasData()
{
  return hasData;
}

am_dat_entry* EquipmentDB::GetEntryArmor(i32 type, i32 id)
{
  am_dat_meta* armor = &am_dat;
  am_dat_entry* info = nullptr;

  if (armor && armor->header) {
    for (int i = 0; i < armor->header->entry_count; i++) {
      am_dat_entry* current = &armor->entries[i];
      if (current->equip_slot == type && current->set_group == id) {
        info = current;
        break;
      }
    }
  }

  return info;
}

QString EquipmentDB::GetNameArmor(i32 type, i32 id)
{
  gmd_meta* gmd = &gmd_armor;
  am_dat_entry* entry = GetEntryArmor(type, id);

  QString name;
  if (gmd->header && entry)
  {
    u16 gmdIndex = entry->gmd_name_index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = GMD_FAILURE.c_str();
  else {
    name = name.replace("<ICON BETA>", " \u03B2");
    name = name.replace("<ICON ALPHA>", " \u03B1");
    name = name.replace("<ICON GAMMA>", " \u03B3");
  }

  return name;
}

int EquipmentDB::CountArmor()
{
  return am_dat.header ? am_dat.header->entry_count : 0;
}

am_dat_entry* EquipmentDB::IndexArmor(i32 index)
{
  return &am_dat.entries[index];
}

wp_dat_entry* EquipmentDB::GetEntryWeaponMelee(i32 type, i32 id)
{
  wp_dat_meta* weapon_data = map_wp_dat.value(type, nullptr);
  wp_dat_entry* info = nullptr;

  if (weapon_data && weapon_data->header) {
    for (int i = 0; i < weapon_data->header->entry_count; i++) {
      wp_dat_entry* current = &weapon_data->entries[i];
      if (current->id == id) {
        info = current;
        break;
      }
    }
  }

  return info;
}

wp_dat_g_entry* EquipmentDB::GetEntryWeaponRanged(i32 type, i32 id)
{
  wp_dat_g_meta* weapon_data = map_wp_dat_g.value(type, nullptr);
  wp_dat_g_entry* info = nullptr;

  if (weapon_data && weapon_data->header) {
    for (int i = 0; i < weapon_data->header->entry_count; i++) {
      wp_dat_g_entry* current = &weapon_data->entries[i];
      if (current->id == id) {
        info = current;
        break;
      }
    }
  }

  return info;
}

equipment_info* EquipmentDB::GetEntryWeapon(i32 type, i32 id)
{
  wp_dat_meta* weapon_dat = map_wp_dat.value(type, nullptr);
  wp_dat_g_meta* weapon_dat_g = map_wp_dat_g.value(type, nullptr);
  equipment_info* info = nullptr;

  Q_ASSERT(!(weapon_dat && weapon_dat_g));
  if (weapon_dat) info = (equipment_info*)GetEntryWeaponMelee(type, id);
  if (weapon_dat_g) info = (equipment_info*)GetEntryWeaponRanged(type, id);

  return info;
}

QString EquipmentDB::GetNameWeaponMelee(i32 type, i32 id)
{
  gmd_meta* gmd = map_gmd.value(type);
  wp_dat_entry* entry = GetEntryWeaponMelee(type, id);

  QString name;
  if (gmd->header && entry)
  {
    u16 gmdIndex = entry->gmd_name_index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = GMD_FAILURE.c_str();

  return name;
}

QString EquipmentDB::GetNameWeaponRanged(i32 type, i32 id)
{
  gmd_meta* gmd = map_gmd.value(type);
  wp_dat_g_entry* entry = GetEntryWeaponRanged(type, id);

  QString name;
  if (gmd->header && entry)
  {
    u16 gmdIndex = entry->gmd_name_index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = GMD_FAILURE.c_str();

  return name;
}

QString EquipmentDB::GetNameWeapon(i32 type, i32 id)
{
  wp_dat_meta* wp_dat = map_wp_dat.value(type);
  wp_dat_g_meta* wp_dat_g = map_wp_dat_g.value(type);
  QString name = GMD_FAILURE.c_str();

  if (wp_dat) name = GetNameWeaponMelee(type, id);
  if (wp_dat_g) name = GetNameWeaponRanged(type, id);
  return name;
}

int EquipmentDB::CountWeaponMelee(i32 type)
{
  int count = 0;

  wp_dat_meta* wp_dat = map_wp_dat.value(type);
  if (wp_dat && wp_dat->header) count = wp_dat->header->entry_count;
  return count;
}

int EquipmentDB::CountWeaponRanged(i32 type)
{
  int count = 0;

  wp_dat_g_meta* wp_dat_g = map_wp_dat_g.value(type);
  if (wp_dat_g && wp_dat_g->header) count = wp_dat_g->header->entry_count;
  return count;
}

wp_dat_entry* EquipmentDB::IndexWeaponMelee(i32 type, i32 index)
{
  wp_dat_entry* entry = nullptr;

  wp_dat_meta* wp_dat = map_wp_dat.value(type);
  if (wp_dat && wp_dat->header) entry = &wp_dat->entries[index];
  return entry;
}

wp_dat_g_entry* EquipmentDB::IndexWeaponRanged(i32 type, i32 index)
{
  wp_dat_g_entry* entry = nullptr;

  wp_dat_g_meta* wp_dat_g = map_wp_dat_g.value(type);
  if (wp_dat_g && wp_dat_g->header) entry = &wp_dat_g->entries[index];
  return entry;
}

rod_inse_entry* EquipmentDB::GetEntryKinsect(i32 type, i32 id)
{
  rod_inse_meta* kinsect_data = &rod_inse;
  rod_inse_entry* info = nullptr;

  if (kinsect_data && kinsect_data->header) {
    for (int i = 0; i < kinsect_data->header->entry_count; i++) {
      rod_inse_entry* current = &kinsect_data->entries[i];
      if (current->equip_id == id) {
        info = current;
        break;
      }
    }
  }

  return info;
}

QString EquipmentDB::GetNameKinsect(i32 type, i32 id)
{
  gmd_meta* gmd = &gmd_kinsect;
  rod_inse_entry* entry = GetEntryKinsect(type, id);

  QString name;
  if (gmd->header && entry)
  {
    u16 gmdIndex = entry->index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = GMD_FAILURE.c_str();

  return name;
}

int EquipmentDB::CountKinsect()
{
  return rod_inse.header ? rod_inse.header->entry_count : 0;
}

rod_inse_entry* EquipmentDB::IndexKinsect(i32 index)
{
  return &rod_inse.entries[index];
}

QString EquipmentDB::GetName(const mhw_equipment* equipment)
{
  mhw_equip_category category = equipment->category;
  i32 type = equipment->type;
  u32 id = equipment->id;

  QString name;

  switch (category) {
  case mhw_equip_category::Armor:
  case mhw_equip_category::Charm:
    name = GetNameArmor(type, id); break;

  case mhw_equip_category::Weapon:
    name = GetNameWeapon(type, id); break;

  case mhw_equip_category::Kinsect:
    name = GetNameKinsect(type, id); break;
  }

  return name;
}

void EquipmentDB::DumpWeaponInfo()
{
  qInfo().noquote().nospace() << "category" << ';' << "type" << ';' << "id" << ';' << "name";
  i32 category = 1;

  QMapIterator<i32, QString> wp(mapBasenames);
  while (wp.hasNext()) {
    wp.next();

    i32 type = wp.key();

    wp_dat_meta* wp_dat = map_wp_dat.value(type);
    wp_dat_g_meta* wp_dat_g = map_wp_dat_g.value(type);
    gmd_meta* gmd = map_gmd.value(type);

    if (wp_dat) {
      for (int i = 0; i < wp_dat->header->entry_count; i++) {
        wp_dat_entry* current = &wp_dat->entries[i];
        i32 id = current->id;
        QString name = GetNameWeaponMelee(type, id);

        qInfo().noquote().nospace() << category << ';' << type << ';' << id << ';' << name;
      }
    }

    if (wp_dat_g) {
      for (int i = 0; i < wp_dat_g->header->entry_count; i++) {
        wp_dat_g_entry* current = &wp_dat_g->entries[i];
        i32 id = current->id;
        QString name = GetNameWeaponRanged(type, id);

        qInfo().noquote().nospace() << category << ';' << type << ';' << id << ';' << name;
      }
    }
  }
}

void EquipmentDB::DumpArmorInfo()
{
  qInfo().noquote().nospace() << "index" << ';' << "variant" << ';' << "type" << ';'
    << "equip_slot" << ';' << "gender" << ';' << "set_group " << ';' << "name";
  i32 category = 0;

  for (int i = 0; i < am_dat.header->entry_count; i++) {
    am_dat_entry* current = &am_dat.entries[i];
    u32 index = current->index;
    u8 variant = current->variant;
    u8 type = current->type;
    u8 equip_slot = current->equip_slot;
    u32 gender = current->gender;
    u16 set_group = current->set_group;
    u16 gmd_name_index = current->gmd_name_index;
    Q_ASSERT(i == index);

    QString name = QString::fromUtf8(gmd_armor.value(gmd_name_index));
    qInfo().noquote().nospace() << index << ';' << variant << ';' << type << ';'
      << equip_slot << ';' << gender << ';' << set_group << ';' << name;
  }
}

void EquipmentDB::DumpKinsectInfo()
{
  qInfo().noquote().nospace() << "index" << ';' << "id" << ';' << "equip_id" << ';' << "name";
  i32 category = 0;

  for (int i = 0; i < rod_inse.header->entry_count; i++) {
    rod_inse_entry* current = &rod_inse.entries[i];
    u32 index = current->index;
    u8 id = current->id;
    u16 equip_id = current->equip_id;
    u32 gmd_name_index = current->index;

    QString name = QString::fromUtf8(gmd_kinsect.value(gmd_name_index));
    qInfo().noquote().nospace() << index << ';' << id << ';' << equip_id << ';' << name;
  }
}
