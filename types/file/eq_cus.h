#pragma once

#include <stdlib.h>
#include <string.h>

#include "../types.h"

#pragma pack(push, 1)
struct eq_cus_header {
  u32 magic;
  u16 version;
  u32 entry_count;
};

struct eq_cus_mat {
  u16 id;
  u8 count;
};

struct eq_cus_entry {
  u8 equipment_category_raw;
  u16 equipment_index_raw;
  u16 needed_item_discover;
  i32 monster_unlock;
  u32 story_unlock;

  u32 item_rank;
  eq_cus_mat mats[4];
  u16 children[4];
  u8 unknown1[4];
};

struct eq_cus_meta {
  eq_cus_header* header;
  eq_cus_entry* entries;
};
#pragma pack(pop)

static void FreeMetaFile(eq_cus_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);

  meta->header = nullptr;
  meta->entries = nullptr;
}

static bool InitMetaFile(eq_cus_meta* meta, eq_cus_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (eq_cus_header*)base;
  meta->entries = (eq_cus_entry*)(base + sizeof(eq_cus_header));

  return true;
}
