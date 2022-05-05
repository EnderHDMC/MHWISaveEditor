#include "SmithyDB.h"

#include "../utility/mhw_save_utils.h"

SmithyDB* SmithyDB::instance = nullptr;

SmithyDB::SmithyDB()
{
  ReadMetaFile(&eq_crt_armor, Paths::GetResourcesPath("chunk/common/equip/armor.eq_crt"));
  ReadMetaFile(&eq_crt_pendants, Paths::GetResourcesPath("chunk/common/equip/charm.eq_crt"));
  ReadMetaFile(&eq_crt_ot_equip, Paths::GetResourcesPath("chunk/common/equip/ot_equip.eq_crt"));
  ReadMetaFile(&eq_crt_weapon, Paths::GetResourcesPath("chunk/common/equip/weapon.eq_crt"));

  ReadMetaFile(&eq_cus_charms, Paths::GetResourcesPath("chunk/common/equip/equip_custom.eq_cus"));
  ReadMetaFile(&eq_cus_insect, Paths::GetResourcesPath("chunk/common/equip/insect.eq_cus"));
  ReadMetaFile(&eq_cus_insect_element, Paths::GetResourcesPath("chunk/common/equip/insect_element.eq_cus"));
  ReadMetaFile(&eq_cus_weapon, Paths::GetResourcesPath("chunk/common/equip/weapon.eq_cus"));
}

mhw_item_slot* SmithyDB::AddMaterial(QList<mhw_item_slot>& mats, mhw_item_slot mat)
{
  mhw_item_slot* material = nullptr;
  for (int i = 0; i < mats.count(); ++i) {
    if (mats[i].id == mat.id) {
      material = &mats[i];
      break;
    }
  }

  if (!material) { mats.append(mat); material = &mats[mats.count() - 1]; }
  else material->amount += mat.amount;
  return material;
}

SmithyDB* SmithyDB::GetInstance()
{
  if (!instance) instance = new SmithyDB();
  return instance;
}

void SmithyDB::Free()
{
  FreeMetaFile(&eq_crt_armor);
  FreeMetaFile(&eq_crt_pendants);
  FreeMetaFile(&eq_crt_ot_equip);
  FreeMetaFile(&eq_crt_weapon);

  FreeMetaFile(&eq_cus_charms);
  FreeMetaFile(&eq_cus_insect);
  FreeMetaFile(&eq_cus_insect_element);
  FreeMetaFile(&eq_cus_weapon);

  delete instance;
  instance = nullptr;
}

eq_crt_entry* SmithyDB::GetForgeEntry(eq_crt_meta* eq_crt, u8 type, u16 raw_index)
{
  eq_crt_entry* result = nullptr;

  if (eq_crt && eq_crt->header) {
    for (int i = 0; i < eq_crt->header->entry_count; ++i) {
      eq_crt_entry* entry = eq_crt->entries + i;

      if (entry->equipment_category_raw == type && entry->equipment_index_raw == raw_index) {
        result = entry;
        break;
      }
    }
  }

  return result;
}

eq_cus_entry* SmithyDB::GetUpgradeEntry(eq_cus_meta* eq_cus, u8 type, u16 raw_index)
{
  eq_cus_entry* result = nullptr;

  if (eq_cus && eq_cus->header) {
    for (int i = 0; i < eq_cus->header->entry_count; ++i) {
      eq_cus_entry* entry = eq_cus->entries + i;

      if (entry->equipment_category_raw == type && entry->equipment_index_raw == raw_index) {
        result = entry;
        break;
      }
    }
  }

  return result;
}

eq_cus_entry* SmithyDB::GetUpgradeParent(eq_cus_meta* upgrades, eq_cus_entry* child)
{
  eq_cus_entry* result = nullptr;
  if (child && upgrades && upgrades->header) {
    eq_cus_entry* base = upgrades->entries;
    const size_t index = child - base;

    if (index > -1 && index < upgrades->header->entry_count) {
      for (int i = index - 1; i >= 0; --i) {
        eq_cus_entry* entry = base + i;
        int temp = entry - base;

        if (entry->children[0] == index) { result = entry; break; }
        if (entry->children[1] == index) { result = entry; break; }
        if (entry->children[2] == index) { result = entry; break; }
        if (entry->children[3] == index) { result = entry; break; }
      }
    }
  }

  return result;
}

QList<mhw_item_slot> SmithyDB::GetLineCraftingMats(equipment_info* info, mhw_equipment *equipment)
{
  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  i32 raw_index = equipmentDB->GetRawIndex(info);

  QList<mhw_item_slot> result;
  if (raw_index < 0) return result;

  eq_crt_meta* eq_crt = nullptr;
  eq_cus_meta* eq_cus = nullptr;
  eq_cus_meta* eq_cus_extra = nullptr;

  switch (equipment->category) {
  case mhw_equip_category::Armor: eq_crt = &eq_crt_armor; eq_cus = nullptr; eq_cus_extra = nullptr; break;
  case mhw_equip_category::Weapon: eq_crt = &eq_crt_weapon; eq_cus = &eq_cus_weapon; eq_cus_extra = nullptr; break;
  case mhw_equip_category::Charm: eq_crt = &eq_crt_armor; eq_cus = &eq_cus_charms; eq_cus_extra = nullptr; break;
  case mhw_equip_category::Kinsect: eq_crt = nullptr; eq_cus = &eq_cus_insect; eq_cus_extra = &eq_cus_insect_element; break;
  }

  eq_crt_entry* forge_recipe = GetForgeEntry(eq_crt, equipment->type, raw_index);
  if (forge_recipe) {
    if (forge_recipe->mats[0].id) AddMaterial(result, { forge_recipe->mats[0].id, forge_recipe->mats[0].count });
    if (forge_recipe->mats[1].id) AddMaterial(result, { forge_recipe->mats[1].id, forge_recipe->mats[1].count });
    if (forge_recipe->mats[2].id) AddMaterial(result, { forge_recipe->mats[2].id, forge_recipe->mats[2].count });
    if (forge_recipe->mats[3].id) AddMaterial(result, { forge_recipe->mats[3].id, forge_recipe->mats[3].count });
  }

  eq_cus_entry* upgrade_recipe = GetUpgradeEntry(eq_cus, equipment->type, raw_index);
  if (upgrade_recipe) {
    eq_cus_entry* upgrade_parent = upgrade_recipe;
    eq_cus_entry* upgrade_prev = nullptr;
    do {
      if (upgrade_parent->mats[0].id) AddMaterial(result, { upgrade_parent->mats[0].id, upgrade_parent->mats[0].count });
      if (upgrade_parent->mats[1].id) AddMaterial(result, { upgrade_parent->mats[1].id, upgrade_parent->mats[1].count });
      if (upgrade_parent->mats[2].id) AddMaterial(result, { upgrade_parent->mats[2].id, upgrade_parent->mats[2].count });
      if (upgrade_parent->mats[3].id) AddMaterial(result, { upgrade_parent->mats[3].id, upgrade_parent->mats[3].count });
      upgrade_prev = upgrade_parent;
    } while (upgrade_parent = GetUpgradeParent(eq_cus, upgrade_parent));

    // We assume that the base craft has no upgrade recipe.
    if (upgrade_prev) {
      eq_crt_entry* forge_base = GetForgeEntry(eq_crt, upgrade_prev->equipment_category_raw, upgrade_prev->equipment_index_raw);
      if (forge_base) {
        if (upgrade_prev->mats[0].id) AddMaterial(result, { upgrade_prev->mats[0].id, upgrade_prev->mats[0].count });
        if (upgrade_prev->mats[1].id) AddMaterial(result, { upgrade_prev->mats[1].id, upgrade_prev->mats[1].count });
        if (upgrade_prev->mats[2].id) AddMaterial(result, { upgrade_prev->mats[2].id, upgrade_prev->mats[2].count });
        if (upgrade_prev->mats[3].id) AddMaterial(result, { upgrade_prev->mats[3].id, upgrade_prev->mats[3].count });
      }
    }
  }

  // TODO: Refund?
  //       Upgrades?
  //       Kinsect element?
  //       Augments?

  return result;
}
