#pragma once

#include <stdlib.h>
#include <string.h>

#include "../types.h"

#pragma pack(push, 1)
struct wp_dat_g_header {
  u32 magic;
  u16 version;
  u32 entry_count;
};

struct wp_dat_g_entry {
  u32 index;
  i16 unknown0;
  i16 base_model_id;
  i16 part1_id;
  i16 part2_id;
  u8 unknown1;
  u8 color;
  u8 tree_id;
  u8 is_fixed_upgrade;
  u8 muzzle_type;
  u8 barrel_type;
  u8 magazine_type;
  u8 scope_type;
  u32 cost;
  u8 rarity;
  u16 damage;
  u16 defense;
  i8 affinity;
  u8 element;
  u16 element_damage;
  u8 element_hidden;
  u16 element_hidden_damage;
  u8 elderseal;
  u8 shell_type_id;
  u8 unknown2;
  u8 deviation;
  u8 slot_count;
  u8 slot1_size;
  u8 slot2_size;
  u8 slot3_size;
  u32 unknown3;
  u32 unknown4;
  u32 unknown5;
  u8 unknown6;
  u8 special_ammo_type;
  u8 tree_position;
  u16 id;
  u16 gmd_name_index;
  u16 gmd_description_index;
  u16 skill;
  u16 unknown7;
};

struct wp_dat_g_meta {
  wp_dat_g_header* header;
  wp_dat_g_entry* entries;
};
#pragma pack(pop)

static void FreeMetaFile(wp_dat_g_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);

  meta->header = nullptr;
  meta->entries = nullptr;
}

static bool InitMetaFile(wp_dat_g_meta* meta, wp_dat_g_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (wp_dat_g_header*)base;
  meta->entries = (wp_dat_g_entry*)(base + sizeof(wp_dat_g_header));

  return true;
}
