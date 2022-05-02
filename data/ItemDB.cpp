#include "ItemDB.h"

#include <QRegularExpression>
#include <QDebug>
#include <QFile>

#include "../utility/read_bin_file.h"
#include "../types/inventory_areas.h"
#include "../utility/common/Settings.h"

enum class FlagFileIndex : u32 {
  Discoverable = 0,
  Obtainable = 1,
  SlingerAmmo = 2,
  Appraisal = 3,
  Unavailable = 4,
  HARDUMMY = 5,
  TripleQ = 6, // ??? items

   // Assumes every other entry is seqential and starts at 0.
   FlagFileIndexCount
};

ItemDB::ItemDB()
{
  Settings* settings = settings->GetInstance();
  game_language itemLanguage = settings->GetItemLanguage();

  QString itemPath = Paths::GetResourcesPath("chunk/common/item/itemData.itm");
  success_itm = ReadMetaFile(&itm, itemPath);
  LoadGMD(itemLanguage);

  if (itm.header)
    success_flags = ReadCustomFlags(&itm);
}

ItemDB::~ItemDB()
{
  FreeMetaFile(&itm);
  FreeMetaFile(&gmd);
}

void ItemDB::LoadGMD(game_language language)
{
  success_gmd = ReadMetaFileLanguage(&gmd, "chunk/common/text/steam/item_%1.gmd", language);
}

bool ItemDB::ReadCustomFlags(itm_meta* itm) {
  QString flagsPath = Paths::GetResourcesPath("CustomFlags.bin");
  mhw_items_discovered flags[(u64)FlagFileIndex::FlagFileIndexCount] = {};

  u8* obtain = ReadEntireFile(flagsPath, (u8*)flags, sizeof(flags));
  if (!obtain) {
    return false;
  }

  for (u32 i = 0; i < itm->header->entry_count; i++) {
    itm_entry* info = &itm->items[i];

    // Leave the null item as-is.
    if (!info->id) continue;

    // Set stuff for special case items.
    if (info->id == MegaDashJuiceID) info->type = (u32)itemCategory::Item;
    if (info->id == ExhaustCoatingID) info->type = (u32)itemCategory::Ammo;

    u32 is = info->id / 8;
    u32 isi = info->id % 8;
    bool isDiscoverable = flags[(u32)FlagFileIndex::Discoverable].items[is] & (1 << isi);
    bool isObtainable = flags[(u32)FlagFileIndex::Obtainable].items[is] & (1 << isi);
    bool isSlingerAmmo = flags[(u32)FlagFileIndex::SlingerAmmo].items[is] & (1 << isi);
    bool isAppraisalItem = flags[(u32)FlagFileIndex::Appraisal].items[is] & (1 << isi);
    bool isUnavailable = flags[(u32)FlagFileIndex::Unavailable].items[is] & (1 << isi);
    bool isHARDUMMY = flags[(u32)FlagFileIndex::HARDUMMY].items[is] & (1 << isi);
    bool isTripleQ = flags[(u32)FlagFileIndex::TripleQ].items[is] & (1 << isi);

    u32 customFlags = 0;
    if (isDiscoverable)  customFlags |= (u32)itemFlag::CustomDiscoverable;
    if (isObtainable)    customFlags |= (u32)itemFlag::CustomObtainable;
    if (isSlingerAmmo)   customFlags |= (u32)itemFlag::CustomSlingerAmmo;
    if (isAppraisalItem) customFlags |= (u32)itemFlag::CustomAppraisal;
    if (isUnavailable)   customFlags |= (u32)itemFlag::CustomUnavailable;
    if (isHARDUMMY)      customFlags |= (u32)itemFlag::CustomHARDUMMY;
    if (isTripleQ)       customFlags |= (u32)itemFlag::CustomTripleQ;
    info->flags |= customFlags;
  }
  return true;
}

itm_entry* ItemDB::GetItemByIdSafe(u32 id)
{
  if (!success_itm) return nullptr;

  if (id >= itm.header->entry_count) return nullptr;
  return &itm.items[id];
}

itm_entry* ItemDB::GetItemById(u32 id)
{
  return &itm.items[id];
}

int ItemDB::count()
{
  return success_itm ? itm.header->entry_count : 0;
}

QString ItemDB::ItemName(u32 id)
{
  if (!success_gmd) return "GMD Failure";
  
  id = AdjustItemID(id);
  const QRegularExpression regex("(<STYL.*>)(.*)(</STYL>)");
  QString name = QString::fromUtf8(gmd.value(id * 2));
  return name.replace(regex, "\\2");
}

QString ItemDB::ItemName(itm_entry* info)
{
  if (success_itm) return ItemName(info->id);
  else return "ITM Failure";
}

u32 ItemDB::AdjustItemID(u32 id)
{
  switch (id) {
  // Smoke Jewel & Survival Jewel switch
  case SurvivalJewelID: id = SmokeJewelID; break;
  case SmokeJewelID: id = SurvivalJewelID; break;

  // Igni Sign & Hunter Runestone switch
  case IgniSignID: id = HunterRunestoneID; break;
  case HunterRunestoneID: id = IgniSignID; break;
  }
  return id;
}

itm_entry* ItemDB::AdjustItemPtr(itm_entry* entry)
{
  if (entry) {
    switch (entry->id) {
      // Smoke Jewel & Survival Jewel switch
    case SurvivalJewelID: entry = GetItemById(SmokeJewelID); break;
    case SmokeJewelID: entry = GetItemById(SurvivalJewelID); break;

      // Igni Sign & Hunter Runestone switch
    case IgniSignID: entry = GetItemById(HunterRunestoneID); break;
    case HunterRunestoneID: entry = GetItemById(IgniSignID); break;
    }
  }
  return entry;
}

game_language ItemDB::CurrentLanguage()
{
  if (success_gmd) return (game_language)gmd.header->language_id;
  else return game_language::InvalidLanguage;
}
