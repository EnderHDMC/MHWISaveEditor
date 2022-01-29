#pragma once

#include <QWidget>

#include "../types/mhw_save.h"
#include "common/saveloader.h"

namespace Ui { class LimitedUnlocks; };

class LimitedUnlocks : public QWidget, public SaveLoader
{
    Q_OBJECT

public slots:
  void UnlockAssassinHood(int checked);
  void UpgradeAssassinHood(int checked);
  void UnlockLayeredArtemis(int checked);
  void UnlockLayeredBayek(int checked);
  void GiveArtemisGear();
  void GiveYukumoLoadout();
  void GiveSilverKnightLoadout();
  void GiveSamuraiLoadout();

public:
    LimitedUnlocks(QWidget *parent = Q_NULLPTR);
    ~LimitedUnlocks();

    // Inherited via SaveLoader
    virtual void Load(mhw_save_raw* mhwSave, int slotIndex) override;

private:
    Ui::LimitedUnlocks *ui;

    void GiveLayeredLoadout(i32 layered, const QString &name);
};
