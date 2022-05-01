#pragma once

#include <stdlib.h>
#include <string.h>

#include "../mhw_save.h"

#pragma pack(push, 1)
struct uct_header
{
  u32 magic;
  u32 extension;
  u32 version;
};

struct uct_color_info
{
  u32 unknown0;
  mhw_color color;
  f32 unknown1[5];
};

struct uct_color_pallete
{
  u32 color_count;
  uct_color_info color_info[];
};

struct uct_meta
{
  uct_header* header;

  uct_color_pallete* item;
  uct_color_pallete* rarity;
  uct_color_pallete* unknown0;
  uct_color_pallete* unknown1;
  uct_color_pallete* palletes[4];
};
#pragma pack(pop)

static void FreeMetaFile(uct_meta* meta) {
  if (!meta) return;

  if (meta->header) free(meta->header);

  meta->header = nullptr;
  meta->item = nullptr;
  meta->rarity = nullptr;
  meta->unknown0 = nullptr;
  meta->unknown1 = nullptr;

  meta->palletes[0] = meta->item;
  meta->palletes[1] = meta->rarity;
  meta->palletes[2] = meta->unknown0;
  meta->palletes[3] = meta->unknown1;
}

static bool InitMetaFile(uct_meta* meta, uct_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (uct_header*)base;
  meta->item = (uct_color_pallete*)(base + sizeof(uct_header));
  meta->rarity = (uct_color_pallete*)((u8*)meta->item + sizeof(uct_color_pallete::color_count) + meta->item->color_count * sizeof(uct_color_pallete::color_info[0]));
  meta->unknown0 = (uct_color_pallete*)((u8*)meta->rarity + sizeof(uct_color_pallete::color_count) + meta->rarity->color_count * sizeof(uct_color_pallete::color_info[0]));
  meta->unknown1 = (uct_color_pallete*)((u8*)meta->unknown0 + sizeof(uct_color_pallete::color_count) + meta->unknown0->color_count * sizeof(uct_color_pallete::color_info[0]));

  meta->palletes[0] = meta->item;
  meta->palletes[1] = meta->rarity;
  meta->palletes[2] = meta->unknown0;
  meta->palletes[3] = meta->unknown1;

  return true;
}
