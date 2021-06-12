#include "BitmapDB.h"
#include <QDir>

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

QIcon* BitmapDB::Pixmap(QString path)
{
  QIcon* result = nullptr;

  if (!QFile::exists(path)) return nullptr;

  if (images.find(path) == images.end()) {
    qInfo("Loading %s", qUtf8Printable(path));

    result = new QIcon(path);
    images[path] = result;
  }
  else {
    qInfo("Re-using %s", qUtf8Printable(path));

    result = images.at(path);
  }

  return result;
}
