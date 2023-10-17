#include "IconAtlas.h"
#include "../utility/common/Settings.h"

IconAtlas::IconAtlas(const QPixmap& atlas, const QPixmap& mask, const QRect& region, QColor color) {
  this->atlas = atlas;
  this->mask = mask;
  this->region = region;
  this->color = color;
}

void IconAtlas::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
  QSize size = QSize(rect.width(), rect.height());
  QPixmap map = this->pixmap(size, mode, state);
  painter->drawPixmap(rect, map);
}

QPixmap IconAtlas::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
  Settings* settings = settings->GetInstance();
  bool matrixMode = settings->GetMatrixMode();

  QPainter::CompositionMode modeA = QPainter::CompositionMode_DestinationOut;
  QPainter::CompositionMode modeB = QPainter::CompositionMode_Multiply;
  if (matrixMode) {
    modeA = QPainter::CompositionMode_DestinationIn;
    modeB = QPainter::CompositionMode_DestinationIn;
  }

  QPixmap pix(size);
  pix.fill(Qt::transparent);

  QPainter painter(&pix);
  QRect rect(QPoint(0, 0), size);
  painter.fillRect(rect, color);
  painter.setCompositionMode(modeA);
  painter.drawPixmap(rect, mask, region);
  painter.setCompositionMode(modeB);
  painter.drawPixmap(rect, atlas, region);

  return pix;
}
QList<QSize> IconAtlas::availableSizes(QIcon::Mode mode, QIcon::State state)
{
  QSize size = region.size();
  QList<QSize> sizes = { size };
  return sizes;
}

QIconEngine* IconAtlas::clone() const {
  return new IconAtlas(atlas, mask, region, color);
}
