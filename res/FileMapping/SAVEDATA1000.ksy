meta:
  id: mhw
  endian: le

seq:
  - id: header
    type: mhw_save_header
    doc: "Offset: 0"
  - id: section_offsets
    type: mhw_section_offsets
    doc: "Offset: 64"
  - id: section0
    type: mhw_section
    doc: "Offset: 96"
  - id: section1
    type: mhw_section1
    doc: "Offset: 3145840"
  - id: section2
    type: mhw_section
    doc: "Offset: 3149952"
  - id: section3
    type: mhw_section3
    doc: "Offset: 3150024"

types:
  mhw_save_header:
    seq:
      - id: magic
        contents: [0x01,0x00,0x00,0x00]
      - id: unknown0
        type: u4
      - id: unknown1
        type: u4
      - id: hash
        size: 20
      - id: data_size
        type: u8
        enum: mhw_data_size
      - id: steam_id
        type: u8
      - id: padding
        size: 16

  mhw_section_offsets:
    seq:
      - id: section_offsets
        type: u8
        repeat: expr
        repeat-expr: 4

  mhw_section_header:
    seq:
      - id: signature
        type: u4
      - id: unknown0
        type: u4
      - id: size
        type: u8

  mhw_section:
    seq:
      - id: header
        type: mhw_section_header
      - id: data
        size: header.size

  mhw_section1:
    seq:
      - id: header
        type: mhw_section_header
      - id: unknown0
        size: 56
      - id: character_edit_voucher
        type: u1
      - id: unknown1
        size: 1096
      - id: character_edit_voucher_free
        type: u1
      - id: unknown2
        size: 11
      - id: palico_edit_voucher
        type: u1
      - id: unknown3
        size: 2930

  mhw_section3:
    seq:
      - id: header
        type: mhw_section_header
      - id: saves
        type: mhw_save_slot
        repeat: expr
        repeat-expr: 3
      - id: data
        size: 1724360

  mhw_save_slot:
    seq:
      - id: unknown0
        type: u4
      - id: hunter
        type: mhw_hunter
      - id: appearance
        type: mhw_character_appearance
      - id: unknown1
        size: 382
      - id: palico_appearance
        type: mhw_palico_appearance
      - id: guild_card
        type: mhw_guild_card
      - id: collected_guild_card
        type: mhw_guild_card
        repeat: expr
        repeat-expr: 100
      - id: unknown2
        size: 209447
      - id: item_loadouts
        size: 142200
      - id: item_pouch
        type: mhw_item_pouch
      - id: storage
        type: mhw_storage
      - id: equipment
        size: 315000
      - id: unknown3
        size: 285483
      - id: palico_name
        type: strz
        size: 64
        encoding: UTF-8
      - id: unknown4
        size: 379053
      - id: hash_table
        size: 512

  mhw_hunter:
    seq:
      - id: name
        type: strz
        size: 64
        encoding: UTF-8
      - id: hunter_rank
        type: u4
      - id: master_rank
        type: u4
      - id: zeni
        type: u4
      - id: research_points
        type: u4
      - id: hunter_rank_xp
        type: u4
      - id: master_rank_xp
        type: u4
      - id: playtime
        type: u4

  mhw_character_appearance:
    seq:
      - id: unknown0
        type: u4
      - id: makeup
        type: mhw_character_makeup
        repeat: expr
        repeat-expr: 3
        
      - id: left_eye_color
        type: mhw_color
      - id: right_eye_color
        type: mhw_color
      - id: eyebrow_color
        type: mhw_color
      - id: facial_hair_color
        type: mhw_color
      
      - id: eye_width
        type: u1
      - id: eye_height
        type: u1
      - id: skin_color_x
        type: u1
      - id: skin_color_y
        type: u1
      - id: age
        type: u1
      - id: wrinkles
        type: u1
      - id: nose_height
        type: u1
      - id: mouth_height
        type: u1
      - id: gender
        type: u4
        enum: mhw_gender
      - id: brow_type
        type: u1
      - id: face_type
        type: u1
      - id: eye_type
        type: u1
      - id: nose_type
        type: u1
      - id: mouth_type
        type: u1
      - id: eyebrow_type
        type: u1
      - id: eyelash_length
        type: u1
        enum: mhw_eyelash_length
      - id: facial_hair_type
        type: u1
      - id: zero0
        type: u4
        
      - id: hair_color
        type: mhw_color
      - id: clothing_color
        type: mhw_color
      
      - id: hair_type
        type: u2
      - id: clothing_type
        type: u1
      - id: voice
        type: u1
      - id: expression
        type: u4

  mhw_character_makeup:
    seq:
      - id: color
        type: mhw_color
      - id: pos
        type: mhw_pos
      - id: size
        type: mhw_dimension
      - id: glossy
        type: f4
      - id: metallic
        type: f4
      - id: luminescent
        type: u4
      - id: type
        type: u4

  mhw_palico_appearance:
    seq:
      - id: pattern_color
        type: mhw_color
        repeat: expr
        repeat-expr: 3
      - id: fur_color
        type: mhw_color
      - id: left_eye_color
        type: mhw_color
      - id: right_eye_color
        type: mhw_color
      - id: clothing_color
        type: mhw_color
      
      - id: fur_length
        type: f4
      - id: fur_thickness
        type: f4
      - id: pattern_type
        type: u1
      - id: eye_type
        type: u1
      - id: ear_type
        type: u1
      - id: tail_type
        type: u1
      - id: voice_type
        type: u1
        enum: mhw_palico_voice_type
      - id: voice_pitch
        type: u1
        enum: mhw_palico_voice_pitch
      - id: unknown0
        type: u2
    
  mhw_guild_card:
    seq:
      - id: steam_id
        type: u8
      - id: created
        type: u8
      - id: unknown0
        type: u1
      - id: hunter_rank
        type: u4
      - id: playtime
        type: u4
      - id: last_update
        type: u8
      - id: unknown1
        type: u4
      
      - id: name
        type: strz
        size: 64
        encoding: UTF-8
      - id: primary_group
        type: strz
        size: 54
        encoding: UTF-8
      - id: unknown2
        size: 16
      
      - id: appearance
        type: mhw_character_appearance
      
      - id: unknown3
        size: 212
      
      - id: palico_name
        type: strz
        size: 64
        encoding: UTF-8
      - id: palico_rank
        type: u4
        
      - id: unknown4
        size: 194
      - id: low_rank_usage
        type: mhw_weapon_usage
      - id: high_rank_usage
        type: mhw_weapon_usage
      - id: investigation_usage
        type: mhw_weapon_usage
      - id: master_rank_usage
        type: mhw_weapon_usage
      - id: guiding_lands_usage
        type: mhw_weapon_usage
      
      - id: pose_id
        type: u1
      - id: expression_id
        type: u1
      - id: background_id
        type: u1
      - id: sticker_id
        type: u1
      
      - id: greeting
        type: strz
        size: 256
        encoding: UTF-8
      - id: title
        type: strz
        size: 256
        encoding: UTF-8
      
        # TODO: Expand Unknown5, since the some of the structure is known
        # 5454 + 864 = 6318
        # Skip 5454 // Unknown
        # Monsters: (864)
        # Skip 2 * 96 // u16 * MaxMonsterCount = Captured monsters
        # Skip 2 * 96 // u16 * MaxMonsterCount = Slayed monsters
        # Skip 2 * 96 // u16 * MaxMonsterCount = largest
        # Skip 2 * 96 // u16 * MaxMonsterCount = smallest
        # Skip 1 * 96 // u8 * MaxMonsterCount = researchLevel
      - id: unknown5
        size: 6318

  mhw_item_pouch:
    seq:
      - id: unknown0
        size: 16
      - id: items
        type: mhw_item_slot
        repeat: expr
        repeat-expr: 24
      - id: ammo
        type: mhw_item_slot
        repeat: expr
        repeat-expr: 16
      - id: unknown1
        size: 296
  
  mhw_storage:
    seq:
      - id: items
        type: mhw_item_slot
        repeat: expr
        repeat-expr: 200
      - id: ammo
        type: mhw_item_slot
        repeat: expr
        repeat-expr: 200
      - id: materials
        type: mhw_item_slot
        repeat: expr
        repeat-expr: 1250
      - id: decorations
        type: mhw_item_slot
        repeat: expr
        repeat-expr: 500
  
  mhw_item_slot:
    seq:
      - id: id
        type: u4
      - id: amount
        type: u4

  mhw_weapon_usage:
    seq:
      - id: greatsword
        type: u2
      - id: longsword
        type: u2
      - id: sword_and_shield
        type: u2
      - id: dual_blades
        type: u2
      - id: hammer
        type: u2
      - id: hunting_horn
        type: u2
      - id: lance
        type: u2
      - id: gunlance
        type: u2
      - id: switch_axe
        type: u2
      - id: charge_blade
        type: u2
      - id: insect_glaive
        type: u2
      - id: light_bowgun
        type: u2
      - id: heavy_bowgun
        type: u2
      - id: bow
        type: u2

  mhw_color:
    seq:
      - id: r
        type: u1
      - id: g
        type: u1
      - id: b
        type: u1
      - id: a
        type: u1

  mhw_pos:
    seq:
      - id: x
        type: f4
      - id: y
        type: f4

  mhw_dimension:
    seq:
      - id: width
        type: f4
      - id: height
        type: f4

enums:
  mhw_data_size:
    9438368: mhw
    11284640: mhwib

  mhw_gender:
    0: male
    1: female

  mhw_eyelash_length:
    0: short
    1: average
    2: long

  mhw_palico_voice_type:
    0: type1
    1: type2
    2: type3

  mhw_palico_voice_pitch:
    0: medium
    1: low
    2: high