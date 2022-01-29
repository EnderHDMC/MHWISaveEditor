#pragma once

#include "mhw_save.h"

mhw_equipment MHW_EQUIPMENT_EMPTY = {
  /* .sort_index = */ 0,
  /* .serial_item_category = */ -1,
  /* .type = */ -1,
  /* .id = */ 0,
  /* .level = */ 0,
  /* .points = */ 0,
  /* .decos = */ {-1, -1, -1},
  /* .bowgun_mods = */ {-1, -1, -1, -1, -1},
  /* .augments = */ {0, 0, 0},
  /* .unknown0 = */ {0xffffffff, 0xffffffff},
  /* .pendant = */ -1,
  /* .extra_slots = */ 0,
  /* .attack_increase = */ 0,
  /* .affinity_increase = */ 0,
  /* .defense_increase = */ 0,
  /* .slot_upgrade = */ 0,
  /* .health_regen = */ 0,
  /* .element_status_effect_up = */ 0,
  /* .unknown1 = */ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  /* .custom_upgrades = */ {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  /* .base = */ -1,
  /* .parts = */ -1,
  /* .awakens = */ {0, 0, 0, 0, 0},
  /* .awakens_checksum = */ 0
};
