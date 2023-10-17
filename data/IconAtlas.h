#pragma once

#include <QRect>
#include <QIcon>
#include <QIconEngine>
#include <QPainter>

class IconAtlas : public QIconEngine
{
public:
  IconAtlas(const QPixmap& atlas, const QPixmap& mask, const QRect& region, QColor color);

  void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
  QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
  QList<QSize> availableSizes(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) override;

  QIconEngine* clone() const override;

private:
  QPixmap atlas;
  QPixmap mask;
  QRect region;
  QColor color;
};
