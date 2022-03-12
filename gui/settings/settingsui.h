#pragma once

#include <QDialog>

#include "../../utility/common/Settings.h"

namespace Ui { class SettingsUI; };

class SettingsUI : public QDialog
{
  Q_OBJECT

public slots:
  // Backups
  void SetAutoBackups(int checked);
  void SetMaxBackups(int value);
  // Items
  void SetMatrixMode(int checked);
  void SetShowUnobtainableItems(int checked);
  void SetIncludePouchInSearch(int checked);
  // QOL
  void SetDarkMode(int checked);
  // Language
  void SetItemLanguage(int index);
  void SetUILanguage(int index);


public:
  SettingsUI(QWidget* parent = Q_NULLPTR);
  ~SettingsUI();

private:
  Ui::SettingsUI* ui;

  void PopulateUILanguageSelection();

  Settings* settings = nullptr;
  bool init = false;
};
