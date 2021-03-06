#pragma once

#include <string>

#include "types.h"

// File failure strings
const std::string ITM_FAILURE = "ITM FAILURE";
const std::string GMD_FAILURE = "GMD FAILURE";

// Steam registry items
const std::string STEAM_INSTALL_REG = R"(HKEY_CURRENT_USER\SOFTWARE\Valve\Steam)";
const std::string STEAM_PROCESS_REG = R"(HKEY_CURRENT_USER\Software\Valve\Steam\ActiveProcess)";
const std::string STEAM_USERS_REG   = R"(HKEY_CURRENT_USER\Software\Valve\Steam\Users)";

// Game registry items
const std::string MHW_INSTALL_REG   = R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 582010)";

// Game keys
const std::string KEY_SAVEDATA1000  = "xieZjoe#P2134-3zmaghgpqoe0z8$3azeq";
const std::string KEY_ROD_INSE      = "SFghFQVFJycHnypExurPwut98ZZq1cwvm7lpDpASeP4biRhstQgULzlb";

const std::string MHW_ID          = "582010";
const std::string MHW_SAVE_NAME   = "SAVEDATA1000";
const std::string MHW_FOLDER_NAME = "Monster Hunter World";
const std::string MHW_EXE_NAME    = "MonsterHunterWorld.exe";

const std::string MHW_EXE_REL_BACKUP  = "../../MonsterHunterWorld.exe";
const std::string MHW_SAVE_NAME_REL   = "/SAVEDATA1000.bin";

const u32 MHW_IB_SAVE_SIZE = 11284704;


const i32 LAYERED_YUKUMO_ID = 471;
const i32 LAYERED_SILVER_KNIGHT_ID = 472;
const i32 LAYERED_SAMURAI_ID = 190;

const u8 ASSASSIN_HOOD_INDEX = 19;
const u32 ARTEMIS_GEAR_ID = 725;

// 390000 obtained from:
// https://steamcommunity.com/app/582010/discussions/0/3974929535247630028/
// and verifed in game disassembly.
// ~330000 is sufficient to get/keep all regions at 65000 xp.
// But the game caps the total level at this value.
// 390000 = 65000 * 6
const u32 GUIDING_LANDS_LEVEL_UNCAP = 390000;
const u32 GUIDING_LANDS_XP_PER_LEVEL = 10000;
const u32 GUIDING_LANDS_XP_MAX = 65000;
