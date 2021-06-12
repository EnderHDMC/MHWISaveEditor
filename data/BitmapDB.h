#pragma once

#include <QPixmap>
#include <QIcon>

#include <unordered_map>

class BitmapDB
{
  static BitmapDB* instance;
  std::unordered_map<QString, QIcon*> images;

  BitmapDB() { }

public:
  static BitmapDB* GetInstance();
  void Free();

  QIcon* Pixmap(QString path);

};