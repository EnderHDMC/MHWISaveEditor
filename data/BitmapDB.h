#pragma once

#include <QPixmap>
#include <QIcon>

#include <unordered_map>

#include "ItemDB.h"

class BitmapDB
{
private:
  static BitmapDB* instance;
  std::unordered_map<QString, QIcon*> images;

  BitmapDB() { }

public:
  static BitmapDB* GetInstance();
  void Free();

  QIcon* Pixmap(QString path);
  QIcon* ItemIcon(itemInfo* info);
};