#include "BitmapDB.h"
#include <QDir>

#include "../utility/common/Settings.h"
#include "../utility/system/paths.h"

BitmapDB::BitmapDB(ItemDB* itemDB, EquipmentDB* equipmentDB)
{
  items = QImage(Paths::GetResourcesPath("items.png"));
  itemsMask = QImage(Paths::GetResourcesPath("items_mask.png"));
  ReadMetaFile(&uct, Paths::GetResourcesPath("chunk/common/ui/ui_colortable.uct"));

  int imageWidth = items.width();
  int imageHeight = items.height();

  Settings* settings = settings->GetInstance();
  bool matrixMode = settings->GetMatrixMode();
  bool darkMode = settings->GetDarkMode();

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
  QPainter::CompositionMode modeA = QPainter::CompositionMode_DestinationOut;
  QPainter::CompositionMode modeB = QPainter::CompositionMode_Multiply;
  if (matrixMode) {
    modeA = QPainter::CompositionMode_DestinationIn;
    modeB = QPainter::CompositionMode_DestinationIn;
  }

  mhw_color matrixColor = uct.item->color_info[2].color;
  QPainter p;

  for (u16 indexPallete = 0; indexPallete < COUNTOF(uct.palletes); ++indexPallete) {
    uct_color_pallete* pallete = uct.palletes[indexPallete];

    for (u16 indexColor = 0; indexColor < pallete->color_count; ++indexColor) {
      u32 key = indexPallete << 16 | indexColor;

      uct_color_pallete* pallete = uct.palletes[indexPallete];
      mhw_color trueColor = pallete->color_info[indexColor].color;
      mhw_color fullColor = (matrixMode && !darkMode) ? matrixColor : trueColor;
      QColor color = fullColor.a << 24 | fullColor.r << 16 | fullColor.g << 8 | fullColor.b << 0;

      QImage* tint = new QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
      tint->fill(color);

      p.begin(tint);
      p.setCompositionMode(modeA);
      p.fillRect(0, 0, imageWidth, imageHeight, itemsMask);
      p.setCompositionMode(modeB);
      p.fillRect(0, 0, imageWidth, imageHeight, items);
      p.end();

      iconTints.insert(key, tint);
    }
  }
}

void BitmapDB::InitItems(ItemDB* itemDB)
{
  const int icon_pallete = 0;

  for (size_t i = 0; i < itemDB->count(); i++)
  {
    itm_entry* info = itemDB->GetItemById(i);
    int icon_id = info->icon_id;
    int icon_color = info->icon_color;

    u64 key = BuildKey(icon_id, icon_pallete, icon_color);
    if (!icons.contains(key)) {
      AddIcon(key, icon_id, icon_pallete, icon_color & 0xFF);
    }
  }
}

void BitmapDB::InitEquipment(EquipmentDB* equipmentDB)
{
  const int icon_pallete = 1;

  for (int i = 0; i < equipmentDB->CountArmor(); i++)
  {
    am_dat_entry* armor = equipmentDB->IndexArmor(i);
    int icon_id = 16 * 14 + armor->equip_slot;
    int icon_color = armor->rarity;

    u64 key = BuildKey(icon_id, icon_pallete, icon_color);
    if (!icons.contains(key)) {
      AddIcon(key, icon_id, icon_pallete, icon_color);
    }
  }

  for (int i = 0; i < equipmentDB->CountKinsect(); i++)
  {
    rod_inse_entry* kinsect = equipmentDB->IndexKinsect(i);
    int icon_id = 16 * 14 + 8 + kinsect->attack_type;
    int icon_color = kinsect->rarity;

    u64 key = BuildKey(icon_id, icon_pallete, icon_color);
    if (!icons.contains(key)) {
      AddIcon(key, icon_id, icon_pallete, icon_color);
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

      u64 key = BuildKey(icon_id, icon_pallete, icon_color);
      if (!icons.contains(key)) {
        AddIcon(key, icon_id, icon_pallete, icon_color);
      }
    }

    for (int i = 0; i < wp_dat_gCount; i++)
    {
      wp_dat_g_entry* wp_dat_g = equipmentDB->IndexWeaponRanged(y, i);
      int icon_color = wp_dat_g->rarity;

      u64 key = BuildKey(icon_id, icon_pallete, icon_color);
      if (!icons.contains(key)) {
        AddIcon(key, icon_id, icon_pallete, icon_color);
      }
    }
  }
}

void BitmapDB::AddIcon(u64 key, u32 id, u16 pallete, u16 color)
{
  int x = (id % 16) * iconWidth;
  int y = (id / 16) * iconHeight;

  u32 palleteColor = pallete << 16 | color;
  QImage iconImage = iconTints.value(palleteColor, &items)->copy(x, y, iconWidth, iconHeight);
  QPixmap iconPixmap = QPixmap::fromImage(iconImage);
  QIcon* icon = new QIcon(iconPixmap);
  icons.insert(key, icon);
}

QIcon* BitmapDB::ItemIcon(itm_entry* info)
{
  if (!info) return &nullIcon;
  const int icon_pallete = 0;

  u64 key = BuildKey(info->icon_id, icon_pallete, info->icon_color);
  return icons.value(key, &nullIcon);
}

QIcon* BitmapDB::EquipmentIcon(mhw_equipment* equipment)
{
  if (!equipment) return &nullIcon;
  const int icon_pallete = 1;

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

    key = BuildKey(icon_id, icon_pallete, icon_color);
  } break;

  case 1: // Weapons
  {
    wp_dat_entry* wp_dat = equipmentDB->GetEntryWeaponMelee(type, id);
    wp_dat_g_entry* wp_dat_g = equipmentDB->GetEntryWeaponRanged(type, id);

    int icon_id = 16 * 15 + wp_index_id_map[type];
    int icon_color = 0;
    if (wp_dat) icon_color = wp_dat->rarity;
    if (wp_dat_g) icon_color = wp_dat_g->rarity;

    key = BuildKey(icon_id, icon_pallete, icon_color);
  }; break;

  case 4: // Kinsects
  {
    rod_inse_entry* kinsect = equipmentDB->GetEntryKinsect(type, id);
    int icon_id = 16 * 14 + 8 + kinsect->attack_type;
    int icon_color = kinsect->rarity;

    key = BuildKey(icon_id, icon_pallete, icon_color);
  } break;
  }

  return icons.value(key, &nullIcon);
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
