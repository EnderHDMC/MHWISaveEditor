#include "BitmapDB.h"
#include <QDir>

#include "../utility/common/Settings.h"
#include "../utility/system/paths.h"
#include "../utility/math.h"
#include "IconAtlas.h"

BitmapDB::BitmapDB(ItemDB* itemDB, EquipmentDB* equipmentDB)
{
  items = QImage(Paths::GetResourcesPath("items.png"));
  itemsMask = QImage(Paths::GetResourcesPath("items_mask.png"));
  ReadMetaFile(&uct, Paths::GetResourcesPath("chunk/common/ui/ui_colortable.uct"));
  textureAtlas = QPixmap::fromImage(items);
  maskAtlas = QPixmap::fromImage(itemsMask);

  int imageWidth = items.width();
  int imageHeight = items.height();
}

BitmapDB::~BitmapDB()
{
  QMapIterator<u64, QIcon*> iconMap(icons);
  while (iconMap.hasNext()) {
    iconMap.next();
    delete(iconMap.value());
  }
}

QIcon* BitmapDB::CreateIcon(u64 key, QIcon* nullIcon)
{
  if (key == (u64)-1) return nullIcon;
  if (icons.contains(key)) return icons.value(key, nullIcon);

  u32 icon_id = 0;
  u16 icon_palette = 0;
  u16 icon_color = 0;
  DissectKey(key, &icon_id, &icon_palette, &icon_color);

  const uct_color_palette* palette = uct.palettes[icon_palette];
  const mhw_color fullColor = palette->color_info[icon_color].color;
  QColor iconColor = QColor(fullColor.r, fullColor.g, fullColor.b, fullColor.a);

  const int x = (icon_id % 16) * iconWidth;
  const int y = (icon_id / 16) * iconHeight;
  QRect iconRegion(x, y, iconWidth, iconHeight);

  IconAtlas* iconEngine = new IconAtlas(textureAtlas, maskAtlas, iconRegion, iconColor);
  QIcon* icon = new QIcon(iconEngine);
  icons.insert(key, icon);
  return icon;
}

QIcon* BitmapDB::ItemIcon(ItemDB* itemDB, itm_entry* info)
{
  if (!info) return &nullIcon;
  info = itemDB->AdjustItemPtr(info);
  const u32 id = info->icon_id;
  const u16 palette = 0;
  const u16 color = info->icon_color;

  u64 key = BuildKey(id, palette, color);
  QIcon* icon = CreateIcon(key, &nullIcon);
  return icon;
}

QIcon* BitmapDB::EquipmentIcon(const mhw_equipment* equipment)
{
  if (!equipment) return &nullIcon;
  const int icon_palette = 1;

  EquipmentDB* equipmentDB = equipmentDB->GetInstance();
  mhw_equip_category category = equipment->category;
  const i32 type = equipment->type;
  const u32 id = equipment->id;
  u64 key = -1;

  switch (category) {
  case mhw_equip_category::Armor:
  case mhw_equip_category::Charm:
    key = BuildArmorKey(equipmentDB, type, id, icon_palette);
    break;

  case mhw_equip_category::Weapon:
    key = BuildWeaponKey(equipmentDB, type, id, icon_palette);
    break;

  case mhw_equip_category::Kinsect:
    key = BuildKinsectKey(equipmentDB, type, id, icon_palette);
    break;
  }

  QIcon* icon = CreateIcon(key, &nullIcon);
  return icon;
}

u64 BitmapDB::BuildArmorKey(EquipmentDB* equipmentDB, const i32 type, const u32 id, const u16 palette)
{
  am_dat_entry* armor = equipmentDB->GetEntryArmor(type, id);
  if (!armor) return -1;

  const u32 icon_id = 16 * 14 + armor->equip_slot;
  const u16 icon_color = armor->rarity;

  return BuildKey(icon_id, palette, icon_color);
}

u64 BitmapDB::BuildWeaponKey(EquipmentDB* equipmentDB, const i32 type, const u32 id, const u16 palette)
{
  wp_dat_entry* wp_dat = equipmentDB->GetEntryWeaponMelee(type, id);
  wp_dat_g_entry* wp_dat_g = equipmentDB->GetEntryWeaponRanged(type, id);
  if (!wp_dat && !wp_dat_g) return -1;

  u32 icon_id = 16 * 15 + wp_index_id_map[type];
  u16 icon_color = 0;
  if (wp_dat) icon_color = wp_dat->rarity;
  if (wp_dat_g) icon_color = wp_dat_g->rarity;

  return BuildKey(icon_id, palette, icon_color);
}

u64 BitmapDB::BuildKinsectKey(EquipmentDB* equipmentDB, const i32 type, const u32 id, const u16 palette) {
  rod_inse_entry* kinsect = equipmentDB->GetEntryKinsect(type, id);
  if (!kinsect) return -1;

  const u32 icon_id = 16 * 14 + 8 + kinsect->attack_type;
  const u16 icon_color = kinsect->rarity;

  return BuildKey(icon_id, palette, icon_color);
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
    u16 icon_palette = 0;
    u16 icon_color = 0;
    DissectKey(key, &icon_id, &icon_palette, &icon_color);

    QIcon* icon = iconIterator.value();
    if (icon) {
      QList<QSize> sizes = icon->availableSizes();
      QSize size = sizes.first();
      QPixmap iconPixmap = icon->pixmap(size);
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
    u16 icon_palette = 0;
    u16 icon_color = 0;
    DissectKey(key, &icon_id, &icon_palette, &icon_color);

    QString filepath = QString("%1/item_i%2_p%3_c%4.png").arg(outPath).arg(icon_id).arg(icon_palette).arg(icon_color);
    QIcon* icon = iconIterator.value();
    bool localSuccess = (bool)icon;
    if (localSuccess) {
      QList<QSize> sizes = icon->availableSizes();
      QSize size = sizes.first();
      QPixmap iconPixmap = icon->pixmap(size);
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
      int x = (icon_id % 16) * iconWidth;
      int y = (icon_id / 16) * iconHeight;

      p.fillRect(x, y, iconWidth, iconHeight, Qt::black);
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
