#include "BitmapDB.h"
#include <QDir>

#include "../utility/common/Settings.h"

BitmapDB::BitmapDB(ItemDB* itemDB)
{
  items = QImage(Settings::GetResourcesPath("items.png"));
  itemsMask = QImage(Settings::GetResourcesPath("items_mask.png"));
  int imageWidth = items.width();
  int imageHeight = items.height();
  QPainter p;

  iconTints.insert( 0, &items); iconTints.insert( 1, &items);
  iconTints.insert( 2, &items); iconTints.insert( 3, &items);
  iconTints.insert( 4, &items); iconTints.insert( 5, &items);
  iconTints.insert( 6, &items); iconTints.insert( 7, &items);
  iconTints.insert( 8, &items); iconTints.insert( 9, &items);
  iconTints.insert(10, &items); iconTints.insert(11, &items);
  iconTints.insert(12, &items); iconTints.insert(13, &items);
  iconTints.insert(14, &items); iconTints.insert(15, &items);
  iconTints.insert(16, &items); iconTints.insert(19, &items);
  iconTints.insert(22, &items); iconTints.insert(24, &items);
  iconTints.insert(25, &items); iconTints.insert(26, &items);

  Settings* settings = settings->GetInstance();
  QPainter::CompositionMode modeA = QPainter::CompositionMode_DestinationOut;
  QPainter::CompositionMode modeB = QPainter::CompositionMode_Multiply;
  bool matrixMode = settings->GetMatrixMode();
  bool darkMode = settings->GetDarkMode();
  if (matrixMode) {
    modeA = QPainter::CompositionMode_DestinationIn;
    modeB = QPainter::CompositionMode_DestinationIn;
  }

  int count = 0;
  QMapIterator<u32, QImage*> tinter(iconTints);
  while (tinter.hasNext()) {
    tinter.next();
    u32 key = tinter.key();

    QImage* tint = new QImage(1024, 1024, QImage::Format_ARGB32);
    QColor pallete = (matrixMode && !darkMode) ? palletes[2] : palletes[key];
    tint->fill(pallete);

    p.begin(tint);
    p.setCompositionMode(modeA);
    p.fillRect(0, 0, imageWidth, imageHeight, itemsMask);
    p.setCompositionMode(modeB);
    p.fillRect(0, 0, imageWidth, imageHeight, items);
    p.end();

    iconTints.insert(key, tint);
  }

  for (size_t i = 0; i < itemDB->count(); i++)
  {
    itm_entry* info = itemDB->GetItemById(i);
    int icon_id = info->icon_id;
    int icon_color = info->icon_color;

    u64 key = BuildKey(icon_id, icon_color);
    if (!icons.contains(key)) {
      int iconWidth = 64;
      int iconHeight = 64;
      int x = (icon_id % 16) * iconWidth;
      int y = (icon_id / 16) * iconHeight;

      QImage iconImage = iconTints.value(icon_color & 0xFF, &items)->copy(x, y, iconWidth, iconHeight);
      QPixmap iconPixmap = QPixmap::fromImage(iconImage);
      QIcon* icon = new QIcon(iconPixmap);
      icons.insert(key, icon);
    }
  }
}

void BitmapDB::OutputIcons(const QString& path, ItemDB* itemDB)
{
  QDir outDir = QDir(path);
  QString outPath = outDir.path();

  QMapIterator<u32, QImage*> tinter(iconTints);
  while (tinter.hasNext()) {
    tinter.next();
    u32 key = tinter.key();

    QImage* tint = iconTints.value(key);
    if (tint)
      tint->save(QString("%1/items_p%2.png").arg(outPath).arg(key));
  }

  for (size_t i = 0; i < itemDB->count(); i++)
  {
    itm_entry* info = itemDB->GetItemById(i);
    QIcon* icon = ItemIcon(info);

    if (icon) {
      QPixmap iconPixmap = icon->pixmap(icon->availableSizes().first());
      iconPixmap.save(QString("%1/item_%2_%3.png").arg(outPath).arg(info->icon_id).arg(info->icon_color));
    }
  }
}

BitmapDB::~BitmapDB()
{
  QMapIterator<u64, QIcon*> iconMap(icons);
  while (iconMap.hasNext()) {
    iconMap.next();
    delete(iconMap.value());
  }

  QMapIterator<u32, QImage*> tinter(iconTints);
  while (tinter.hasNext()) {
    tinter.next();
    delete(tinter.value());
  }
}

QIcon* BitmapDB::ItemIcon(itm_entry* info)
{
  if (!info) return nullptr;

  u64 key = BuildKey(info->icon_id, info->icon_color);
  return icons.value(key, nullptr);
}
