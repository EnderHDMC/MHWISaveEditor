#pragma once

#include <QWidget>
#include <QSpinBox>

#include "../../utility/common/saveloader.h"

namespace Ui { class HunterInfo; };

class HunterInfo : public QWidget, public SaveLoader
{
  Q_OBJECT

public slots:
  void HunterNameChange(const QString& text);
  void PalicoNameChange(const QString& text);
  void ZennyChange(int value);
  void ResearchPointsChange(int value);
  void SteamworksFuelChange(int value);
  void RegionalLevelGLChange(int value);
  void UncapGuidingLands();
  void PlaytimeChange(double value);
  void HRChange(int value);
  void MRChange(int value);

public:
  HunterInfo(QWidget* parent = Q_NULLPTR);
  ~HunterInfo();

  // Inherited via SaveLoader
  virtual void Load(mhw_save_raw* mhwSave, int slotIndex) override;

private:
  Ui::HunterInfo* ui;

  QMap<QSpinBox*, u8> regionIndexMapping;

  u32 hr_original = 0;
  u32 mr_original = 0;
};
