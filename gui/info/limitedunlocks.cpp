#include "limitedunlocks.h"
#include "ui_limitedunlocks.h"

#include "../common/Notification.h"
#include "../../utility/mhw_save_utils.h"
#include "../../utility/settype.h"

LimitedUnlocks::LimitedUnlocks(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::LimitedUnlocks();
  ui->setupUi(this);

  layeredYukumoName = tr("Yukumo", "Name of the Yukumo layered set.");
  layeredSilverKnightName = tr("Silver Knight", "Name of the Silver Knight layered set.");
  layeredSamuraiName = tr("Samurai", "Name of the Samurai layered set.");
}

LimitedUnlocks::~LimitedUnlocks()
{
  delete ui;
}

void LimitedUnlocks::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

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
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  checked = checked != 0;
  u8 n = ASSASSIN_HOOD_INDEX;
  u32 unlock = mhwSaveSlot->tool_unlocks[0];
  unlock = (unlock & ~(1UL << n)) | (checked << n);
  mhwSaveSlot->tool_unlocks[0] = unlock;
}


void LimitedUnlocks::UpgradeAssassinHood(int checked)
{
  MHW_LOADING_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  checked = checked != 0;
  mhwSaveSlot->tools[ASSASSIN_HOOD_INDEX].level = checked;
}

void LimitedUnlocks::UnlockLayeredArtemis(int checked)
{
  MHW_LOADING_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  checked = checked != 0;
  mhwSaveSlot->progress.layered_artemis = checked & 0x1;
}

void LimitedUnlocks::UnlockLayeredBayek(int checked)
{
  MHW_LOADING_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  checked = checked != 0;
  mhwSaveSlot->progress.layered_bayek = checked & 0x1;
}

void LimitedUnlocks::GiveArtemisGear()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  int type;
  for (type = 0; type < 5; type++)
  {
    mhw_equipment* equipment = MHWSaveUtils::FindEquipment(mhwSaveSlot, -1, 0);
    if (equipment) {
      u32 sort_index = equipment->sort_index;
      memcpy_s(equipment, sizeof(mhw_equipment), &MHW_EQUIPMENT_EMPTY, sizeof(mhw_equipment));
      equipment->sort_index = sort_index;
      equipment->category = mhw_equip_category::Armor;
      equipment->type = type;
      equipment->id = ARTEMIS_GEAR_ID;
    }
    else break;
  }

  Notification* notif = notif->GetInstance();
  if (type < 5) {
    notif->ShowMessage(tr("Failed to add all equipment, not enough storage.", "Indicate that the equipment cannot be added, since the user does not have space in their equipment box."), 5000);
  }
  else {
    notif->ShowMessage(tr("Added Artemis gear.", "Indicate equipment has been successfully added."), 5000);
  }
}

void LimitedUnlocks::GiveYukumoLoadout()
{
  MHW_SAVE_GUARD;

  i32 layered_id = LAYERED_YUKUMO_ID;
  QString name = layeredYukumoName;
  GiveLayeredLoadout(layered_id, name);
}

void LimitedUnlocks::GiveSilverKnightLoadout()
{
  MHW_SAVE_GUARD;

  i32 layered_id = LAYERED_SILVER_KNIGHT_ID;
  QString name = layeredSilverKnightName;
  GiveLayeredLoadout(layered_id, name);
}

void LimitedUnlocks::GiveSamuraiLoadout()
{
  MHW_SAVE_GUARD;

  i32 layered_id = LAYERED_SAMURAI_ID;
  QString name = layeredSamuraiName;
  GiveLayeredLoadout(layered_id, name);
}

void LimitedUnlocks::GiveLayeredLoadout(i32 layered, const QString& name)
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  mhw_layered_loadout* loadout = MHWSaveUtils::FindEmptyLayeredLoadout(mhwSaveSlot);

  Notification* notif = notif->GetInstance();
  if (loadout) {
    i32 index = MHWSaveUtils::SetLayeredLoadout(loadout, layered, name);

    notif->ShowMessage(tr("Added layered loadout: '%1' at slot: %2", "Indicate a layered loadout has been added, %1 is the loadout name, %2 is the index it was added in.").arg(name).arg(index + 1), 5000);
  }
  else {
    notif->ShowMessage(tr("Failed to add layered loadout: '%1'. No empty loadouts.", "Indicate the layered loadout cannot be added, since the user has no open slots.").arg(name), 5000);
  }
}
