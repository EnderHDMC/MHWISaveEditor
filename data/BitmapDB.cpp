#include "BitmapDB.h"

BitmapDB* BitmapDB::instance = nullptr;

BitmapDB* BitmapDB::GetInstance()
{
  if (!instance) instance = new BitmapDB();
  return instance;
}

void BitmapDB::Free()
{
  for (auto x : images)
    delete x.second;

  delete(instance);
  instance = nullptr;
}

QPixmap* BitmapDB::Pixmap(QString path)
{
  QPixmap* result = nullptr;

  if (images.find(path) == images.end()) {
    qInfo("Loading %s", qUtf8Printable(path));

    result = new QPixmap(path);
    images[path] = result;
  }
  else {
    qInfo("Re-using %s", qUtf8Printable(path));

    result = (QPixmap*)images.at(path);
  }

  return result;
}
