#pragma once

#include <QPainter>
#include <QIcon>
#include <QMap>

#include "EquipmentDB.h"
#include "ItemDB.h"

class BitmapDB
{
private:
  const int iconWidth = 64;
  const int iconHeight = 64;

  uct_meta uct = {};
  QImage items;
  QImage itemsMask;
  QMap<u32, QImage*> iconTints;
  QMap<u64, QIcon*> icons;
  QIcon nullIcon = QIcon();
  QPixmap textureAtlas;
  QPixmap maskAtlas;

  const int wp_index_id_map[14] = {
    1,  // Great Sword
    0,  // Sword And Shield
    3,  // Dual Blades
    2,  // Longsword
    6,  // Hammer
    7,  // Hunting Horn
    4,  // Lance
    5,  // Gunlance
    8,  // Switch Axe
    9,  // Charge Blade
    10, // Insect Glaive
    13, // Bow
    12, // Heavy Bowgun
    11, // Light Bowgun
  };

public:
  BitmapDB(ItemDB* itemDB, EquipmentDB* equipmentDB);
  ~BitmapDB();

  inline u64 BuildKey(u32 id, u16 palette, u16 color)
  {
    return (u64)id << 32 | (u64)palette << 16 | color;
  }

  inline void DissectKey(u64 key, u32* id, u16* palette, u16* color) {
    *id = key >> 32;
    *palette = (key >> 16) & 0xffff;
    *color = key & 0xffff;
  }

  QIcon* CreateIcon(u64 key, QIcon* nullIcon);
  QIcon* ItemIcon(ItemDB* itemDB, itm_entry* info);
  u64 BuildArmorKey(EquipmentDB* equipmentDB, const i32 type, const u32 id, const u16 palette);
  u64 BuildWeaponKey(EquipmentDB* equipmentDB, const i32 type, const u32 id, const u16 palette);
  u64 BuildKinsectKey(EquipmentDB* equipmentDB, const i32 type, const u32 id, const u16 palette);
  QIcon* EquipmentIcon(const mhw_equipment* equipment);

  // Debug
  bool DebugDumpAtlas(const QString& path);
  bool DebugDumpIcons(const QString& path);
  bool DebugDumpUsedMask(const QString& path);
};
