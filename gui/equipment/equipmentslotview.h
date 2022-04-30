#pragma once

#include <QWidget>

#include "../../data/EquipmentDB.h"
#include "../../utility/common/saveloader.h"

namespace Ui { class EquipmentSlotView; };

class EquipmentSlotView : public QWidget, public SaveLoader
{
    Q_OBJECT

public:
    EquipmentSlotView(int index, QWidget *parent = Q_NULLPTR);
    ~EquipmentSlotView();

    // Inherited via SaveLoader
    virtual void Load(mhw_save_raw* mhwSave, int slotIndex) override;

public slots:
  void Uncraft();

private:
    Ui::EquipmentSlotView *ui;

    void UpdateEquipDisplay(mhw_equipment* equipment, bool uncraftable);

    int equipslot = 0;
};
