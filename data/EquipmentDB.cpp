#include "EquipmentDB.h"

#include "../utility/common/Settings.h"
#include "../utility/read_bin_file.h"

EquipmentDB* EquipmentDB::instance = nullptr;

EquipmentDB::EquipmentDB()
{
  Settings* settings = settings->GetInstance();
  game_language itemLanguage = settings->GetItemLanguage();

  Read_am_dat(&am_dat, Paths::GetResourcesPath("game/armor.am_dat"));
  ReadLanguage_gmd(&gmd_armor, "game/armor_%1.gmd", itemLanguage);

  Read_rod_inse(&rod_inse, Paths::GetResourcesPath("game/rod_insect.rod_inse"));
  ReadLanguage_gmd(&gmd_kinsect, "game/rod_insect_%1.gmd", itemLanguage);

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

  QMapIterator<i32, QString> wp(mapBasenames);
  while (wp.hasNext()) {
    wp.next();
    i32 type = wp.key();

    QString basename = mapBasenames.value(type);
    wp_dat_meta* wp_dat = map_wp_dat.value(type);
    wp_dat_g_meta* wp_dat_g = map_wp_dat_g.value(type);
    gmd_meta* gmd = map_gmd.value(type);

    QString file_dat = QString("game/%1.wp_dat").arg(basename);
    QString file_dat_g = QString("game/%1.wp_dat_g").arg(basename);
    QString file_gmd = QString("game/%1_%2.gmd").arg(basename).arg("%1");

    if (wp_dat) Read_wp_dat(wp_dat, Paths::GetResourcesPath(file_dat));
    if (wp_dat_g) Read_wp_dat_g(wp_dat_g, Paths::GetResourcesPath(file_dat_g));
    ReadLanguage_gmd(gmd, file_gmd, itemLanguage);
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
  FreeMeta_am_dat(&am_dat);         FreeMeta_gmd(&gmd_armor);
  FreeMeta_rod_inse(&rod_inse);     FreeMeta_gmd(&gmd_kinsect);

  FreeMeta_wp_dat(&wp_dat_c_axe);   FreeMeta_gmd(&gmd_c_axe);
  FreeMeta_wp_dat(&wp_dat_g_lance); FreeMeta_gmd(&gmd_g_lance);
  FreeMeta_wp_dat(&wp_dat_hammer);  FreeMeta_gmd(&gmd_hammer);
  FreeMeta_wp_dat(&wp_dat_lance);   FreeMeta_gmd(&gmd_lance);
  FreeMeta_wp_dat(&wp_dat_l_sword); FreeMeta_gmd(&gmd_l_sword);
  FreeMeta_wp_dat(&wp_dat_rod);     FreeMeta_gmd(&gmd_rod);
  FreeMeta_wp_dat(&wp_dat_s_axe);   FreeMeta_gmd(&gmd_sword);
  FreeMeta_wp_dat(&wp_dat_sword);   FreeMeta_gmd(&gmd_s_axe);
  FreeMeta_wp_dat(&wp_dat_tachi);   FreeMeta_gmd(&gmd_tachi);
  FreeMeta_wp_dat(&wp_dat_whistle); FreeMeta_gmd(&gmd_whistle);
  FreeMeta_wp_dat(&wp_dat_w_sword); FreeMeta_gmd(&gmd_w_sword);

  FreeMeta_wp_dat_g(&wp_dat_g_bow); FreeMeta_gmd(&gmd_bow);
  FreeMeta_wp_dat_g(&wp_dat_g_hbg); FreeMeta_gmd(&gmd_hbg);
  FreeMeta_wp_dat_g(&wp_dat_g_lbg); FreeMeta_gmd(&gmd_lbg);

  delete instance;
  instance = nullptr;
}

equipment_info* EquipmentDB::GetEquipment(mhw_equipment* equipment)
{
  i32 category = equipment->serial_item_category;
  i32 type = equipment->type;
  u32 id = equipment->id;

  equipment_info* info = nullptr;

  switch (category) {
  case 0: info = (equipment_info*)GetEntryArmor(type, id); break;
  case 1: info = (equipment_info*)GetEntryWeapon(type, id); break;
  case 2: info = (equipment_info*)GetEntryArmor(type, id); break;
  case 4: info = (equipment_info*)GetEntryKinsect(type, id); break;
  }

  return info;
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
  if (entry)
  {
    u16 gmdIndex = entry->gmd_name_index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = "GMD FAILURE";
  else {
    name = name.replace("<ICON BETA>", " \u03B2");
    name = name.replace("<ICON ALPHA>", " \u03B1");
    name = name.replace("<ICON GAMMA>", " \u03B3");
  }

  return name;
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
  if (entry)
  {
    u16 gmdIndex = entry->gmd_name_index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = "GMD FAILURE";

  return name;
}

QString EquipmentDB::GetNameWeaponRanged(i32 type, i32 id)
{
  gmd_meta* gmd = map_gmd.value(type);
  wp_dat_g_entry* entry = GetEntryWeaponRanged(type, id);

  QString name;
  if (entry)
  {
    u16 gmdIndex = entry->gmd_name_index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = "GMD FAILURE";

  return name;
}

QString EquipmentDB::GetNameWeapon(i32 type, i32 id)
{
  wp_dat_meta* wp_dat = map_wp_dat.value(type);
  wp_dat_g_meta* wp_dat_g = map_wp_dat_g.value(type);
  QString name = "GMD FAILURE";

  if (wp_dat) name = GetNameWeaponMelee(type, id);
  if (wp_dat_g) name = GetNameWeaponRanged(type, id);
  return name;
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
  if (entry)
  {
    u16 gmdIndex = entry->index;
    name = QString::fromUtf8(gmd->value(gmdIndex));
  }
  if (name.isNull()) name = "GMD FAILURE";

  return name;
}

QString EquipmentDB::GetName(mhw_equipment* equipment)
{
  i32 category = equipment->serial_item_category;
  i32 type = equipment->type;
  u32 id = equipment->id;

  QString name;

  switch (category) {
  case 0: name = GetNameArmor(type, id); break;
  case 1: name = GetNameWeapon(type, id); break;
  case 2: name = GetNameArmor(type, id); break;
  case 4: name = GetNameKinsect(type, id); break;
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
