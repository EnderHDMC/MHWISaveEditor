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
  virtual void Load(mhw_ib_save* mhwSave, mhw_ps4_save* ps4, int slotIndex) override;

private:
  Ui::GeneralInfo* ui;
};
