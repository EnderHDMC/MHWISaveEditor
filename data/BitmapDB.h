#pragma once

#include <QPixmap>

#include <unordered_map>

class BitmapDB
{
  static BitmapDB* instance;
  std::unordered_map<QString, QPixmap*> images;

  BitmapDB() { }

public:
  static BitmapDB* GetInstance();
  void Free();

  QPixmap* Pixmap(QString path);

};