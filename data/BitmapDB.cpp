#include "BitmapDB.h"
#include <QDir>

BitmapDB* BitmapDB::instance = nullptr;

BitmapDB::BitmapDB()
{

}

BitmapDB* BitmapDB::GetInstance()
{
  if (!instance) instance = new BitmapDB();
  return instance;
}

void BitmapDB::Free()
{
  QMapIterator<u64, QIcon*> i(icons);
  while (i.hasNext()) {
    i.next();
    free(i.value());
  }

  delete(instance);
  instance = nullptr;
}

QIcon* BitmapDB::ItemPixmap(u32 icon_id, u32 icon_color)
{
  QIcon* result = nullptr;
  u64 key = (((u64)icon_id) << 32) | icon_color;

  if (!icons.contains(key)) {
    QString path = QString("res/ItemIcons/%1_%2.png").arg(icon_id).arg(icon_color);
    qInfo("Loading %s", qUtf8Printable(path));

    if (QFile::exists(path))
      result = new QIcon(path);
    icons.insert(key, result);
  }
  else {
    qInfo("Re-using icon id=%d, color=%d", icon_id, icon_color);
    result = icons.value(key);
  }

  return result;
}

QIcon* BitmapDB::LoadItemIcon(itemInfo* info)
{
  QIcon* icon = ItemPixmap(info->icon_id, info->icon_color);
  return icon;
}

QIcon* BitmapDB::ItemIcon(itemInfo* info)
{
  u64 key = (((u64)info->icon_id) << 32) | info->icon_color;
  return icons.value(key, nullptr);
}
