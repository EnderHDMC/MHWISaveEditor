#pragma once

#include <QWidget>

#include "../types/mhw_save.h"

namespace Ui { class LimitedUnlocks; };

class LimitedUnlocks : public QWidget
{
    Q_OBJECT

public slots:
  void UnlockAssassinHood(int checked);
  void UpgradeAssassinHood(int checked);
  void GiveArtemisGear();

public:
    LimitedUnlocks(QWidget *parent = Q_NULLPTR);
    ~LimitedUnlocks();

    void Load(mhw_save_raw* mhwSave, int saveslot);

private:
    Ui::LimitedUnlocks *ui;

    mhw_save_raw* mhwSave = nullptr;
    int saveslot = 0;

    bool loading = false;
};
