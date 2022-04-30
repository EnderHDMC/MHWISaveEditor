#pragma once
#include "types.h"

#pragma warning(push)
#pragma warning(disable: 26812)
enum mhw_gender : u32
{
  Male = 0,
  Female = 1
};

enum mhw_eyelash_length : u8
{
  Short = 0,
  Average = 1,
  Long = 2
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
  LowPitch = 1,
  HighPitch = 2
};

enum mhw_sound_device : u8
{
  TV = 0,
  HomeTheatre = 1,
  Headphones = 2
};

enum mhw_dynamic_range : u8
{
  WideRange = 0,
  MidRange = 1,
  NarrowRange = 2
};

enum mhw_toggle : u8
{
  On = 0,
  Off = 1
};

enum mhw_bool : u8
{
  BoolFalse = 0,
  BoolTrue = 1
};

enum mhw_language : u8
{
  Japanese = 0,
  English = 1,
  French = 2,
  Spanish = 3,
  Dutch = 4,
  Italian = 5,
  Korean = 6,
  TraditionalChinese = 7,
  SimplifiedChinese = 8,
  Russian = 10,
  Polish = 11,
  PortugueseBrazil = 21,
  Arabic = 22
};

enum mhw_voice_language : u8
{
  Normal = 0,
  MonsterHunter = 1
};

enum mhw_text_size : u8
{
  Default = 0,
  Large = 1
};

enum mhw_confirm_button : u8
{
  ax = 0,
  bo = 1
};

enum mhw_gamepad_glyphs : u8
{
  XBOX = 0,
  Playstation = 1,
  OtherXBOX = 2
};

enum mhw_equip_category : i32
{
  Empty = -1,
  Armor = 0,
  Weapon = 1,
  Charm = 2,

  Kinsect = 4
};

enum mhw_handler_outfit : u8
{
  O_The_Handlers_Outfit = 0,
  O_Guildmarms_Outfit = 1,
  O_Astera_3_Star_Chef_Coat = 2,
  O_Chun_Li_Costume = 3,
  O_Busy_Bee_Dress = 4,
  O_Sunshine_Pareo = 5,
  O_Mischievous_Dress = 6,
  O_Winter_Spirit_Coat = 7,
  O_Friendly_Felyne = 8,
  O_Winter_Outfit = 9,
  O_Kokoto_Gals_Costume = 10,
  O_Terrifying_Tyrant = 11,
  O_Graceful_ShortDress = 12,
  O_Techno_Handler = 13,
  O_Rose_Vestido = 14,
  O_Festive_Samba = 15,
  O_Cute_Demoness = 16
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
#pragma warning(pop)
