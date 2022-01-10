#pragma once

#include <QPainter>
#include <QIcon>
#include <QMap>

#include "ItemDB.h"

class BitmapDB
{
private:
  static BitmapDB* instance;
  QImage items;
  QImage itemsMask;
  QMap<u32, QImage*> iconTints;
  QMap<u64, QIcon*> icons;

  BitmapDB();

public:
  static BitmapDB* GetInstance();
  void Init(ItemDB* itemDB);
  void OutputIcons(const QString& path, ItemDB* itemDB);
  void Free();

  inline u64 BuildKey(u32 id, u32 color)
  {
    return (((u64)id) << 32) | color;
  }

  QIcon* ItemPixmap(u32 icon, u32 color);
  QIcon* ItemIcon(itemInfo* info);
};