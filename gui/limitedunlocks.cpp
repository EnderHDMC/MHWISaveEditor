#include "limitedunlocks.h"
#include "ui_limitedunlocks.h"

#include "common/Notification.h"
#include "../utility/mhw_save_utils.h"
#include "../utility/settype.h"

LimitedUnlocks::LimitedUnlocks(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::LimitedUnlocks();
  ui->setupUi(this);
}

LimitedUnlocks::~LimitedUnlocks()
{
  delete ui;
}

void LimitedUnlocks::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  u32 assassinHoodUnlocked = (mhwSaveSlot->tool_unlocks[0] >> ASSASSIN_HOOD_INDEX) & 0x01;
  u32 assassinHoodUpgraded = mhwSaveSlot->tools[ASSASSIN_HOOD_INDEX].level;
  ui->chkAssassinHoodUnlock->setChecked(assassinHoodUnlocked);
  ui->chkAssassinHoodUpgrade->setChecked(assassinHoodUpgraded);

  u8 layeredArtemis = mhwSaveSlot->progress.layered_artemis;
  u8 layeredBayek = mhwSaveSlot->progress.layered_bayek;
  ui->chkLayeredArtemis->setChecked(layeredArtemis);
  ui->chkLayeredBayek->setChecked(layeredBayek);

  SaveLoader::FinishLoad();
}

void LimitedUnlocks::UnlockAssassinHood(int checked)
{
  MHW_LOADING_GUARD;
  checked = checked != 0;

  u8 n = ASSASSIN_HOOD_INDEX;
  u32 unlock = mhwSaveSlot->tool_unlocks[0];
  unlock = (unlock & ~(1UL << n)) | (checked << n);
  mhwSaveSlot->tool_unlocks[0] = unlock;
}


void LimitedUnlocks::UpgradeAssassinHood(int checked)
{
  MHW_LOADING_GUARD;
  checked = checked != 0;

  mhwSaveSlot->tools[ASSASSIN_HOOD_INDEX].level = checked;
}

void LimitedUnlocks::UnlockLayeredArtemis(int checked)
{
  MHW_LOADING_GUARD;
  checked = checked != 0;

  mhwSaveSlot->progress.layered_artemis = checked & 0x1;
}

void LimitedUnlocks::UnlockLayeredBayek(int checked)
{
  MHW_LOADING_GUARD;
  checked = checked != 0;

  mhwSaveSlot->progress.layered_bayek = checked & 0x1;
}

void LimitedUnlocks::GiveArtemisGear()
{
  MHW_SAVE_GUARD;

  int type;
  for (type = 0; type < 5; type++)
  {
    mhw_equipment* equipment = FindEquipment(mhwSaveSlot, -1, 0);
    if (equipment) {
      u32 sort_index = equipment->sort_index;
      memcpy_s(equipment, sizeof(mhw_equipment), &MHW_EQUIPMENT_EMPTY, sizeof(mhw_equipment));
      equipment->sort_index = sort_index;
      equipment->serial_item_category = 0;
      equipment->type = type;
      equipment->id = ARTEMIS_GEAR_ID;
    }
    else break;
  }

  Notification* notification = notification->GetInstance();
  if (type < 5) {
    notification->ShowMessage("Failed to add all equipment, not enough storage.", 5000);
  }
  else {
    notification->ShowMessage("Added Artemis gear.", 5000);
  }
}

void LimitedUnlocks::GiveYukumoLoadout()
{
  MHW_SAVE_GUARD;

  i32 layered_id = LAYERED_YUKUMO_ID;
  QString name = QString::fromUtf8(LAYERED_YUKUMO_NAME);
  GiveLayeredLoadout(layered_id, name);
}

void LimitedUnlocks::GiveSilverKnightLoadout()
{
  MHW_SAVE_GUARD;

  i32 layered_id = LAYERED_SILVER_KNIGHT_ID;
  QString name = QString::fromUtf8(LAYERED_SILVER_KNIGHT_NAME);
  GiveLayeredLoadout(layered_id, name);
}

void LimitedUnlocks::GiveSamuraiLoadout()
{
  MHW_SAVE_GUARD;

  i32 layered_id = LAYERED_SAMURAI_ID;
  QString name = QString::fromUtf8(LAYERED_SAMURAI_NAME);
  GiveLayeredLoadout(layered_id, name);
}

void LimitedUnlocks::GiveLayeredLoadout(i32 layered, const QString& name)
{
  mhw_layered_loadout* loadout = FindEmptyLayeredLoadout(mhwSaveSlot);

  if (loadout) {
    i32 index = SetLayeredLoadout(loadout, layered, name);

    Notification* notification = notification->GetInstance();
    notification->ShowMessage(QString("Added layered loadout: '%1' at slot: %2").arg(name).arg(index + 1), 5000);
  }
  else {
    Notification* notification = notification->GetInstance();
    notification->ShowMessage(QString("Failed to add layered loadout: '%1'. No empty loadouts.").arg(name), 5000);
  }
}
