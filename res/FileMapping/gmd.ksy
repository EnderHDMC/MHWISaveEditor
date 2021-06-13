meta:
  id: mhw
  endian: le

seq:
  - id: header
    contents: [G,M,D,0]
  - id: version
    type: u4
    enum: version
  - id: language
    type: u4
  - id: unknown0
    type: u4
  - id: unknown1
    type: u4
  - id: key_count
    type: u4
  - id: string_count
    type: u4
    doc: Usually same as key_count
  - id: key_block_size
    type: u4
  - id: string_block_size
    type: u4
  - id: filename_length
    type: u4
  - id: filename
    type: strz
    size: filename_length + 1
    encoding: UTF-8
  - id: info_table
    type: info_table_entry
    repeat: expr
    repeat-expr: key_count
  - id: unknown2
    size: 0x800
  - id: name_table
    type: strz
    encoding: UTF-8
    repeat: expr
    repeat-expr: string_count
  - id: string_table
    type: strz
    encoding: UTF-8
    repeat: expr
    repeat-expr: string_count
  - id: test
    type: u8
    repeat: eos

types:
  info_table_entry:
    seq:
      - id: string_index
        type: u4
      - id: hash1
        type: u4
      - id: hash2
        type: u4
      - id: unknown0
        type: u2
      - id: unknown1
        type: u2
        
      - id: key_offset
        type: u4
      - id: unknown2
        type: u4
      - id: unknown3
        type: u4
      - id: unknown4
        type: u4

enums:
  version:
    0x00010201: mhw3u_eu
    0x00010101: mh3g_jp
    0x00010302: mhw