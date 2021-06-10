#pragma once

#include <QWidget>
namespace Ui { class ItemSlotView; };

class ItemSlotView : public QWidget
{
    Q_OBJECT

public:
    ItemSlotView(QWidget *parent = Q_NULLPTR);
    ~ItemSlotView();

private:
    Ui::ItemSlotView *ui;
};
