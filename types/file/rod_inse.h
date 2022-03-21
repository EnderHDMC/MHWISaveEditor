#pragma once

#include <stdlib.h>
#include <string.h>

#include "../types.h"

#pragma pack(push, 1)
struct rod_inse_header {
  u32 magic;
  u16 version;
  u32 entry_count;
};

// Entries
struct rod_inse_entry {
  u32 index;
  u8 attack_type;
  u8 id;
  u8 tree_position_id;
  u16 base_model_id;
  u8 tree_id;
  u32 craft_cost;
  u8 rarity;
  u16 power;
  u16 speed;
  u16 heal;
  u16 element;
  u16 dust_effect;
  u8 tree_position;
  u16 equip_id;
};


struct rod_inse_meta {
  rod_inse_header* header;
  rod_inse_entry* entries;
};
#pragma pack(pop)

static void FreeMetaFile(rod_inse_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);

  meta->header = nullptr;
  meta->entries = nullptr;
}

static bool InitMetaFile(rod_inse_meta* meta, rod_inse_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (rod_inse_header*)base;
  meta->entries = (rod_inse_entry*)(base + sizeof(rod_inse_header));

  return true;
}
