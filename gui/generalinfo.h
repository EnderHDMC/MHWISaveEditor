#pragma once

#include <QWidget>
#include "../types/mhw_save.h"

namespace Ui { class GeneralInfo; };

class GeneralInfo : public QWidget
{
    Q_OBJECT

public slots:
  void ChangeSteamID();
  void ResetEditVouchers();

public:
    GeneralInfo(QWidget *parent = Q_NULLPTR);
    ~GeneralInfo();

    void Load(mhw_save_raw* mhwSave, int saveslot);

private:
    Ui::GeneralInfo *ui;

    mhw_save_raw* mhwSave = nullptr;
    int saveslot = 0;
};
