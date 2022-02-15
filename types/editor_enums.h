#pragma once

#include "mhw_enums.h"

enum class item_language : i8
{
  Japanese = mhw_language::Japanese,
  English = mhw_language::English,
  French = mhw_language::French,
  Italian = mhw_language::Italian,
  Dutch = mhw_language::Dutch,
  Spanish = mhw_language::Spanish,
  PortugueseBrazil = mhw_language::PortugueseBrazil,
  Polish = mhw_language::Polish,
  Russian = mhw_language::Russian,
  Korean = mhw_language::Korean,
  TraditionalChinese = mhw_language::TraditionalChinese,
  SimplifiedChinese = mhw_language::SimplifiedChinese,
  Arabic = mhw_language::Arabic,

  InvalidLanguage = -1,
  GameLanguage = -2,
  EditorUILanguage = -3,
};
