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

void LimitedUnlocks::Load(mhw_save_raw* mhwSave, int mhwSaveSlot)
{
  SaveLoader::Load(mhwSave, mhwSaveSlot);
  loading = true;

  u32 assassinHoodUnlocked = (mhwSave->save.section3.Saves[mhwSaveSlot].tool_unlocks[0] >> ASSASSIN_HOOD_INDEX) & 0x01;
  u32 assassinHoodUpgraded = mhwSave->save.section3.Saves[mhwSaveSlot].tools[ASSASSIN_HOOD_INDEX].level;
  ui->chkAssassinHoodUnlock->setChecked(assassinHoodUnlocked);
  ui->chkAssassinHoodUpgrade->setChecked(assassinHoodUpgraded);

  loading = false;
}

void LimitedUnlocks::UnlockAssassinHood(int checked)
{
  if (!mhwSave) return;
  if (loading) return;
  checked = checked != 0;

  u8 n = ASSASSIN_HOOD_INDEX;
  u32 unlock = mhwSave->save.section3.Saves[mhwSaveSlot].tool_unlocks[0];
  unlock = (unlock & ~(1UL << n)) | (checked << n);
  mhwSave->save.section3.Saves[mhwSaveSlot].tool_unlocks[0] = unlock;
}


void LimitedUnlocks::UpgradeAssassinHood(int checked)
{
  if (!mhwSave) return;
  if (loading) return;
  checked = checked != 0;

  mhwSave->save.section3.Saves[mhwSaveSlot].tools[ASSASSIN_HOOD_INDEX].level = checked;
}

void LimitedUnlocks::GiveArtemisGear()
{
  if (!mhwSave) return;

  for (int type = 0; type < 5; type++)
  {
    mhw_equipment* equipment = FindEquipment(mhwSaveIB, mhwSaveSlot, -1, 0);
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
