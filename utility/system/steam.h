#pragma once

#include "registry.h"

#include "../../types/constants.h"
#include "../../types/steamid.h"

#include "../../gui/steam/steamuserselect.h"

class Steam {
private:
  static inline const QString GetActiveSteamUser() { return Registry::Query(STEAM_PROCESS_REG.c_str(), "ActiveUser").toString(); }

public:
  static inline const QStringList GetSteamUsers() { return Registry::Groups(STEAM_USERS_REG.c_str()); }

  static const QString GetSteamUser() {
    static QString selectedSteamUser = NULL;
    QString user = selectedSteamUser;
    if (user.isNull()) user = GetActiveSteamUser();
    if (user == "0") user = NULL;

    if (user.isNull()) {
      QStringList users = GetSteamUsers();
      int userCount = users.length();
      if (userCount == 1) {
        user = users.at(0);
        SteamSpecID steamID = SteamIDFrom32(user.toUInt());
        qInfo() << "Steam ID:" << steamID.full;
      }
      else if (userCount > 1) {
        SteamUserSelect userSelect = SteamUserSelect(users);
        userSelect.exec();

        u64 userId = userSelect.userId.toULongLong();
        u32 userAccountId = SteamAccountIDFromFull(userId);
        if (userId) user = QString::number(userAccountId);
      }
    }

    if (selectedSteamUser.isNull()) selectedSteamUser = user;
    return user;
  }
};
