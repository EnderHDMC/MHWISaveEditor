#include "generalinfo.h"
#include "ui_generalinfo.h"

#include <QInputDialog>
#include "common/Notification.h"

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
  MHW_SAVE_GUARD;
  u64 steamID = mhwSaveIB->header.steam_id;
  QString id = QString::number(steamID);

  bool ok;
  QString text = QInputDialog::getText(
    this, tr("Enter Steam ID"), tr("Steam ID:"),
    QLineEdit::Normal, id, &ok);

  if (ok && !text.isEmpty())
  {
    Notification* notification = notification->GetInstance();

    steamID = text.toULongLong(&ok);
    if (ok) {
      ui->btnSteamID->setText(text);
      mhwSaveIB->header.steam_id = steamID;

      notification->ShowMessage("Steam ID set.");
    }
    else {
      notification->ShowMessage("Invalid steam ID format.", 5000);
    }
  }
}

void GeneralInfo::ResetEditVouchers()
{
  MHW_SAVE_GUARD;

  // TODO: Map out the other edit vouchers and add them here.
  mhwSaveIB->section1.character_edit_voucher = 0;
  mhwSaveIB->section1.character_edit_voucher_free = 0;
  mhwSaveIB->section1.palico_edit_voucher = 0;

  Notification* notification = notification->GetInstance();
  notification->ShowMessage("Free edit vouchers have been reset.", 5000);
}

void GeneralInfo::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  u64 steamID = mhwSave->save.header.steam_id;
  ui->btnSteamID->setText(QString::number(steamID));

  SaveLoader::FinishLoad();
}
