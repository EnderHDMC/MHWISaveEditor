#include "hunterinfo.h"
#include "ui_hunterinfo.h"

#include "../common/Notification.h"
#include "../../utility/common/StringByteLengthValidator.h"
#include "../../utility/settype.h"

#include "../../utility/mhw_save_utils.h"

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
  Q_ASSERT(index > -1);

  if (index != -1) {
    MHWSaveUtils::SetGuidingLandsRegionLevel(mhwSaveSlot, index, value);
  }
}

void HunterInfo::UncapGuidingLands()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  u32 old_level = mhwSaveSlot->guiding_lands.level_total;
  mhwSaveSlot->guiding_lands.level_total = GUIDING_LANDS_LEVEL_UNCAP;

  Notification* notif = notif->GetInstance();
  notif->ShowMessage(tr("Guiding Lands levels uncapped, xp %1 -> %2.",
    "Notify that the Guiding Lands region levels have been uncapped. %1 is the old experience level, %2 is the the uncapped experience level.")
    .arg(old_level).arg(GUIDING_LANDS_LEVEL_UNCAP), 5000);
  qInfo().nospace() << "Guiding Lands levels uncapped, xp " << old_level << " -> " << GUIDING_LANDS_LEVEL_UNCAP;
}

void HunterInfo::PlaytimeChange(double value)
{
  u32 playtime = value;

  u32 seconds = (playtime) % 60;
  u32 minutes = (playtime / 60) % 60;
  u32 hours = (playtime / 3600);
  u32 secondsG = hours > 9999 ? 59 : seconds;
  u32 minutesG = hours > 9999 ? 59 : minutes;
  u32 hoursG = hours > 9999 ? 9999 : hours;

  QChar fill0 = '0';
  QString format = "%1:%2";
  QString minsec = format.arg(minutes, 2, 10, fill0).arg(seconds, 2, 10, fill0);
  QString minsecG = format.arg(minutesG, 2, 10, fill0).arg(secondsG, 2, 10, fill0);
  QString time = format.arg(hours).arg(minsec);
  QString timeG = format.arg(hoursG).arg(minsecG);

  QString suffix = tr(" seconds, time = %1, game = %2", "display playtime, %1 is the actual playtime, %2 is the playtime the game will show.").arg(time).arg(timeG);
  ui->spnPlaytime->setSuffix(suffix);

  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  mhwSaveSlot->hunter.playtime = playtime;
}

void HunterInfo::HRChange(int value)
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  u32 game_hr = mhwSaveSlot->hunter.hunter_rank;
  u32 rank_cap = MHWSaveUtils::HRNextLock(game_hr);
  u32 milestone = MHWSaveUtils::HRPreviousMilestone(game_hr);

  u32 hr_exp = value;
  u32 exp_rank = MHWSaveUtils::HRExpToRank(hr_exp);
  game_hr = exp_rank;

  if (game_hr > hr_original) {
    game_hr = hr_original;
  }

  QString suffix = tr(
    " exp, HR = %1, game cap = %2, game hr = %3",
    "HR Experience indicator, %1 is the converted rank, %2 is the current game cap, $3 is the game display.")
    .arg(exp_rank).arg(rank_cap).arg(game_hr);
  ui->spnHunterHR->setSuffix(suffix);

  MHW_LOADING_GUARD;
  mhwSaveSlot->hunter.hunter_rank = game_hr;
  mhwSaveSlot->hunter.hunter_rank_xp = hr_exp;
}

void HunterInfo::MRChange(int value)
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  u32 game_mr = mhwSaveSlot->hunter.master_rank;
  u32 rank_cap = MHWSaveUtils::MRNextLock(game_mr);
  u32 milestone = MHWSaveUtils::MRPreviousMilestone(game_mr);

  u32 mr_exp = value;
  u32 exp_rank = MHWSaveUtils::MRExpToRank(mr_exp);
  game_mr = exp_rank;

  if (game_mr > mr_original) {
    game_mr = mr_original;
  }

  QString suffix = tr(
    " exp, MR = %1, game cap = %2, game mr = %3",
    "MR Experience indicator, %1 is the converted rank, %2 is the current game cap, $3 is the game display.")
    .arg(exp_rank).arg(rank_cap).arg(game_mr);
  ui->spnHunterMR->setSuffix(suffix);

  MHW_LOADING_GUARD;
  mhwSaveSlot->hunter.master_rank = game_mr;
  mhwSaveSlot->hunter.master_rank_xp = mr_exp;
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
  u32 playtime = mhwSaveSlot->hunter.playtime;

  strncpy_s(hunterName, sizeof(hunterName), (char*)mhwSaveSlot->hunter.name, COUNTOF(hunterName));
  strncpy_s(palicoName, sizeof(palicoName), (char*)mhwSaveSlot->palico_name, COUNTOF(palicoName));
  hunterName[COUNTOF(hunterName) - 1] = '\0';
  palicoName[COUNTOF(palicoName) - 1] = '\0';

  ui->edtHunterName->setText(hunterName);
  ui->edtPalicoName->setText(palicoName);
  ui->spnZenny->setValue(zeni);
  ui->spnResearchPoints->setValue(researchPoints);
  ui->spnSteamworksFuel->setValue(steamworksFuel);
  ui->spnPlaytime->setValue(playtime);

  {
    hr_original = mhwSaveSlot->hunter.hunter_rank;

    u32 hr = mhwSaveSlot->hunter.hunter_rank;
    u32 hr_xp = mhwSaveSlot->hunter.hunter_rank_xp;
    u32 hr_min = MHWSaveUtils::HRPreviousMilestone(hr);
    u32 hr_min_xp = MHWSaveUtils::HRRankToEXP(hr_min);
    if (hr_min == hr) hr_min_xp = hr_xp;
    ui->spnHunterHR->setRange(0, HR_MAX_XP);
    ui->spnHunterHR->setValue(hr_xp);
    ui->spnHunterHR->setRange(hr_min_xp, HR_MAX_XP);
  }

  {
    mr_original = mhwSaveSlot->hunter.master_rank;

    u32 mr = mhwSaveSlot->hunter.master_rank;
    u32 mr_xp = mhwSaveSlot->hunter.master_rank_xp;
    u32 mr_min = MHWSaveUtils::MRPreviousMilestone(mr);
    u32 mr_min_xp = MHWSaveUtils::MRRankToEXP(mr_min);
    if (mr_min == mr) mr_min_xp = mr_xp;
    ui->spnHunterMR->setRange(0, MR_MAX_XP);
    ui->spnHunterMR->setValue(mr_xp);
    ui->spnHunterMR->setRange(mr_min_xp, MR_MAX_XP);
  }

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
