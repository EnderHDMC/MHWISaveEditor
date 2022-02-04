#include "ItemDB.h"

#include <QtGlobal>
#include <QFile>

#include "../utility/settype.h"

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
  // read a JSON file
  std::ifstream file("res/MasterItemList.json");
  if (!file) {
    qFatal("Cannot open file: res/MasterItemList.json");
    loadError = true;
    fatalError = true;
    return;
  }
  json json;
  file >> json;

  items = json.get<std::vector<itemInfo>>();
  qInfo("Loaded %d items.", items.size());

  QFile obtainFile("res/CustomFlags.bin");
  if (!obtainFile.open(QIODevice::ReadOnly)) {
    qWarning("Cannot open file: res/CustomFlags.bin");
    loadError = true;
    return;
  }

  // It just sounded like a cool name.
  QByteArray blobtain = obtainFile.readAll();
  file.close();
  mhw_items_discovered flags[(u64)FlagFileIndex::FlagFileIndexCount] = {};
  u8* obtain = QByteArrayToU8(blobtain, (u8*)flags, sizeof(flags));
  if (!obtain) {
    loadError = true;
    return;
  }

  for (u32 i = 0; i != items.size(); i++) {
    itemInfo* info = &items[i];

    // Leave the null item as-is.
    if (!info->id) continue;

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
}

ItemDB* ItemDB::GetInstance()
{
  if (!instance) instance = new ItemDB();
  return instance;
}

void ItemDB::Free()
{
  items.~vector();
  delete(instance);
  instance = nullptr;
}

// TODO: Right now we're assuming item id = item index
itemInfo* ItemDB::GetItemById(u32 id)
{
  if (id >= items.size()) return nullptr;
  return &items[id];
}

itemInfo* ItemDB::GetItemByIndex(int index)
{
  return &items[index];
}

int ItemDB::count()
{
  return items.size();
}

QString ItemDB::ItemName(itemInfo* info)
{
  switch (info->id) {
  case SurvivalJewelID:
    return QString::fromUtf8(GetItemById(SmokeJewelID)->name);

  case SmokeJewelID:
    return QString::fromUtf8(GetItemById(SurvivalJewelID)->name);

  default:
    return QString::fromUtf8(info->name);
  }
}
