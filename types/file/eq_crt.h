#pragma once

#include <stdlib.h>
#include <string.h>

#include "../types.h"

#pragma pack(push, 1)
struct eq_crt_header {
  u32 magic;
  u16 version;
  u32 entry_count;
};

struct eq_crt_mat {
  u16 id;
  u8 count;
};

struct eq_crt_entry {
  u8 equipment_category_raw;
  u16 equipment_index_raw;
  u16 needed_item_discover;
  i32 monster_unlock;
  u32 story_unlock;
  u32 unknown0;
  u32 item_rank;
  eq_crt_mat mats[4];

  u8 unknown1[4];
};

struct eq_crt_meta {
  eq_crt_header* header;
  eq_crt_entry* entries;
};
#pragma pack(pop)

static void FreeMetaFile(eq_crt_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);

  meta->header = nullptr;
  meta->entries = nullptr;
}

static bool InitMetaFile(eq_crt_meta* meta, eq_crt_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (eq_crt_header*)base;
  meta->entries = (eq_crt_entry*)(base + sizeof(eq_crt_header));

  return true;
}
