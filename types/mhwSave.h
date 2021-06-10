#pragma once
#include "types.h"
#include "mhwAppearance.h"

#pragma pack(push, 1)
struct MHWSaveHeader
{
    u32 Magic; // 0x01_00_00_00
    u32 Unknown1;
    u32 Unknown2;
    u8 Hash[20];

    /// <remarks>
    /// Size of file minus header
    /// Base Game Data Size = 9438368
    /// Iceborne Data Size = 11284640
    /// </remarks>
    u64 DataSize;

    u64 SteamId;
    u8 Padding[16];
};

struct MHWSectionOffsets
{
    u64 Section0;
    u64 Section1;
    u64 Section2;
    u64 Section3;
};

struct MHWSectionHeader
{
    u32 Signature;
    u32 Unknown0;
    u64 SectionSize;
};

struct MHWHunter
{
    u8 HunterName[64];
    u32 HunterRank;
    u32 Zero0;
    u32 Zeni;
    u32 ResearchPoints;
    u32 HunterXp;
    u32 Zero1;
    u32 PlayTime; // In seconds
};

struct CharacterMakeup
{
    u8 Color[4];

    float PosX;
    float PosY;
    float SizeX;
    float SizeY;
    float Glossy;
    float Metallic;
    int Luminescent; // New in Iceborne
    int Type;
};

struct CharacterAppearance
{
    int Unk2; // New in Iceborne
    CharacterMakeup Makeup2;
    CharacterMakeup Makeup1;
    CharacterMakeup Makeup3; // New in Iceborne

    u8 LeftEyeColor[4];
    u8 RightEyeColor[4];
    u8 EyebrowColor[4];
    u8 FacialHairColor[4];

    u8 EyeWidth;
    u8 EyeHeight;
    u8 SkinColorX;
    u8 SkinColorY;
    u8 Age;
    u8 Wrinkles;
    u8 NoseHeight;
    u8 MouthHeight;
    Gender Gender;
    u8 BrowType;
    u8 FaceType;
    u8 EyeType;
    u8 NoseType;
    u8 MouthType;
    u8 EyebrowType;
    EyelashLength EyelashLength;
    u8 FacialHairType;
    u32 Zero1;

    u8 HairColor[4];
    u8 ClothingColor[4];

    short HairType;
    u8 ClothingType;
    u8 Voice;
    int Expression;
};

struct PalicoAppearance
{
    u8 PatternColor1[4];
    u8 PatternColor2[4];
    u8 PatternColor3[4];
    u8 FurColor[4];
    u8 LeftEyeColor[4];
    u8 RightEyeColor[4];
    u8 ClothingColor[4];

    float FurLength;
    float FurThickness;
    u8 PatternType;
    u8 EyeType;
    u8 EarType;
    u8 TailType;
    PalicoVoiceType VoiceType;
    PalicoVoicePitch VoicePitch;
    u16 Unk; // Zero?
};

struct MHWWeaponUsage {
    u16 Greatsword;
    u16 LongSword;
    u16 SwordAndShield;
    u16 DualBlades;
    u16 Hammer;
    u16 HuntingHorn;
    u16 Lance;
    u16 Gunlance;
    u16 SwitchAxe;
    u16 ChargeBlade;
    u16 InsectGlaive;
    u16 LightBowgun;
    u16 HeavyBowgun;
    u16 Bow;
};

struct MHWGuildCard
{
    u64 SteamId;
    u64 Created; // Timestamp
    u8 Unknown0;
    u32 HunterRank;
    u32 PlayTime; // Seconds
    u64 LastUpdate; // Timestamp
    u32 Unknown1;

    u8 HunterName[64];
    u8 PrimaryGroup[54];
    u8 Unknown2[16];

    CharacterAppearance Appearance;

    u8 Unknown3[212];

    u8 PalicoName[64];
    u32 PalicoRank; // Actual rank minus 1?

    u8 Unknown4[194];
    MHWWeaponUsage LowRankUsage;
    MHWWeaponUsage HighRankUsage;
    MHWWeaponUsage InvestigationUsage;
    MHWWeaponUsage MasterRankUsage;
    MHWWeaponUsage GuidingLandsUsage;
    u8 PoseID;
    u8 ExpressionID;
    u8 BackgroundID;
    u8 StickerID;

    u8 Greeting[256];
    u8 Title[256];

    //  TODO: Expand Unknown5, since the some of the structure is known
    //  5454 + 864 = 6318
    //  Skip 5454 // Unknown
    //  Monsters: (864)
    //      Skip 2 * 96 // u16 * MaxMonsterCount = Captured monsters
    //      Skip 2 * 96 // u16 * MaxMonsterCount = Slayed monsters
    //      Skip 2 * 96 // u16 * MaxMonsterCount = largest
    //      Skip 2 * 96 // u16 * MaxMonsterCount = smallest
    //      Skip 1 * 96 // u8 * MaxMonsterCount = researchLevel
    u8 Unknown5[6318];
};

struct MHWItemSlot {
    u32 id;
    u32 amount;
};

struct MHWItemPouch {
    u8 unkowna[16];
    MHWItemSlot items[24];
    MHWItemSlot ammo[16];
    u8 unkownb[296];
};

struct MHWStorage {
    MHWItemSlot items[200];
    MHWItemSlot ammo[200];
    MHWItemSlot materials[1250];
    MHWItemSlot decorations[500];
};

struct MHWSaveSlot
{
    u32 Unknown0;
    MHWHunter Hunter;
    CharacterAppearance CharacterAppearance;
    u8 Unknown1[382];
    PalicoAppearance PalicoAppearance;
    MHWGuildCard GuildCard;
    MHWGuildCard CollectedGuildCards[100];
    u8 Unknown2[209447];
    u8 ItemLoadouts[142200];
    MHWItemPouch ItemPouch;
    MHWStorage Storage;
    u8 Equipment[315000];
    u8 Unkown3[285483];
    u8 PalicoName[64];
    u8 Unknown4[379053];
    u8 HashTable[512];
};

struct MHWIBSave
{
    MHWSaveHeader Header;
    MHWSectionOffsets Offsets;

    // Section 0, 1 and 2
    u8 Unknown0[3149928];

    // Section 3
    MHWSectionHeader Section3Header;
    MHWSaveSlot Saves[3];

    // Unknown, but probably part of section 3
    u8 Unknown1[1724360];
};

union MHWSaveRaw
{
    MHWIBSave save;
    u8 data[sizeof(MHWIBSave)];
};
#pragma pack(pop)

static bool IsBlowfishDecrypted(MHWIBSave* save) {
    return save->Header.Magic == 0x00000001;
}

static_assert(sizeof(MHWIBSave) == 11284704, "Size of MHW:IB Save is not as expected.");
