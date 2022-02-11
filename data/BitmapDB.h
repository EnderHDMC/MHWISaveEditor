#pragma once

#include <QPainter>
#include <QIcon>
#include <QMap>

#include "ItemDB.h"

class BitmapDB
{
private:
  QImage items;
  QImage itemsMask;
  QMap<u32, QImage*> iconTints;
  QMap<u64, QIcon*> icons;

public:
  BitmapDB(ItemDB* itemDB);
  ~BitmapDB();
  void OutputIcons(const QString& path, ItemDB* itemDB);

  inline u64 BuildKey(u32 id, u32 color)
  {
    return (((u64)id) << 32) | (color & 0xFF);
  }

  QIcon* ItemIcon(itm_entry* info);

  // 0xaarrggbb
  const u32 palletes[28] = {
    0xFFEBEBEB, 0xFFE1505C, 0xFF47B267, 0xFF577BFF,
    0xFFE8C506, 0xFF9788D1, 0xFF4EC3E5, 0xFFDF9C65,
    0xFFD98B94, 0xFFDFE500, 0xFFAFAFAF, 0xFFBA954B,
    0xFF92D0C1, 0xFF3D7F3A, 0xFFAD1547, 0xFF4C4CD9,
    0xFF56379E, 0xFF909BB0, 0xFFD464A2, 0xFF685ECD,
    0xFF3257A7, 0xFF2A8D61, 0xFF9A5C45, 0xFF9EC09A,
    0xFFD7C18D, 0xFFE16D44, 0xFF86B239, 0xFFFFFFFF
  };
};