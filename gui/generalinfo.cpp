#include "generalinfo.h"
#include "ui_generalinfo.h"

#include <QInputDialog>
#include <QMessageBox>

GeneralInfo::GeneralInfo(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::GeneralInfo();
  ui->setupUi(this);
}

GeneralInfo::~GeneralInfo()
{
  delete ui;
}

void GeneralInfo::ChangeSteamID()
{
  if (!this->mhwSave) return;
  u64 steamID = mhwSave->save.header.steam_id;
  QString id = QString::number(steamID);

  bool ok;
  QString text = QInputDialog::getText(
    this, tr("Enter Steam ID"), tr("Steam ID:"),
    QLineEdit::Normal, id, &ok);

  if (ok && !text.isEmpty())
  {
    steamID = text.toULongLong(&ok);
    if (ok) {
      ui->btnSteamID->setText(text);
      mhwSave->save.header.steam_id = steamID;
    }
    else {
      QMessageBox msgBox;
      msgBox.setText("Invalid steam ID format.");
      msgBox.exec();
    }
  }
}

void GeneralInfo::ResetEditVouchers()
{
  if (!mhwSave) return;

  // TODO: Map out the other edit vouchers and add them here.
  mhwSave->save.section1.character_edit_voucher = 0;
  mhwSave->save.section1.character_edit_voucher_free = 0;
  mhwSave->save.section1.palico_edit_voucher = 0;

  QMessageBox msgBox;
  msgBox.setText("Free edit vouchers have been reset.");
  msgBox.exec();
}

void GeneralInfo::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  u64 steamID = mhwSave->save.header.steam_id;
  ui->btnSteamID->setText(QString::number(steamID));

  SaveLoader::FinishLoad();
}
