#include "BitmapDB.h"
#include <QDir>

#include "../utility/common/Settings.h"
#include "../utility/system/paths.h"

BitmapDB::BitmapDB(ItemDB* itemDB)
{
  items = QImage(Paths::GetResourcesPath("items.png"));
  itemsMask = QImage(Paths::GetResourcesPath("items_mask.png"));
  int imageWidth = items.width();
  int imageHeight = items.height();

  Settings* settings = settings->GetInstance();
  bool matrixMode = settings->GetMatrixMode();
  bool darkMode = settings->GetDarkMode();

  EquipmentDB* equipmentDB = equipmentDB->GetInstance();

  InitTints(imageWidth, imageHeight, matrixMode, darkMode);
  InitItems(itemDB);
  InitEquipment(equipmentDB);
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

void BitmapDB::InitTints(int imageWidth, int imageHeight, bool matrixMode, bool darkMode)
{
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

  QPainter::CompositionMode modeA = QPainter::CompositionMode_DestinationOut;
  QPainter::CompositionMode modeB = QPainter::CompositionMode_Multiply;
  if (matrixMode) {
    modeA = QPainter::CompositionMode_DestinationIn;
    modeB = QPainter::CompositionMode_DestinationIn;
  }

  QPainter p;
  int count = 0;
  QMapIterator<u32, QImage*> tinter(iconTints);
  while (tinter.hasNext()) {
    tinter.next();
    u32 key = tinter.key();

    QImage* tint = new QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
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
}

void BitmapDB::InitItems(ItemDB* itemDB)
{
  for (size_t i = 0; i < itemDB->count(); i++)
  {
    itm_entry* info = itemDB->GetItemById(i);
    int icon_id = info->icon_id;
    int icon_color = info->icon_color;

    u64 key = BuildKey(icon_id, icon_color);
    if (!icons.contains(key)) {
      AddIcon(key, icon_id, icon_color & 0xFF);
    }
  }
}

void BitmapDB::InitEquipment(EquipmentDB* equipmentDB)
{
  for (int i = 0; i < equipmentDB->CountArmor(); i++)
  {
    am_dat_entry* armor = equipmentDB->IndexArmor(i);
    int icon_id = 16 * 14 + armor->equip_slot;
    int icon_color = armor->rarity;

    u64 key = BuildKey(icon_id, icon_color);
    if (!icons.contains(key)) {
      AddIcon(key, icon_id, icon_color);
    }
  }

  for (int i = 0; i < equipmentDB->CountKinsect(); i++)
  {
    rod_inse_entry* kinsect = equipmentDB->IndexKinsect(i);
    int icon_id = 16 * 14 + 8 + kinsect->attack_type;
    int icon_color = kinsect->rarity;

    u64 key = BuildKey(icon_id, icon_color);
    if (!icons.contains(key)) {
      AddIcon(key, icon_id, icon_color);
    }
  }

  for (int y = 0; y < 14; y++)
  {
    int wp_datCount = equipmentDB->CountWeaponMelee(y);
    int wp_dat_gCount = equipmentDB->CountWeaponRanged(y);
    int icon_id = 16 * 15 + wp_index_id_map[y];

    for (int i = 0; i < wp_datCount; i++)
    {
      wp_dat_entry* wp_dat = equipmentDB->IndexWeaponMelee(y, i);
      int icon_color = wp_dat->rarity;

      u64 key = BuildKey(icon_id, icon_color);
      if (!icons.contains(key)) {
        AddIcon(key, icon_id, icon_color);
      }
    }

    for (int i = 0; i < wp_dat_gCount; i++)
    {
      wp_dat_g_entry* wp_dat_g = equipmentDB->IndexWeaponRanged(y, i);
      int icon_color = wp_dat_g->rarity;

      u64 key = BuildKey(icon_id, icon_color);
      if (!icons.contains(key)) {
        AddIcon(key, icon_id, icon_color);
      }
    }
  }
}

void BitmapDB::AddIcon(u64 key, int id, int color)
{
  int x = (id % 16) * iconWidth;
  int y = (id / 16) * iconHeight;

  QImage iconImage = iconTints.value(color, &items)->copy(x, y, iconWidth, iconHeight);
  QPixmap iconPixmap = QPixmap::fromImage(iconImage);
  QIcon* icon = new QIcon(iconPixmap);
  icons.insert(key, icon);
}

QIcon* BitmapDB::ItemIcon(itm_entry* info)
{
  if (!info) return nullptr;

  u64 key = BuildKey(info->icon_id, info->icon_color);
  return icons.value(key, nullptr);
}

QIcon* BitmapDB::EquipmentIcon(mhw_equipment* equipment)
{
  if (!equipment) return nullptr;

  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  i32 category = equipment->serial_item_category;
  i32 type = equipment->type;
  u32 id = equipment->id;
  u64 key = -1;

  switch (category) {
  case 0: // Armors
  case 2: // Charms
  {
    am_dat_entry* armor = equipmentDB->GetEntryArmor(type, id);
    int icon_id = 16 * 14 + armor->equip_slot;
    int icon_color = armor->rarity;

    key = BuildKey(icon_id, icon_color);
  } break;

  case 1: // Weapons
  {
    wp_dat_entry* wp_dat = equipmentDB->GetEntryWeaponMelee(type, id);
    wp_dat_g_entry* wp_dat_g = equipmentDB->GetEntryWeaponRanged(type, id);

    int icon_id = 16 * 15 + wp_index_id_map[type];
    int icon_color = 0;
    if (wp_dat) icon_color = wp_dat->rarity;
    if (wp_dat_g) icon_color = wp_dat_g->rarity;

    key = BuildKey(icon_id, icon_color);
  }; break;

  case 4: // Kinsects
  {
    rod_inse_entry* kinsect = equipmentDB->GetEntryKinsect(type, id);
    int icon_id = 16 * 14 + 8 + kinsect->attack_type;
    int icon_color = kinsect->rarity;

    key = BuildKey(icon_id, icon_color);
  } break;
  }

  return icons.value(key, nullptr);
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
