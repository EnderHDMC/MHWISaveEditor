#pragma once
#include "types.h"

enum mhw_gender : u32
{
  Male   = 0,
  Female = 1
};

enum mhw_eyelash_length : u8
{
  Short   = 0,
  Average = 1,
  Long    = 2
};

enum mhw_palico_voice_type : u8
{
  Type1 = 0,
  Type2 = 1,
  Type3 = 2
};

enum mhw_palico_voice_pitch : u8
{
  MediumPitch = 0,
  LowPitch    = 1,
  HighPitch   = 2
};

enum mhw_sound_device : u8
{
  TV          = 0,
  HomeTheatre = 1,
  Headphones  = 2
};

enum mhw_dynamic_range : u8
{
  WideRange   = 0,
  MidRange    = 1,
  NarrowRange = 2
};

enum mhw_toggle : u8
{
  On  = 0,
  Off = 1
};

enum mhw_bool : u8
{
  BoolFalse = 0,
  BoolTrue = 1
};

enum mhw_language : u8
{
  Japanese           = 0,
  English            = 1,
  French             = 2,
  Spanish            = 3,
  Dutch              = 4,
  Italian            = 5,
  Korean             = 6,
  TraditionalChinese = 7,
  SimplifiedChinese  = 8,
  Russian            = 10,
  Polish             = 11,
  PortugueseBrazil   = 21,
  Arabic             = 22
};

enum mhw_voice_language : u8
{
  Normal        = 0,
  MonsterHunter = 1
};

enum mhw_text_size : u8
{
  Default = 0,
  Large   = 1
};

enum mhw_confirm_button : u8
{
  ax = 0,
  bo = 1
};

enum mhw_gamepad_glyphs : u8
{
  XBOX        = 0,
  Playstation = 1,
  OtherXBOX   = 2
};

enum mhw_equip_category : i32
{
  Empty   = -1,
  Armor   = 0,
  Weapon  = 1,
  Charm   = 2,
  Tool    = 3,
  Kinsect = 4
};

enum mhw_handler_outfit : u8
{
  O_The_Handlers_Outfit     = 0,
  O_Guildmarms_Outfit       = 1,
  O_Astera_3_Star_Chef_Coat = 2,
  O_Chun_Li_Costume         = 3,
  O_Busy_Bee_Dress          = 4,
  O_Sunshine_Pareo          = 5,
  O_Mischievous_Dress       = 6,
  O_Winter_Spirit_Coat      = 7,
  O_Friendly_Felyne         = 8,
  O_Winter_Outfit           = 9,
  O_Kokoto_Gals_Costume     = 10,
  O_Terrifying_Tyrant       = 11,
  O_Graceful_ShortDress     = 12,
  O_Techno_Handler          = 13,
  O_Rose_Vestido            = 14,
  O_Festive_Samba           = 15,
  O_Cute_Demoness           = 16
};

enum mhw_monster_index : i32
{
  I_None                =  -1, I_Anjanath             =   0, I_Rathalos            =   1,
  I_Aptonoth            =   2, I_Jagras               =   3, I_Zorah_Magdaros      =   4,
  I_Mosswine            =   5, I_Gajau                =   6, I_Great_Jagras        =   7,
  I_Kestodon_Male       =   8, I_Rathian              =   9, I_Pink_Rathian        =  10,
  I_Azure_Rathalos      =  11, I_Diablos              =  12, I_Black_Diablos       =  13,
  I_Kirin               =  14, I_Behemoth             =  15, I_Kushala_Daora       =  16,
  I_Lunastra            =  17, I_Teostra              =  18, I_Lavasioth           =  19,
  I_Deviljho            =  20, I_Barroth              =  21, I_Uragaan             =  22,
  I_Leshen              =  23, I_Pukei_Pukei          =  24, I_Nergigante          =  25,
  I_Xeno_Jiiva          =  26, I_Kulu_Ya_Ku           =  27, I_Tzitzi_Ya_Ku        =  28,
  I_Jyaratodus          =  29, I_Tobi_Kadachi         =  30, I_Paolumu             =  31,
  I_Legiana             =  32, I_Great_Girros         =  33, I_Odogaron            =  34,
  I_Radobaan            =  35, I_Vaal_Hazak           =  36, I_Dodogama            =  37,
  I_Kulve_Taroth        =  38, I_Bazelgeuse           =  39, I_Apceros             =  40,
  I_KelbiA              =  41, I_KelbiB               =  42, I_Hornetaur           =  43,
  I_Vespoid             =  44, I_Mernos               =  45, I_Kestodon_Female     =  46,
  I_Raphinos            =  47, I_Shamos               =  48, I_Barnos              =  49,
  I_Girros              =  50, I_Ancient_Leshen       =  51, I_Gastodon            =  52,
  I_Noios               =  53, I_Gajalaka             =  56, I_Tigrex              =  61,
  I_Nargacuga           =  62, I_Barioth              =  63, I_Savage_Deviljho     =  64,
  I_Brachydios          =  65, I_Glavenus             =  66, I_Acidic_Glavenus     =  67,
  I_Fulgur_Anjanath     =  68, I_Coral_Pukei_Pukei    =  69, I_Ruiner_Nergigante   =  70,
  I_Viper_Tobi_Kadachi  =  71, I_Nightshade_Paolumu   =  72, I_Shrieking_Legiana   =  73,
  I_Ebony_Odogaron      =  74, I_Blackveil_Vaal_Hazak =  75, I_Seething_Bazelgeuse =  76,
  I_Beotodus            =  77, I_Banbaro              =  78, I_Velkhana            =  79,
  I_Namielle            =  80, I_Shara_Ishvalda       =  81, I_Popo                =  82,
  I_Anteka              =  83, I_Wulg                 =  84, I_Cortos              =  85,
  I_Boaboa              =  86, I_Alatreon             =  87, I_Gold_Rathian        =  88,
  I_Silver_Rathalos     =  89, I_Yian_Garuga          =  90, I_Rajang              =  91,
  I_Furious_Rajang      =  92, I_Brute_Tigrex         =  93, I_Zinogre             =  94,
  I_Stygian_Zinogre     =  95, I_Raging_Brachydios    =  96, I_Safi_jiiva          =  97,
  I_Scarred_Yian_Garuga =  99, I_Frostfang_Barioth    = 100, I_Fatalis             = 101
};

enum mhw_achievement : i32
{
  BASE_CONQUEROR_OF_THE_NEW_WORLD    =  0, BASE_WELCOME_TO_THE_NEW_WORLD         =  1,
  BASE_NOTHING_STOPS_THIS_COMMISSION =  2, BASE_DEFENDER_OF_ASTERA               =  3,
  BASE_INTO_THE_DEEP                 =  4, BASE_DEATH_BEGETS_LIFE                =  5,
  BASE_THE_EMPRESS_OF_THE_HIGHLANDS  =  6, BASE_ONE_SHALL_STAND_ONE_SHALL_FALL   =  7,
  BASE_THE_SAPPHIRE_STAR             =  8, BASE_THE_HUNTERS_LIFE_FOR_ME          =  9,
  BASE_AN_INQUISITIVE_MIND           = 10, BASE_THE_FRANCHISE_HUNTER             = 11,
  BASE_STEP_INTO_THE_ARENA           = 12, BASE_NOWHERE_TO_GO_BUT_UP             = 13,
  BASE_NEW_WORLD_SETTLER             = 14, BASE_THE_ART_OF_CAMOUFLAGE            = 15,
  BASE_ANGLING_FOR_A_BITE            = 16, BASE_MMM_SO_TASTY                     = 17,
  BASE_THE_BIGGER_THEY_ARE           = 18, BASE_A_LIVING_FOSSIL                  = 19,
  BASE_SNUGGLES_FOR_ALL              = 20, BASE_BRISTLES_FOR_ALL                 = 21,
  BASE_RAINBOW_BRIGHT                = 22, BASE_COMMISSIONED_WORK                = 23,
  BASE_BOURGEOIS_HUNTER              = 24, BASE_IMPREGNABLE_DEFENSE              = 25,
  BASE_POWER_IS_EVERYTHING           = 26, BASE_MOVIN_ON_UP                      = 27,
  BASE_FIRST_FRIENDS                 = 28, BASE_BOSOM_BUDDIES                    = 29,
  BASE_MONSTER_PHD                   = 30, BASE_TEMPER_TEMPER                    = 31,
  BASE_INDOMITABLE                   = 32, BASE_MINIATURE_CROWN                  = 33,
  BASE_MINIATURE_CROWN_COLLECTOR     = 34, BASE_MINIATURE_CROWN_MASTER           = 35,
  BASE_GIANT_CROWN                   = 36, BASE_GIANT_CROWN_COLLECTOR            = 37,
  BASE_GIANT_CROWN_MASTER            = 38, BASE_CAPTURE_NOVICE                   = 39,
  BASE_CAPTURE_PRO                   = 40, BASE_ELDERSLAYER                      = 41,
  BASE_MONSTER_SLAYER                = 42, BASE_MONSTER_HUNTER                   = 43,
  BASE_HELP                          = 44, BASE_I_AM_THE_REINFORCEMENTS          = 45,
  BASE_HUNTERS_UNITED                = 46, BASE_HUNTERS_UNITED_FOREVER           = 47,
  BASE_SPREADING_THE_WORD            = 48, BASE_ESTABLISHED_HUNTER               = 49,
  ICE_CONQUEROR_OF_THE_HINTERLANDS   = 50, ICE_THE_BEGINNING_OF_A_NEW_EXPEDITION = 51,
  ICE_TIME_TO_GET_SERIOUS            = 52, ICE_THE_ELUSIVE_ELDER_DRAGON          = 53,
  ICE_INDOMITABLE_SPIRIT             = 54, ICE_THE_OLD_EVERWYRM                  = 55,
  ICE_AN_END_AND_A_BEGINNING         = 56, ICE_TO_THE_LAND_OF_DISCOVERIES        = 57,
  ICE_EVOLVING_ECOLOGY               = 58, ICE_IN_SEARCH_OF_RARE_MATERIALS       = 59,
  ICE_INSATIABLE_INVESTIGATOR        = 60, ICE_FATES_CONCLUSION                  = 61,
  ICE_MASTER_EXPLORER                = 62, ICE_SOURCE_OF_RELAXATION              = 63,
  ICE_CLUTCH_CLAW_NEOPHYTE           = 64, ICE_GOLDEN_GLEAM                      = 65,
  ICE_FRIENDLY_POINTER               = 66, ICE_SWEET_MELODY                      = 67,
  ICE_SUBMERGED_MYSTERY              = 68, ICE_CELESTIAL_ILLUSION                = 69,
  ICE_DEFT_DIGGER                    = 70, ICE_CREATURES_OF_THE_EARTH            = 71,
  ICE_UNWAVERING_DEFENSE             = 72, ICE_DEVASTATING_OFFENSE               = 73,
  ICE_PERSONAL_TREASURE              = 74, ICE_FIRST_RIDE                        = 75,
  ICE_EXPERIENCED_RIDER              = 76, ICE_FLEDGLING_COLLECTOR               = 77,
  ICE_VETERAN_COLLECTOR              = 78, ICE_ULTIMATE_COLLECTOR                = 79,
  ICE_REMODELER                      = 80, ICE_INTERIOR_DECORATOR                = 81,
  ICE_ARCHITECTURAL_ARTIST           = 82, ICE_EAGER_ENGINEER                    = 83,
  ICE_SKILLED_STEAMWORKER            = 84, ICE_ANOTHER_MINIATURE_CROWN           = 85,
  ICE_ANOTHER_GIANT_CROWN            = 86, ICE_FLEDGLING_OBSERVER                = 87,
  ICE_OUTSTANDING_OBSERVER           = 88, ICE_HELPFUL_HUNTER                    = 89,
  ICE_MASTER_OF_MASTERS              = 90, ICE_MONSTER_MASTER                    = 91,
  ICE_TRUE_MINIATURE_CROWN_COLLECTOR = 92, ICE_TRUE_LARGE_CROWN_COLLECTOR        = 93,
  ICE_THE_TRUE_HUNT_BEGINS           = 94, ICE_HUNTER_PRODIGY                    = 95,
  ICE_MASTER_CAPTURER                = 96, ICE_MASTER_SLAYER                     = 97,
  ICE_CONFRONTING_THE_UNKNOWN        = 98, ICE_SEEN_IT_ALL                       = 99,
};
