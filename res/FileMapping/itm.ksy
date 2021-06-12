meta:
  id: itm
  endian: le

seq:
  - id: header
    type: header
  - id: info
    type: item_info
    repeat: expr
    repeat-expr: header.count

types:
  header:
    seq:
      - id: magic1
        type: u4
      - id: magic2
        type: u2
      - id: count
        type: u4
  
  item_info:
    seq:
      - id: id
        type: u4
      - id: sub_type
        type: u1
      - id: type
        type: u4
      - id: rarity
        type: u1
      - id: carry_limit
        type: u1
      - id: unknown
        type: u1
      - id: sort_order
        type: u2
      - id: flags
        type: u4
      - id: icon_id
        type: u4
      - id: icon_color
        type: u2
      - id: sell_price
        type: u4
      - id: buy_price
        type: u4