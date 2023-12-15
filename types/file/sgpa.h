#pragma once

#include <stdlib.h>

#include "../types.h"

#pragma pack(push, 1)
typedef struct {
  u32 id;
  u32 index;
  u32 size;
  u32 skill1;
  u32 skill1_level;
  u32 skill2;
  u32 skill2_level;
} sgpa_entry;

typedef struct {
  u32 magic1;
  u16 magic2;
  u32 entry_count;
} sgpa_header;

typedef struct {
  sgpa_header* header;
  sgpa_entry* gems;
} sgpa_meta;
#pragma pack(pop)

static bool InitMetaFile(sgpa_meta* meta, sgpa_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  if (!base) return false;

  meta->header = (sgpa_header*)base;
  meta->gems = (sgpa_entry*)(base + sizeof(sgpa_header));
  return true;
}

static void FreeMetaFile(sgpa_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);
  meta->header = nullptr;
  meta->gems = nullptr;
}
