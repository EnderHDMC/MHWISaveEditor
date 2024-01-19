#pragma once

#include <stdlib.h>
#include <string.h>

#include "../types.h"

#pragma pack(push, 1)
struct gmd_info_entry {
  u32 string_index;
  u32 hash_key_2x; // signed_crc32(key_value + key_value)
  u32 hash_key_3x; // signed_crc32(key_value + key_value + key_value)
  u32 pad; // always 0xCDCDCDCD
  u64 key_offset;
  u64 bucket_index;
};

struct gmd_header {
  u32 magic;
  u32 version;
  u32 language_id;
  u8 unknown[8];
  u32 key_count;
  u32 string_count;
  u32 key_block_size;
  u32 string_block_size;
  u32 name_length;
  // u8 filename[name_length + 1]; // Assuming null byte terminated
};

struct gmd_meta {
  gmd_header* header;
  u8* filename;
  gmd_info_entry* gmd_info;
  u64 bucket_count = 256;
  u64* buckets;

  u8* first_key;
  u8* first_string;
  u8** keys;
  u8** strings;

  u8* value(u32 index) const {
    u8* str = nullptr;
    if (index < header->string_count) str = strings[index];
    return str;
  }
};
#pragma pack(pop)

static void FreeMetaFile(gmd_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);
  if (meta->keys) free(meta->keys);
  if (meta->strings) free(meta->strings);

  meta->header = nullptr;
  meta->filename = nullptr;
  meta->gmd_info = nullptr;
  meta->buckets = nullptr;
  meta->first_key = nullptr;
  meta->first_string = nullptr;

  meta->keys = nullptr;
  meta->strings = nullptr;
}

static bool InitMetaFile(gmd_meta* meta, gmd_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (gmd_header*)base;
  meta->filename = base + sizeof(gmd_header);
  meta->gmd_info = (gmd_info_entry*)(base + sizeof(gmd_header) + header->name_length + 1);
  meta->buckets = (u64*)((u8*)meta->gmd_info + sizeof(gmd_info_entry) * header->key_count);
  meta->first_key = ((u8*)meta->buckets + sizeof(u64) * meta->bucket_count);
  meta->first_string = ((u8*)meta->first_key + header->key_block_size);

  meta->keys = (u8**)malloc(sizeof(u8*) * header->key_count);
  meta->strings = (u8**)malloc(sizeof(u8*) * header->string_count);
  if (!meta->keys || !meta->strings) {
    FreeMetaFile(meta);
    return false;
  }

#pragma warning(push)
#pragma warning(disable : 6386)
  u32 key_index = 0;
  u8* key_end = meta->first_key + header->key_block_size;
  for (u8* key = meta->first_key; key < key_end && key_index < header->key_count; key++) {
    meta->keys[key_index] = key;
    key_index++;
    key += strlen((char*)key);
  }

  u32 str_index = 0;
  u8* str_end = meta->first_string + header->string_block_size;
  for (u8* str = meta->first_string; str < str_end && str_index < header->string_count; str++) {
    meta->strings[str_index] = str;
    str_index++;
    str += strlen((char*)str);
  }
#pragma warning(pop)

  if (key_index != header->key_count || str_index != header->string_count) {
    FreeMetaFile(meta);
    return false;
  }

  return true;
}
