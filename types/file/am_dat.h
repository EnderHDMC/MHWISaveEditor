#pragma once

#include <stdlib.h>
#include <string.h>

#include "../types.h"

#pragma pack(push, 1)
struct am_dat_header {
  u32 magic;
  u16 version;
  u32 entry_count;
};

struct am_dat_entry {
  u32 index;
  u16 sort_order;
  u8 variant;
  u16 layered_id;
  u8 type;
  u8 equip_slot;
  u16 defense;
  u16 model_id1;
  u16 model_id2;
  u16 icon_color;
  u8 icon_effect;
  u8 rarity;
  u32 cost;
  i8 res_fire;
  i8 res_water;
  i8 res_ice;
  i8 res_thunder;
  i8 res_dragon;
  u8 slot_count;
  u8 slot1_size;
  u8 slot2_size;
  u8 slot3_size;
  u16 set_skill;
  u8 set_skill_level;
  u16 hidden_skill;
  u8 hidden_skill_level;
  u16 skill1;
  u8 skill1_level;
  u16 skill2;
  u8 skill2_level;
  u16 skill3;
  u8 skill3_level;
  u32 gender;
  u16 set_group;
  u16 gmd_name_index;
  u16 gmd_description_index;
  u8 is_permanent;
};

struct am_dat_meta {
  am_dat_header* header;
  am_dat_entry* entries;
};
#pragma pack(pop)

static void FreeMetaFile(am_dat_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);

  meta->header = nullptr;
  meta->entries = nullptr;
}

static bool InitMetaFile(am_dat_meta* meta, am_dat_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (am_dat_header*)base;
  meta->entries = (am_dat_entry*)(base + sizeof(am_dat_header));

  return true;
}
