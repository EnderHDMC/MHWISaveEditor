#pragma once
#include "types.h"
#include "mhw_enums.h"
#include "constants.h"

#pragma pack(push, 1)
struct mhw_save_header
{
  u32 magic; // 0x01_00_00_00
  u32 unknown0;
  u32 unknown1;
  u8 hash[20];

  /// <summary>
  /// Size of file minus header
  /// Base Game Data Size = 9438368
  /// Iceborne Data Size = 11284640
  /// </summary>
  u64 data_size;

  u64 steam_id;
  u8 padding[16];
};

struct mhw_section_offsets
{
  u64 section[4];
};

struct mhw_section_header
{
  u32 signature;
  u32 unknown0;
  u64 size;
};

struct mhw_color
{
  u8 r, g, b, a;
};

struct mhw_pos
{
  f32 x, y;
};

struct mhw_dimension
{
  f32 width, height;
};

struct mhw_hunter
{
  u8 name[64];
  u32 hunter_rank;
  u32 master_rank;
  u32 zeni;
  u32 research_points;
  u32 hunter_rank_xp;
  u32 master_rank_xp;
  u32 playtime; // In seconds
};

struct mhw_character_makeup
{
  mhw_color color;
  mhw_pos pos;
  mhw_dimension size;
  f32 glossy;
  f32 metallic;
  u32 Luminescent; // New in Iceborne
  u32 Type;
};

struct mhw_character_appearance
{
  i32 unknown0; // New in Iceborne
  mhw_character_makeup makeup[3]; // 3 for iceborne, 2 for pre-iceborne

  mhw_color left_eye_color;
  mhw_color right_eye_color;
  mhw_color EyebrowColor;
  mhw_color FacialHairColor;

  u8 eye_width;
  u8 eye_height;
  u8 skin_color_x;
  u8 skin_color_y;
  u8 age;
  u8 wrinkles;
  u8 nose_height;
  u8 mouth_height;
  mhw_gender gender;
  u8 brow_type;
  u8 face_type;
  u8 eye_type;
  u8 nose_type;
  u8 mouth_type;
  u8 eyebrow_type;
  mhw_eyelash_length eyelash_length;
  u8 facial_hair_type;
  u32 zero0;

  mhw_color hair_color;
  mhw_color clothing_color;

  u16 hair_type;
  u8 clothing_type;
  u8 voice;
  u32 expression;
};

struct mhw_palico_appearance
{
  mhw_color pattern_color[3];
  mhw_color fur_color;
  mhw_color left_eye_color;
  mhw_color right_eye_color;
  mhw_color clothing_color;

  f32 fur_length;
  f32 fur_thickness;
  u8 pattern_type;
  u8 eye_type;
  u8 ear_type;
  u8 tail_type;
  mhw_palico_voice_type voice_type;
  mhw_palico_voice_pitch voice_pitch;
  u16 unknown0;
};

struct mhw_weapon_usage {
  u16 greatsword;
  u16 longsword;
  u16 sword_and_shield;
  u16 dual_blades;
  u16 hammer;
  u16 hunting_horn;
  u16 lance;
  u16 gunlance;
  u16 switch_axe;
  u16 charge_blade;
  u16 insect_glaive;
  u16 light_bowgun;
  u16 heavy_bowgun;
  u16 bow;
};

struct mhw_guild_card
{
  u64 steam_id;
  u64 created; // Timestamp
  u8 unknown0;
  u32 hunter_rank;
  u32 playtime; // Seconds
  u64 last_update; // Timestamp
  u32 unknown1;

  u8 name[64];
  u8 primary_group[54];
  u8 unknown2[16];

  mhw_character_appearance appearance;

  u8 unknown3[212];

  u8 palico_name[64];
  u32 palico_rank; // Actual rank minus 1?

  u8 unknown4[194];
  mhw_weapon_usage low_rank_usage;
  mhw_weapon_usage high_rank_usage;
  mhw_weapon_usage investigation_usage;
  mhw_weapon_usage master_rank_usage;
  mhw_weapon_usage guiding_lands_usage;
  u8 pose_id;
  u8 expression_id;
  u8 background_id;
  u8 sticker_id;

  u8 greeting[256];
  u8 title[256];

  u8 unknown5[6318];
};

struct mhw_item_slot {
  u32 id;
  u32 amount;
};

struct mhw_item_pouch {
  u8 unknown0[16];
  mhw_item_slot items[24];
  mhw_item_slot ammo[16];
  u8 unknown1[296];
};

struct mhw_storage {
  mhw_item_slot items[200];
  mhw_item_slot ammo[200];
  mhw_item_slot materials[1250];
  mhw_item_slot decorations[500];
};

struct mhw_equipment
{
  i32 sort_index;
  i32 serial_item_category;
  i32 type;
  u32 id;
  u32 level;
  u32 points;
  i32 decos[3];
  i32 bowgun_mods[5];
  u32 augments[3];
  u32 unknown0[2];
  i32 pendant;
  u8 extra_slots;
  u8 attack_increase;
  u8 affinity_increase;
  u8 defense_increase;
  u8 slot_upgrade;
  u8 health_regen;
  u8 element_status_effect_up;
  u8 unknown1[11];
  i8 custom_upgrades[12];
  i16 base;
  i16 parts;
  u16 awakens[5];
  u16 awakened_checksum;
};

struct mhw_save_slot
{
  u32 unknown0;
  mhw_hunter hunter;
  mhw_character_appearance appearance;
  u8 unknown1[382];
  mhw_palico_appearance palico_appearance;
  mhw_guild_card guild_card;
  mhw_guild_card collected_guild_card[100];
  u8 unknown2[209447];
  u8 item_loadouts[142200];
  mhw_item_pouch item_pouch;
  mhw_storage storage;
  mhw_equipment equipment[2500];
  u8 unknown_equipment[221760];
  u32 equipment_index_table[2500];
  u8 unknown4[53723];
  u8 palico_name[64];
  u8 unknown5[20898];
  u32 tool_unlocks[4];
  mhw_equipment tools[128];
  u8 unknown6[342011];
  u8 hash_table[512];
};

struct mhw_section0
{
  mhw_section_header header;
  u8 data[3145728];
};

struct mhw_section1
{
  mhw_section_header header;
  u8 unknown0[56];
  u8 character_edit_voucher;
  u8 unknown1[1096];
  u8 character_edit_voucher_free;
  u8 unknown2[11];
  u8 palico_edit_voucher;
  u8 unknown3[2930];
};

struct mhw_section2
{
  mhw_section_header header;
  u8 data[56];
};

struct mhw_section3
{
  mhw_section_header header;
  mhw_save_slot Saves[3];
  u8 data[1724360];
};

struct mhw_ib_save
{
  mhw_save_header header;
  mhw_section_offsets Offsets;

  // Section 0: content unknown
  mhw_section0 section0;
  // Section 1: content unknown
  mhw_section1 section1;
  // Section 2: content unknown
  mhw_section2 section2;
  // Section 3: contains save slots
  mhw_section3 section3;
};

union mhw_save_raw
{
  mhw_ib_save save;
  u8 data[sizeof(mhw_ib_save)];
};
#pragma pack(pop)

static bool IsBlowfishDecrypted(mhw_ib_save* save) {
  return save->header.magic == 0x00000001;
}

static mhw_item_slot* FindItemMaterial(mhw_ib_save* save, int slot, u32 item_id) {
  mhw_save_slot* save_slot = &save->section3.Saves[slot];
  mhw_storage* storage = &save_slot->storage;
  
  mhw_item_slot* result = nullptr;
  for (int i = 0; i < COUNTOF(storage->materials); i++)
  {
    mhw_item_slot* slot = &storage->materials[i];
    if (slot->id == item_id) {
      result = slot;
      break;
    }
  }

  return result;
}

static mhw_equipment* FindEquipment(mhw_ib_save* save, int slot, i32 type, u32 id) {
  mhw_save_slot* save_slot = &save->section3.Saves[slot];
  mhw_equipment* equipment = save_slot->equipment;
  u32* index_table = save_slot->equipment_index_table;
  int count = COUNTOF(save_slot->equipment);
  
  mhw_equipment* result = nullptr;
  for (int i = 0; i < count; i++)
  {
    int lookup = index_table[i];
    mhw_equipment* slot = &equipment[lookup];
    if (slot->type == type && slot->id == id) {
      result = slot;
      break;
    }
  }

  return result;
}

static void ValidateSaveFile(mhw_ib_save* save) {
  for (int slot = 0; slot < COUNTOF(save->section3.Saves); slot++)
  {
    mhw_save_slot* save_slot = &save->section3.Saves[slot];
    mhw_item_pouch* pouch = &save_slot->item_pouch;
    mhw_storage* storage = &save_slot->storage;

    for (int item_index = 0; item_index < COUNTOF(pouch->ammo); item_index++) {
      if (pouch->ammo[item_index].amount == 0) {
        pouch->ammo[item_index].id = 0;
      }
    }

    for (int item_index = 0; item_index < COUNTOF(pouch->items); item_index++) {
      if (pouch->items[item_index].amount == 0) {
        pouch->items[item_index].id = 0;
      }
    }

    for (int item_index = 0; item_index < COUNTOF(storage->ammo); item_index++) {
      if (storage->ammo[item_index].amount == 0) {
        storage->ammo[item_index].id = 0;
      }
    }

    for (int item_index = 0; item_index < COUNTOF(storage->decorations); item_index++) {
      if (storage->decorations[item_index].amount == 0) {
        storage->decorations[item_index].id = 0;
      }
    }

    for (int item_index = 0; item_index < COUNTOF(storage->items); item_index++) {
      if (storage->items[item_index].amount == 0) {
        storage->items[item_index].id = 0;
      }
    }

    for (int item_index = 0; item_index < COUNTOF(storage->materials); item_index++) {
      if (storage->materials[item_index].amount == 0) {
        storage->materials[item_index].id = 0;
      }
    }
  }
}

static_assert(sizeof(mhw_ib_save) == MHW_IB_SAVE_SIZE, "Size of MHW:IB Save is not as expected.");
static_assert(sizeof(mhw_equipment) == sizeof(B_EMPTY_EQUIPMENT), "Size of constant B_EMPTY_EQUIPMENT is invalid.");
