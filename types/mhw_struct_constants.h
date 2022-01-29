#pragma once

#include "mhw_save.h"

const mhw_equipment MHW_EQUIPMENT_EMPTY = {
  .sort_index = 0,
  .serial_item_category = -1,
  .type = -1,
  .id = 0,
  .level = 0,
  .points = 0,
  .decos = {-1, -1, -1},
  .bowgun_mods = {-1, -1, -1, -1, -1},
  .augments = {0, 0, 0},
  .unknown0 = {0xffffffff, 0xffffffff},
  .pendant = -1,
  .extra_slots = 0,
  .attack_increase = 0,
  .affinity_increase = 0,
  .defense_increase = 0,
  .slot_upgrade = 0,
  .health_regen = 0,
  .element_status_effect_up = 0,
  .unknown1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  .custom_upgrades = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  .base = -1,
  .parts = -1,
  .awakens = {0, 0, 0, 0, 0},
  .awakened_checksum = 0
};

const mhw_layered_loadout MHW_LAYERED_LOADOUT_EMPTY = {
  .slot_id = 0,
  .name = {},
  .layered = {
    .head = -1,
    .torso = -1,
    .arms = -1,
    .coil = -1,
    .feet = -1,
    .unused = -1,
    .pigments = {
      .bitmask_custom_color = 0,
      .bitmask_rainbow = 0,
      .head = {0, 0, 0, 0},
      .torso = {0, 0, 0, 0},
      .arms = {0, 0, 0, 0},
      .coil = {0, 0, 0, 0},
      .feet = {0, 0, 0, 0},
      .whole = {0, 0, 0, 0},
    }
  }
};
