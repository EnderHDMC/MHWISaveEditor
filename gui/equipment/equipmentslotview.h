#pragma once

#include <QWidget>
#include <QLabel>

#include "../../data/EquipmentDB.h"
#include "../../utility/common/saveloader.h"
#include "../../types/mhw_struct_constants.h"

namespace Ui { class EquipmentSlotView; };

class EquipmentSlotView : public QWidget, public SaveLoader
{
  Q_OBJECT

public:
  EquipmentSlotView(int index, QWidget* parent = Q_NULLPTR);
  ~EquipmentSlotView();

  // Inherited via SaveLoader
  virtual void Load(mhw_ib_save* mhwSave, int slotIndex) override;

public slots:
  void Equip();
  void Uncraft();

signals:
  void Equipped(int index);

private:
  Ui::EquipmentSlotView* ui;

  void UpdateEquipDisplay(const mhw_equipment* slot, int position, bool uncraftable, bool equipped, bool empty);

  QLabel equipmentIndex;
  int equipslot = 0;
};
