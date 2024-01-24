#pragma once

#include <QWidget>

#include "../../utility/common/saveloader.h"

namespace Ui { class GeneralInfo; };

class GeneralInfo : public QWidget, public SaveLoader
{
  Q_OBJECT

public slots:
  void ChangeSteamID();
  void ResetEditVouchers();

public:
  GeneralInfo(QWidget* parent = Q_NULLPTR);
  ~GeneralInfo();

  // Inherited via SaveLoader
  virtual void Load(mhw_save_raw* mhwSave, int slotIndex) override;

private:
  Ui::GeneralInfo* ui;
};
