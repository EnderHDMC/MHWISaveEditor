#pragma once

#include <QWidget>
#include "types/mhwSave.h"
namespace Ui { class ItemSlotView; };

class ItemSlotView : public QWidget
{
  Q_OBJECT

public:
  ItemSlotView(QWidget* parent = Q_NULLPTR);
  ~ItemSlotView();

  void Load(MHWSaveRaw* mhwSave, int saveslot, int slot);

private:
  Ui::ItemSlotView* ui;



  MHWSaveRaw* mhwSave = nullptr;
  int saveslot = 0;
  int slot = 0;
};
