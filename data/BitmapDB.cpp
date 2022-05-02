#include "BitmapDB.h"
#include <QDir>

#include "../utility/common/Settings.h"
#include "../utility/system/paths.h"
#include "../utility/math.h"

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

QIcon* BitmapDB::ItemIcon(ItemDB* itemDB, itm_entry* info)
{
  if (!info) return &nullIcon;
  const int icon_pallete = 0;

  info = itemDB->AdjustItemPtr(info);
  u64 key = BuildKey(info->icon_id, icon_pallete, info->icon_color);
  return icons.value(key, &nullIcon);
}

QIcon* BitmapDB::EquipmentIcon(mhw_equipment* equipment)
{
  if (!equipment) return &nullIcon;
  const int icon_pallete = 1;

  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  mhw_equip_category category = equipment->category;
  i32 type = equipment->type;
  u32 id = equipment->id;
  u64 key = -1;

  switch (category) {
  case mhw_equip_category::Armor:
  case mhw_equip_category::Charm:
  {
    am_dat_entry* armor = equipmentDB->GetEntryArmor(type, id);
    int icon_id = 16 * 14 + armor->equip_slot;
    int icon_color = armor->rarity;

    key = BuildKey(icon_id, icon_pallete, icon_color);
  } break;

  case mhw_equip_category::Weapon:
  {
    wp_dat_entry* wp_dat = equipmentDB->GetEntryWeaponMelee(type, id);
    wp_dat_g_entry* wp_dat_g = equipmentDB->GetEntryWeaponRanged(type, id);

    int icon_id = 16 * 15 + wp_index_id_map[type];
    int icon_color = 0;
    if (wp_dat) icon_color = wp_dat->rarity;
    if (wp_dat_g) icon_color = wp_dat_g->rarity;

    key = BuildKey(icon_id, icon_pallete, icon_color);
  }; break;

  case mhw_equip_category::Kinsect:
  {
    rod_inse_entry* kinsect = equipmentDB->GetEntryKinsect(type, id);
    int icon_id = 16 * 14 + 8 + kinsect->attack_type;
    int icon_color = kinsect->rarity;

    key = BuildKey(icon_id, icon_pallete, icon_color);
  } break;
  }

  return icons.value(key, &nullIcon);
}

bool BitmapDB::DebugDumpAtlas(const QString& path)
{
  QDir outDir = QDir(path);
  QString outPath = outDir.path();
  if (path.isNull() || !outDir.exists()) return false;

  const int iconCount = icons.count();
  const int atlasIconCount = RoundUpPow4(iconCount);
  const int atlasCountRoot = 1 << (BSR(atlasIconCount) / 2);

  const int atlasWidth = atlasCountRoot;
  const int atlasHeight = atlasCountRoot;
  const int atlasCanvasWidth = atlasWidth * iconWidth;
  const int atlasCanvasHeight = atlasHeight * iconHeight;

  QPainter p;
  QImage atlas(atlasCanvasWidth, atlasCanvasHeight, QImage::Format_ARGB32);

  p.begin(&atlas);
  p.setCompositionMode(QPainter::CompositionMode_Source);
  p.fillRect(atlas.rect(), Qt::transparent);
  p.setCompositionMode(QPainter::CompositionMode_SourceOver);

  u32 index = 0;
  QMapIterator<u64, QIcon*> iconIterator(icons);
  while (iconIterator.hasNext()) {
    iconIterator.next();
    u64 key = iconIterator.key();
    u32 icon_id = 0;
    u16 icon_pallete = 0;
    u16 icon_color = 0;
    DissectKey(key, &icon_id, &icon_pallete, &icon_color);

    QIcon* icon = iconIterator.value();
    if (icon) {
      QPixmap iconPixmap = icon->pixmap(icon->availableSizes().first());
      int x = (index % atlasWidth) * iconWidth;
      int y = (index / atlasWidth) * iconHeight;

      p.fillRect(x, y, iconWidth, iconHeight, iconPixmap);
    }
    ++index;
  }

  p.end();

  QString filepath = QString("%1/atlas.png").arg(outPath);
  bool success = atlas.save(filepath);
  if (success) {
    qInfo().noquote() << "Dumped atlas:" << filepath;
  }
  else {
    qWarning().noquote() << "Failed to dump atlas:" << filepath;
  }
  return success;
}

bool BitmapDB::DebugDumpAtlases(const QString& path)
{
  QDir outDir = QDir(path);
  QString outPath = outDir.path();
  if (path.isNull() || !outDir.exists()) return false;

  bool success = true;
  QMapIterator<u32, QImage*> tinter(iconTints);
  while (tinter.hasNext()) {
    tinter.next();
    u32 key = tinter.key();
    u32 icon_id = 0;
    u16 icon_pallete = 0;
    u16 icon_color = 0;
    DissectKey(key, &icon_id, &icon_pallete, &icon_color);

    QString filepath = QString("%1/atlas_p%2_c%3.png").arg(outPath).arg(icon_pallete).arg(icon_color);
    QImage* tint = tinter.value();
    bool localSuccess = (bool)tint;
    if (localSuccess) {
      localSuccess &= tint->save(filepath);
    }

    success &= localSuccess;
    if (localSuccess) {
      qInfo().noquote() << "Dumped atlas:" << filepath;
    }
    else {
      qWarning().noquote() << "Failed to dump atlas:" << filepath;
    }
  }

  return success;
}

bool BitmapDB::DebugDumpIcons(const QString& path)
{
  QDir outDir = QDir(path);
  QString outPath = outDir.path();
  if (path.isNull() || !outDir.exists()) return false;

  bool success = true;
  QMapIterator<u64, QIcon*> iconIterator(icons);
  while (iconIterator.hasNext()) {
    iconIterator.next();
    u64 key = iconIterator.key();
    u32 icon_id = 0;
    u16 icon_pallete = 0;
    u16 icon_color = 0;
    DissectKey(key, &icon_id, &icon_pallete, &icon_color);

    QString filepath = QString("%1/item_i%2_p%3_c%4.png").arg(outPath).arg(icon_id).arg(icon_pallete).arg(icon_color);
    QIcon* icon = iconIterator.value();
    bool localSuccess = (bool)icon;
    if (localSuccess) {
      QPixmap iconPixmap = icon->pixmap(icon->availableSizes().first());
      localSuccess &= iconPixmap.save(filepath);
    }

    if (localSuccess) {
      qInfo().noquote() << "Dumped icon:" << filepath;
    }
    else {
      qWarning().noquote() << "Failed to dump icon:" << filepath;
    }
  }

  return success;
}

bool BitmapDB::DebugDumpUsedMask(const QString& path)
{
  QDir outDir = QDir(path);
  QString outPath = outDir.path();
  if (path.isNull() || !outDir.exists()) return false;

  const int iconCount = icons.count();
  const int atlasIconCount = RoundUpPow4(iconCount);
  const int atlasCountRoot = 1 << (BSR(atlasIconCount) / 2);

  const int atlasWidth = atlasCountRoot;
  const int atlasHeight = atlasCountRoot;
  const int atlasCanvasWidth = atlasWidth * iconWidth;
  const int atlasCanvasHeight = atlasHeight * iconHeight;

  QPainter p;
  QImage usedImage = items.copy();

  p.begin(&usedImage);
  p.setCompositionMode(QPainter::CompositionMode_Clear);

  u32 index = 0;
  QMapIterator<u64, QIcon*> iconIterator(icons);
  while (iconIterator.hasNext()) {
    iconIterator.next();
    u64 key = iconIterator.key();
    u32 icon_id = 0;
    u16 dummy = 0;
    DissectKey(key, &icon_id, &dummy, &dummy);

    QIcon* icon = iconIterator.value();
    if (icon) {
      QPixmap iconPixmap = icon->pixmap(icon->availableSizes().first());
      int x = (icon_id % 16) * iconWidth;
      int y = (icon_id / 16) * iconHeight;

      p.setBrush(QBrush(Qt::black));
      p.fillRect(x, y, iconWidth, iconHeight, p.brush());
    }
    ++index;
  }

  QPainter pb;
  QPixmap brushPattern(iconWidth, iconHeight);
  brushPattern.fill(Qt::transparent);
  pb.begin(&brushPattern);
  QPolygon square(QRect(0, 0, iconWidth - 1, iconHeight - 1), true);
  pb.drawPolygon(square);
  pb.end();

  p.setCompositionMode(QPainter::CompositionMode_SourceOver);
  p.drawTiledPixmap(usedImage.rect(), brushPattern);
  p.end();

  QString filepath = QString("%1/used.png").arg(outPath);
  bool success = usedImage.save(filepath);
  if (success) {
    qInfo().noquote() << "Dumped used mask:" << filepath;
  }
  else {
    qWarning().noquote() << "Failed to dump used mask:" << filepath;
  }
  return success;
}
