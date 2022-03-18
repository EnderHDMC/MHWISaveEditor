#include "settingsui.h"
#include "ui_settingsui.h"

#include <QDir>
#include <QRegularExpression>

SettingsUI::SettingsUI(QWidget* parent)
  : QDialog(parent)
{
  ui = new Ui::SettingsUI();
  ui->setupUi(this);

  settings = settings->GetInstance();
  int itemLanguageIndex = Settings::LanguageEnumToIndex(settings->GetItemLanguage());

  ui->chkAutoBackups->setChecked(settings->GetDoAutoBackups());
  ui->spnMaxBackups->setValue(settings->GetMaxBackups());

  ui->chkMatrixMode->setChecked(settings->GetMatrixMode());
  ui->chkShowUnobtainable->setChecked(settings->GetShowUnobtainable());
  ui->chkSearchAllTabsIncludeItemPouch->setChecked(settings->GetItemPouchSearchAll());

  PopulateUILanguageSelection();
  QString uiLanguage = settings->GetUiLanguage();
  int index = ui->cmbUILanguage->findData(uiLanguage);
  if (index > -1) {
    ui->cmbUILanguage->setCurrentIndex(index);
  }
  ui->cmbItemLanguage->setCurrentIndex(itemLanguageIndex);

  ui->chkDarkMode->setChecked(settings->GetDarkMode());

  ui->chkShowConsole->setChecked(settings->GetShowConsole());

  ui->lblMatrixMode->setText("*" + ui->lblMatrixMode->text());
  ui->lblShowUnobtainable->setText("*" + ui->lblShowUnobtainable->text());
  ui->lblUILanguage->setText("*" + ui->lblUILanguage->text());
  ui->lblDarkMode->setText("*" + ui->lblDarkMode->text());
  ui->lblShowConsole->setText("*" + ui->lblShowConsole->text());

  init = true;
}

SettingsUI::~SettingsUI()
{
  delete ui;
}

void SettingsUI::PopulateUILanguageSelection()
{
  QDir dir(Settings::GetResourcesPath("translations/"));
  QStringList nameFilters;
  nameFilters << "*.qm";
  dir.setNameFilters(nameFilters);
  QDir::Filters filters = QDir::Filter::Files;
  QDir::SortFlags sort = QDir::SortFlag::Name;
  QFileInfoList files = dir.entryInfoList(filters, sort);
  int translationCount = files.size();

  QString appname = QCoreApplication::applicationName();
  appname = appname.replace(" ", "").toLower();
  const QRegularExpression re(QString("(.*%1_)(.*)(\\.qm)").arg(appname));

  ui->cmbUILanguage->clear();
  ui->cmbUILanguage->addItem("Default", "");
  ui->cmbUILanguage->setCurrentIndex(0);

  for (int i = 0; i < translationCount; i++)
  {
    QString trPath = files[i].absoluteFilePath();
    QString trCode = trPath;
    trCode.replace(re, "\\2");
    QLocale trLocale = QLocale(trCode);
    QString trName = trLocale.nativeLanguageName();
    QString trLanguage = QLocale::languageToString(trLocale.language());
    qInfo().noquote() << QString("Detect language: %1 (%2)").arg(trName).arg(trLanguage);
    ui->cmbUILanguage->addItem(trName, trCode);
  }
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
  game_language language = Settings::LanguageIndexToEnum(index);
  settings->SetItemLanguage(language);
}

void SettingsUI::SetUILanguage(int index)
{
  if (!init) return;
  QString uiLanguage = ui->cmbUILanguage->itemData(index).toString();
  settings->SetUiLanguage(uiLanguage);
}

void SettingsUI::SetShowConsole(int checked)
{
  if (!init) return;
  bool showConsole = checked;
  settings->SetShowConsole(showConsole);
}
