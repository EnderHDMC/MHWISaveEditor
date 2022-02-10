#include "generalinfo.h"
#include "ui_generalinfo.h"

#include <QInputDialog>
#include "../common/Notification.h"

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
  mhw_ib_save* mhwSaveIB = MHW_SaveIB();

  u64 steamID = mhwSaveIB->header.steam_id;
  QString id = QString::number(steamID);

  bool ok;
  QString text = QInputDialog::getText(
    this, tr("Enter Steam ID"), tr("Steam ID:"),
    QLineEdit::Normal, id, &ok);

  if (ok && !text.isEmpty())
  {
    Notification* notif = notif->GetInstance();

    steamID = text.toULongLong(&ok);
    if (ok) {
      ui->btnSteamID->setText(text);
      mhwSaveIB->header.steam_id = steamID;

      notif->ShowMessage(tr("Steam ID set."));
    }
    else {
      notif->ShowMessage(tr("Invalid steam ID format."), 5000);
    }
  }
}

void GeneralInfo::ResetEditVouchers()
{
  MHW_SAVE_GUARD;
  mhw_section1* mhwSection1 = MHW_Section1();

  mhwSection1->character_edit_voucher = 0;
  mhwSection1->character_edit_voucher_free = 0;
  mhwSection1->character_edit_voucher_single_voucher = 0;
  mhwSection1->character_edit_voucher_two_voucher_pack = 0;
  mhwSection1->character_edit_voucher_three_voucher_pack = 0;
  mhwSection1->character_c_palico_edit_voucher_single_voucher = 0;
  mhwSection1->character_c_palico_edit_two_voucher_pack = 0;
  mhwSection1->character_c_palico_edit_three_voucher_pack = 0;

  mhwSection1->palico_edit_voucher = 0;
  mhwSection1->palico_edit_voucher_single_voucher = 0;
  mhwSection1->palico_edit_voucher_two_voucher_pack = 0;
  mhwSection1->palico_edit_voucher_three_voucher_pack = 0;
  mhwSection1->character_p_palico_edit_voucher_single_voucher = 0;
  mhwSection1->character_p_palico_edit_two_voucher_pack = 0;
  mhwSection1->character_p_palico_edit_three_voucher_pack = 0;

  Notification* notif = notif->GetInstance();
  notif->ShowMessage(tr("All character and palico edit vouchers have been reset."), 5000);
}

void GeneralInfo::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  u64 steamID = mhwSave->save.header.steam_id;
  ui->btnSteamID->setText(QString::number(steamID));

  SaveLoader::FinishLoad();
}
