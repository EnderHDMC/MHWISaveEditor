#include "ItemDB.h"

#include <QDebug>
#include <QFile>

#include "../utility/settype.h"
#include "../types/inventory_areas.h"
#include "../gui/common/Settings.h"

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

ItemDB* ItemDB::instance = nullptr;

ItemDB::ItemDB()
{
  Settings* settings = settings->GetInstance();
  QString language = settings->itemLanguage;

  itmLoad = ReadItemData(&itm);
  gmdLoad = ReadGMD(&gmd, language);

  if (itm.header)
    flagsLoad = ReadCustomFlags(&itm);
}

bool ItemDB::ReadItemData(itm_meta* meta) {
  QString itemPath = "res/game/itemData.itm";
  QFile itemFile(itemPath);
  if (!itemFile.open(QIODevice::ReadOnly)) {
    qWarning() << "Cannot open file: " + itemPath;
    return false;
  }

  QByteArray itmBlob = itemFile.readAll();
  itemFile.close();

  itm_header* itmHeader = (itm_header*)QByteArrayToU8(itmBlob, nullptr, itmBlob.size());
  if (!itmHeader) {
    return false;
  }

  InitMeta_itm(meta, itmHeader);
  return true;
}

bool ItemDB::ReadGMD(gmd_meta* meta, const QString& language)
{
  QString namePath = QString("res/game/item_%1.gmd").arg(language);
  QFile nameFile(namePath);
  if (!nameFile.open(QIODevice::ReadOnly)) {
    qWarning() << "Cannot open file: " + namePath;
    return false;
  }

  QByteArray nameBlob = nameFile.readAll();
  nameFile.close();

  gmd_header* gmdHeader = (gmd_header*)QByteArrayToU8(nameBlob, nullptr, nameBlob.size());
  if (!gmdHeader) {
    return false;
  }

  InitMeta_gmd(meta, gmdHeader);
  return true;
}

bool ItemDB::ReadCustomFlags(itm_meta* itm) {
  QString flagsPath = "res/CustomFlags.bin";
  QFile flagsFile(flagsPath);
  if (!flagsFile.open(QIODevice::ReadOnly)) {
    qWarning() << "Cannot open file: " + flagsPath;
    return false;
  }

  QByteArray flagsBlob = flagsFile.readAll();
  flagsFile.close();
  mhw_items_discovered flags[(u64)FlagFileIndex::FlagFileIndexCount] = {};
  u8* obtain = QByteArrayToU8(flagsBlob, (u8*)flags, sizeof(flags));
  if (!obtain) {
    return false;
  }

  for (u32 i = 0; i < itm->header->entry_count; i++) {
    itm_entry* info = &itm->items[i];

    // Leave the null item as-is.
    if (!info->id) continue;

    // Set stuff for special case items.
    if (info->id == MegaDashJuiceID) info->type = (u32)item_type::Item;
    if (info->id == ExhaustCoatingID) info->type = (u32)item_type::Ammo;

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

ItemDB* ItemDB::GetInstance()
{
  if (!instance) instance = new ItemDB();
  return instance;
}

void ItemDB::Free()
{
  FreeMeta_itm(&itm);
  FreeMeta_gmd(&gmd);

  delete(instance);
  instance = nullptr;
}

itm_entry* ItemDB::GetItemByIdSafe(u32 id)
{
  if (!itmLoad) return nullptr;

  if (id >= itm.header->entry_count) return nullptr;
  return &itm.items[id];
}

itm_entry* ItemDB::GetItemById(u32 id)
{
  return &itm.items[id];
}

int ItemDB::count()
{
  return itmLoad ? itm.header->entry_count : 0;
}

QString ItemDB::ItemName(u32 id)
{
  if (!gmdLoad) return "GMD Failure";

  switch (id) {
  case SurvivalJewelID:
    id = SmokeJewelID;
    break;

  case SmokeJewelID:
    id = SurvivalJewelID;
    break;

  default:
    break;
  }

  return QString::fromUtf8(gmd.strings[id * 2]);
}

QString ItemDB::ItemName(itm_entry* info)
{
  if (itmLoad) return ItemName(info->id);
  else return "ITM Failure";
}
