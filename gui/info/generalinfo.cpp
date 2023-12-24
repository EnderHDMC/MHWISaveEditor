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

  QStringList users = Paths::GetSteamUsers();
  SteamUserSelect userSelect = SteamUserSelect(users, id, true);
  bool ok = userSelect.exec() == QDialog::Accepted;

  if (ok)
  {
    Notification* notif = notif->GetInstance();

    QString text = userSelect.userId;
    steamID = text.toULongLong(&ok);
    if (ok) {
      ui->btnSteamID->setText(text);
      mhwSaveIB->header.steam_id = steamID;

      notif->ShowMessage(tr("Steam ID set.", "Notify a successful steam id change."));
    }
    else {
      notif->ShowMessage(tr("Invalid steam ID format.", "Nofify that the steam id is invalid."), 5000);
    }
  }
}

void GeneralInfo::ResetEditVouchers()
{
  MHW_SAVE_GUARD;
  mhw_section1* mhwSection1 = MHW_Section1();

  u32 charVouchers = 0;
  charVouchers += mhwSection1->character_edit_voucher; mhwSection1->character_edit_voucher = 0;
  charVouchers += mhwSection1->character_edit_voucher_free; mhwSection1->character_edit_voucher_free = 0;
  charVouchers += mhwSection1->character_edit_voucher_single_voucher; mhwSection1->character_edit_voucher_single_voucher = 0;
  charVouchers += mhwSection1->character_edit_voucher_two_voucher_pack; mhwSection1->character_edit_voucher_two_voucher_pack = 0;
  charVouchers += mhwSection1->character_edit_voucher_three_voucher_pack; mhwSection1->character_edit_voucher_three_voucher_pack = 0;
  charVouchers += mhwSection1->character_c_palico_edit_voucher_single_voucher; mhwSection1->character_c_palico_edit_voucher_single_voucher = 0;
  charVouchers += mhwSection1->character_c_palico_edit_two_voucher_pack; mhwSection1->character_c_palico_edit_two_voucher_pack = 0;
  charVouchers += mhwSection1->character_c_palico_edit_three_voucher_pack; mhwSection1->character_c_palico_edit_three_voucher_pack = 0;

  u32 palicoVouchers = 0;
  palicoVouchers += mhwSection1->palico_edit_voucher; mhwSection1->palico_edit_voucher = 0;
  palicoVouchers += mhwSection1->palico_edit_voucher_single_voucher; mhwSection1->palico_edit_voucher_single_voucher = 0;
  palicoVouchers += mhwSection1->palico_edit_voucher_two_voucher_pack; mhwSection1->palico_edit_voucher_two_voucher_pack = 0;
  palicoVouchers += mhwSection1->palico_edit_voucher_three_voucher_pack; mhwSection1->palico_edit_voucher_three_voucher_pack = 0;
  palicoVouchers += mhwSection1->character_p_palico_edit_voucher_single_voucher; mhwSection1->character_p_palico_edit_voucher_single_voucher = 0;
  palicoVouchers += mhwSection1->character_p_palico_edit_two_voucher_pack; mhwSection1->character_p_palico_edit_two_voucher_pack = 0;
  palicoVouchers += mhwSection1->character_p_palico_edit_three_voucher_pack; mhwSection1->character_p_palico_edit_three_voucher_pack = 0;

  Notification* notif = notif->GetInstance();
  notif->ShowMessage(tr("All character and palico edit vouchers have been reset.",
    "Notify that all character and palico edit vouchers have been reset."), 5000);

  qInfo().nospace() << "Reset edit vouchers: "
    << "character vouchers used = 0 <- " << charVouchers
    << ", palico vouchers used = 0 <- " << palicoVouchers;
}

void GeneralInfo::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  u64 steamID = mhwSave->save.header.steam_id;
  ui->btnSteamID->setText(QString::number(steamID));

  SaveLoader::FinishLoad();
}
