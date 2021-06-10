#include "ItemDB.h"

#include <QtGlobal>
#include <QString>

ItemDB* ItemDB::instance = nullptr;

ItemDB::ItemDB()
{
  // read a JSON file
  std::ifstream file("res/MasterItemList.json");
  json json;
  file >> json;

  items = json.get<std::vector<itemInfo>>();

  for (u32 i = 0; i < items.size(); i++)
  {
    names.push_back(items[i].name);
    qInfo("Loaded Item[%d]: \"%s\".", i, qUtf8Printable(QString::fromUtf8(items[i].name)));

    if (items[i].id != i)
      qWarning("Item[%d]: Id(%d), name: \"%s\", item index does not match id.", i, items[i].id, items[i].name);
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
  if (id > items.size()) id = 0;
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

std::vector<itemInfo> ItemDB::itemVector()
{
  return items;
}

std::vector<std::string> ItemDB::itemNamesVector()
{
  return names;
}
