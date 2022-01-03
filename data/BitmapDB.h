#pragma once

#include <QPixmap>
#include <QIcon>
#include <QMap>

#include "ItemDB.h"

class BitmapDB
{
private:
  static BitmapDB* instance;
  QMap<u64, QIcon*> icons;

  BitmapDB();

public:
  static BitmapDB* GetInstance();
  void Free();

  QIcon* ItemPixmap(u32 icon, u32 color);
  QIcon* LoadItemIcon(itemInfo* info);
  QIcon* ItemIcon(itemInfo* info);
};