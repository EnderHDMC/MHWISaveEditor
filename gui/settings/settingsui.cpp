#include "settingsui.h"
#include "ui_settingsui.h"

SettingsUI::SettingsUI(QWidget* parent)
  : QDialog(parent)
{
  ui = new Ui::SettingsUI();
  ui->setupUi(this);

  settings = settings->GetInstance();
  int uiLanguageIndex = Settings::LanguageEnumToIndex(settings->GetUiLanguage());
  int itemLanguageIndex = Settings::LanguageEnumToIndex(settings->GetItemLanguage());

  ui->chkAutoBackups->setChecked(settings->GetDoAutoBackups());
  ui->spnMaxBackups->setValue(settings->GetMaxBackups());

  ui->chkMatrixMode->setChecked(settings->GetMatrixMode());
  ui->chkShowUnobtainable->setChecked(settings->GetShowUnobtainable());
  ui->chkSearchAllTabsIncludeItemPouch->setChecked(settings->GetItemPouchSearchAll());

  ui->cmbUILanguage->setCurrentIndex(uiLanguageIndex);
  ui->cmbItemLanguage->setCurrentIndex(itemLanguageIndex);

  ui->chkDarkMode->setChecked(settings->GetDarkMode());

  init = true;
}

SettingsUI::~SettingsUI()
{
  delete ui;
}

void SettingsUI::SetAutoBackups(int checked)
{
  if (!init) return;
  bool autoBackups = checked;
  settings->SetDoAutoBackups(autoBackups);
}

void SettingsUI::SetMaxBackups(int value)
{
  if (!init) return;
  int maxBackups = value;
  settings->SetMaxBackups(maxBackups);
}

void SettingsUI::SetMatrixMode(int checked)
{
  if (!init) return;
  bool matrixMode = checked;
  settings->SetMatrixMode(matrixMode);
}

void SettingsUI::SetShowUnobtainableItems(int checked)
{
  if (!init) return;
  bool showUnobtainable = checked;
  settings->SetShowUnobtainable(showUnobtainable);
}

void SettingsUI::SetIncludePouchInSearch(int checked)
{
  if (!init) return;
  bool itemPouchSearchAll = checked;
  settings->SetItemPouchSearchAll(itemPouchSearchAll);
}

void SettingsUI::SetDarkMode(int checked)
{
  if (!init) return;
  bool darkMode = checked;
  settings->SetDarkMode(darkMode);
}

void SettingsUI::SetItemLanguage(int index)
{
  if (!init) return;
  mhw_language itemLanguage = Settings::LanguageIndexToEnum(index);
  settings->SetItemLanguage(itemLanguage);
}

void SettingsUI::SetUILanguage(int index)
{
  if (!init) return;
  mhw_language uiLanguage = Settings::LanguageIndexToEnum(index);
  settings->SetUiLanguage(uiLanguage);
}
