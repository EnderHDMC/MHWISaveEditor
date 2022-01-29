#pragma once

#include <QWidget>

#include "common/saveloader.h"

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
  void UncapGuidingLands();

public:
  HunterInfo(QWidget* parent = Q_NULLPTR);
  ~HunterInfo();

  // Inherited via SaveLoader
  virtual void Load(mhw_save_raw* mhwSave, int slotIndex) override;

private:
  Ui::HunterInfo* ui;
};
