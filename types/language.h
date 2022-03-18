#pragma once

#include <QString>

#include "editor_enums.h"

class Language
{
public:
  // Needs to match up with the settings UI options.
  static const game_language Languages(u32 index) {
    game_language languages[] = {
    game_language::GameLanguage,
    game_language::Japanese,
    game_language::English,
    game_language::French,
    game_language::Italian,
    game_language::Dutch,
    game_language::Spanish,
    game_language::PortugueseBrazil,
    game_language::Polish,
    game_language::Russian,
    game_language::Korean,
    game_language::TraditionalChinese,
    game_language::SimplifiedChinese,
    game_language::Arabic,
    game_language::InvalidLanguage
    };

    if (index >= COUNTOF(languages)) return game_language::InvalidLanguage;
    return languages[index];
  }

  static QString GetLanguageCode(game_language language)
  {
    QString result = "eng";
    switch (language) {
    case game_language::Japanese:            result = "jpn"; break;
    case game_language::English:             result = "eng"; break;
    case game_language::French:              result = "fre"; break;
    case game_language::Spanish:             result = "spa"; break;
    case game_language::Dutch:               result = "ger"; break;
    case game_language::Italian:             result = "ita"; break;
    case game_language::Korean:              result = "kor"; break;
    case game_language::TraditionalChinese:  result = "chT"; break;
    case game_language::SimplifiedChinese:   result = "chS"; break;
    case game_language::Russian:             result = "rus"; break;
    case game_language::Polish:              result = "pol"; break;
    case game_language::PortugueseBrazil:    result = "ptB"; break;
    case game_language::Arabic:              result = "ara"; break;

    case game_language::InvalidLanguage:     result = NULL;  break;
    case game_language::GameLanguage:        result = NULL;  break;
    case game_language::EditorUILanguage:    result = NULL;  break;
    }
    return result;
  }

  static game_language LanguageFromCode(const QString& code)
  {
    game_language result = game_language::InvalidLanguage;
    for (int i = 0; Languages(i) != game_language::InvalidLanguage; i++) {
      game_language language = Languages(i);
      QString languageCode = GetLanguageCode(language);

      if (languageCode == code) {
        result = language;
        break;
      }
    }

    return result;
  }

  static game_language LanguageIndexToEnum(int index)
  {
    return Languages(index);
  }

  static int LanguageEnumToIndex(game_language language)
  {
    int index = -1;
    for (int i = 0; (u8)Languages(i) != 0xFF; i++) {
      if (Languages(i) == language) {
        index = i;
      }
    }
    return index;
  }
};

