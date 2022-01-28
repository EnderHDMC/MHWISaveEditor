#include "limitedunlocks.h"
#include "ui_limitedunlocks.h"

#include "../types/constants.h"

LimitedUnlocks::LimitedUnlocks(QWidget *parent)
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

  SaveLoader::FinishLoad();
}

void LimitedUnlocks::UnlockAssassinHood(int checked)
{
  if (!mhwSave) return;
  if (loading) return;
  checked = checked != 0;

  u8 n = ASSASSIN_HOOD_INDEX;
  u32 unlock = mhwSaveSlot->tool_unlocks[0];
  unlock = (unlock & ~(1UL << n)) | (checked << n);
  mhwSaveSlot->tool_unlocks[0] = unlock;
}


void LimitedUnlocks::UpgradeAssassinHood(int checked)
{
  if (!mhwSave) return;
  if (loading) return;
  checked = checked != 0;

  mhwSaveSlot->tools[ASSASSIN_HOOD_INDEX].level = checked;
}

void LimitedUnlocks::GiveArtemisGear()
{
  if (!mhwSave) return;

  for (int type = 0; type < 5; type++)
  {
    mhw_equipment* equipment = FindEquipment(mhwSaveSlot, -1, 0);
    if (equipment) {
      u32 sort_index = equipment->sort_index;
      memcpy_s(equipment, sizeof(mhw_equipment), B_EMPTY_EQUIPMENT, sizeof(mhw_equipment));
      equipment->sort_index = sort_index;
      equipment->serial_item_category = 0;
      equipment->type = type;
      equipment->id= 725;
    }
    else break;
  }
}
