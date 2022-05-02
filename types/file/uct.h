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

struct uct_color_palette
{
  u32 color_count;
  uct_color_info color_info[];
};

struct uct_meta
{
  uct_header* header;

  uct_color_palette* item;
  uct_color_palette* rarity;
  uct_color_palette* unknown0;
  uct_color_palette* unknown1;
  uct_color_palette* palettes[4];
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

  meta->palettes[0] = meta->item;
  meta->palettes[1] = meta->rarity;
  meta->palettes[2] = meta->unknown0;
  meta->palettes[3] = meta->unknown1;
}

static bool InitMetaFile(uct_meta* meta, uct_header* header) {
  if (!meta) return false;

  u8* base = (u8*)header;
  meta->header = (uct_header*)base;
  meta->item = (uct_color_palette*)(base + sizeof(uct_header));
  meta->rarity = (uct_color_palette*)((u8*)meta->item + sizeof(uct_color_palette::color_count) + meta->item->color_count * sizeof(uct_color_palette::color_info[0]));
  meta->unknown0 = (uct_color_palette*)((u8*)meta->rarity + sizeof(uct_color_palette::color_count) + meta->rarity->color_count * sizeof(uct_color_palette::color_info[0]));
  meta->unknown1 = (uct_color_palette*)((u8*)meta->unknown0 + sizeof(uct_color_palette::color_count) + meta->unknown0->color_count * sizeof(uct_color_palette::color_info[0]));

  meta->palettes[0] = meta->item;
  meta->palettes[1] = meta->rarity;
  meta->palettes[2] = meta->unknown0;
  meta->palettes[3] = meta->unknown1;

  return true;
}
