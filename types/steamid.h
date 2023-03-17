#pragma once
#include "types.h"

enum SteamUniverse : u32 {
  UniverseIndividual = 0,
  UniverseUnspecified = 0,
  UniversePublic = 1,
  UniverseBeta = 2,
  UniverseInternal = 3,
  UniverseDev = 4,
  UniverseRC = 5
};

enum SteamType : u32 {
  TypeInvalid = 0,
  TypeIndividual = 1,
  TypeMultiseat = 2,
  TypeGameServer = 3,
  TypeAnonGameServer = 4,
  TypePending = 5,
  TypeContentServer = 6,
  TypeClan = 7,
  TypeChat = 8,
  TypeP2PSuperSeeder = 9,
  TypeAnonUser = 10
};

struct AccountID {
  u32 Y : 1;
  u32 accountNumber : 31;
};

struct SteamID {
  AccountID accountID;
  u32 instance : 20;
  SteamType type : 4;
  SteamUniverse universe : 8;
};

union SteamSpecID {
  u64 full;
  SteamID id;
};

static SteamSpecID SteamIDFrom32(u32 accountID,
                          u32 instance = 1,
                          SteamType type = SteamType::TypeIndividual,
                          SteamUniverse universe = SteamUniverse::UniversePublic) {
  SteamSpecID steamID = {};
  steamID.full = accountID;
  steamID.id.instance = instance;
  steamID.id.type = type;
  steamID.id.universe = universe;

  return steamID;
}
