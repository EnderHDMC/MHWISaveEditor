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

void BitmapDB::Init(ItemDB* itemDB)
{
  items = QImage("res/items.png");
  itemsMask = QImage("res/items_mask.png");
  int imageWidth = items.width();
  int imageHeight = items.height();
  QPainter p;

  QColor palletes[] = {
    QColor::fromRgb(0xEB, 0xEB, 0xEB, 0xFF), QColor::fromRgb(0xE1, 0x50, 0x5C, 0xFF),
    QColor::fromRgb(0x47, 0xB2, 0x67, 0xFF), QColor::fromRgb(0x57, 0x7B, 0xFF, 0xFF),
    QColor::fromRgb(0xE8, 0xC5, 0x06, 0xFF), QColor::fromRgb(0x97, 0x88, 0xD1, 0xFF),
    QColor::fromRgb(0x4E, 0xC3, 0xE5, 0xFF), QColor::fromRgb(0xDF, 0x9C, 0x65, 0xFF),
    QColor::fromRgb(0xD9, 0x8B, 0x94, 0xFF), QColor::fromRgb(0xDF, 0xE5, 0x00, 0xFF),
    QColor::fromRgb(0xAF, 0xAF, 0xAF, 0xFF), QColor::fromRgb(0xBA, 0x95, 0x4B, 0xFF),
    QColor::fromRgb(0x3D, 0x7F, 0x3A, 0xFF), QColor::fromRgb(0xAD, 0x15, 0x47, 0xFF),
    QColor::fromRgb(0x4C, 0x4C, 0xD9, 0xFF), QColor::fromRgb(0xD7, 0xC1, 0x8D, 0xFF),
    QColor::fromRgb(0xD7, 0xC1, 0x8D, 0xFF), QColor::fromRgb(0x86, 0xB2, 0x39, 0xFF)
  };

  iconTints.insert( 0, nullptr); iconTints.insert( 1, nullptr);
  iconTints.insert( 2, nullptr); iconTints.insert( 3, nullptr);
  iconTints.insert( 4, nullptr); iconTints.insert( 5, nullptr);
  iconTints.insert( 6, nullptr); iconTints.insert( 7, nullptr);
  iconTints.insert( 8, nullptr); iconTints.insert( 9, nullptr);
  iconTints.insert(10, nullptr); iconTints.insert(11, nullptr);
  iconTints.insert(13, nullptr); iconTints.insert(14, nullptr);
  iconTints.insert(15, nullptr); iconTints.insert(24, nullptr);
  iconTints.insert(25, nullptr); iconTints.insert(26, nullptr);

  int count = 0;
  QMapIterator<u32, QImage*> tinter(iconTints);
  while (tinter.hasNext()) {
    tinter.next();
    QImage* tint = new QImage(itemsMask);
    QColor pallete = palletes[count++];

    p.begin(tint);
    p.setCompositionMode(QPainter::CompositionMode_Multiply);
    p.fillRect(0, 0, imageWidth, imageHeight, pallete);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(0, 0, imageWidth, imageHeight, items);
    p.end();

    iconTints.insert(tinter.key(), tint);
  }

  for (size_t i = 0; i < itemDB->count(); i++)
  {
    itemInfo* info = itemDB->GetItemById(i);
    int icon_id = info->icon_id;
    int icon_color = info->icon_color;

    u64 key = BuildKey(icon_id, icon_color);
    if (!icons.contains(key)) {
      int iconWidth = 64;
      int iconHeight = 64;
      int x = (icon_id % 16) * iconWidth;
      int y = (icon_id / 16) * iconHeight;

      QImage iconImage = iconTints.value(icon_color, &items)->copy(x, y, iconWidth, iconHeight);
      QPixmap iconPixmap = QPixmap::fromImage(iconImage);
      QIcon* icon = new QIcon(iconPixmap);
      icons.insert(key, icon);
    }
  }
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
  u64 key = BuildKey(icon_id, icon_color);

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

QIcon* BitmapDB::ItemIcon(itemInfo* info)
{
  u64 key = BuildKey(info->icon_id, info->icon_color);
  return icons.value(key, nullptr);
}
