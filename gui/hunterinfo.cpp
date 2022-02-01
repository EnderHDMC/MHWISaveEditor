#include "hunterinfo.h"
#include "ui_hunterinfo.h"

#include "common/StringByteLengthValidator.h"
#include "common/Notification.h"
#include "../utility/settype.h"

#include <QSignalMapper>

HunterInfo::HunterInfo(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::HunterInfo();
  ui->setupUi(this);

  StringByteLengthValidator* hunterNameValidator = new StringByteLengthValidator(sizeof(str64), "Hunter Name");
  StringByteLengthValidator* palicoNameValidator = new StringByteLengthValidator(sizeof(str64), "Palico Name");
  ui->edtHunterName->setValidator(hunterNameValidator);
  ui->edtPalicoName->setValidator(palicoNameValidator);

  regionIndexMapping.insert(ui->spnAncientForestGL, 0);
  regionIndexMapping.insert(ui->spnWildspireWasteGL, 1);
  regionIndexMapping.insert(ui->spnCoralHighlandsGL, 2);
  regionIndexMapping.insert(ui->spnRottenVeilGL, 3);
  regionIndexMapping.insert(ui->spnEldersRecessGL, 4);
  regionIndexMapping.insert(ui->spnHoarfrostReachGL, 5);
}

HunterInfo::~HunterInfo()
{
  delete ui;
}

void HunterInfo::HunterNameChange(const QString& text)
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  str64* hunterName = &mhwSaveSlot->hunter.name;
  SetStr(hunterName, text);
}

void HunterInfo::PalicoNameChange(const QString& text)
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  str64* palicoName = &mhwSaveSlot->palico_name;
  SetStr(palicoName, text);
}

void HunterInfo::ZennyChange(int value)
{
  MHW_LOADING_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  u32 zeni = value;
  mhwSaveSlot->hunter.zeni = zeni;
}

void HunterInfo::ResearchPointsChange(int value)
{
  MHW_LOADING_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  u32 researchPoints = value;
  mhwSaveSlot->hunter.research_points = researchPoints;
}

void HunterInfo::SteamworksFuelChange(int value)
{
  MHW_LOADING_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  u32 steamworksFuel = value;
  mhwSaveSlot->steamworks_stored_fuel = value;
}

void HunterInfo::RegionalLevelGLChange(int value)
{
  MHW_LOADING_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  QSpinBox* senderSpin = qobject_cast<QSpinBox*>(sender());
  int index = regionIndexMapping.value(senderSpin, -1);

  if (index != -1) {
    u32 xp = mhwSaveSlot->guiding_lands.region_levels[index];
    u32 level = xp / GUIDING_LANDS_XP_PER_LEVEL;
    xp -= level * GUIDING_LANDS_XP_PER_LEVEL;
    level = value - 1;
    xp += level * GUIDING_LANDS_XP_PER_LEVEL;
    if (xp > GUIDING_LANDS_XP_MAX) xp = GUIDING_LANDS_XP_MAX;
    mhwSaveSlot->guiding_lands.region_levels[index] = xp;
  }
}

void HunterInfo::UncapGuidingLands()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  mhwSaveSlot->guiding_lands.level_total = GUIDING_LANDS_LEVEL_UNCAP;

  Notification* notif = notif->GetInstance();
  notif->ShowMessage("Guiding Lands levels uncapped.", 5000);
}

void HunterInfo::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  str64 hunterName = {};
  str64 palicoName = {};
  u32 zeni = mhwSaveSlot->hunter.zeni;
  u32 researchPoints = mhwSaveSlot->hunter.research_points;
  u32 steamworksFuel = mhwSaveSlot->steamworks_stored_fuel;
  u32* guidingLandsLevels = mhwSaveSlot->guiding_lands.region_levels;
  u8* guidingLandsLevelsUnlocked = mhwSaveSlot->guiding_lands.region_level_unlocked;

  strncpy_s(hunterName, sizeof(hunterName), (char*)mhwSaveSlot->hunter.name, COUNTOF(hunterName));
  strncpy_s(palicoName, sizeof(palicoName), (char*)mhwSaveSlot->palico_name, COUNTOF(palicoName));
  hunterName[COUNTOF(hunterName) - 1] = '\0';
  palicoName[COUNTOF(palicoName) - 1] = '\0';

  ui->edtHunterName->setText(hunterName);
  ui->edtPalicoName->setText(palicoName);
  ui->spnZenny->setValue(zeni);
  ui->spnResearchPoints->setValue(researchPoints);
  ui->spnSteamworksFuel->setValue(steamworksFuel);

  QMapIterator<QSpinBox*, u8> i(regionIndexMapping);
  while (i.hasNext()) {
    i.next();

    QSpinBox* spnLevelGL = i.key();
    u8 index = i.value();

    u32 level = (guidingLandsLevels[index] / GUIDING_LANDS_XP_PER_LEVEL) + 1;
    u32 maxLevel = guidingLandsLevelsUnlocked[index];
    u32 minLevel = maxLevel > 0;

    spnLevelGL->setMinimum(minLevel);
    spnLevelGL->setMaximum(maxLevel);
    spnLevelGL->setValue(level);
  }

  SaveLoader::FinishLoad();
}
