#!/bin/bash

DOMAP=false
if [ ! -d "map" ]; then
  mkdir map
  DOMAP=true
fi

if [ -d "noteworthy" ]; then
  rm -rf noteworthy
fi
mkdir noteworthy

declare -A noteworthy_files
noteworthy_files=(
# Armor data
[chunk/common/equip/armor.am_dat]=true

# Melee weapon data
[chunk/common/equip/c_axe.wp_dat]=true
[chunk/common/equip/g_lance.wp_dat]=true
[chunk/common/equip/hammer.wp_dat]=true
[chunk/common/equip/lance.wp_dat]=true
[chunk/common/equip/l_sword.wp_dat]=true
[chunk/common/equip/rod.wp_dat]=true
[chunk/common/equip/s_axe.wp_dat]=true
[chunk/common/equip/sword.wp_dat]=true
[chunk/common/equip/tachi.wp_dat]=true
[chunk/common/equip/whistle.wp_dat]=true
[chunk/common/equip/w_sword.wp_dat]=true

# Ranged weapon data
[chunk/common/equip/bow.wp_dat_g]=true
[chunk/common/equip/hbg.wp_dat_g]=true
[chunk/common/equip/lbg.wp_dat_g]=true

# Item data
[chunk/common/item/itemData.itm]=true

# Kinsect data
[chunk/common/equip/rod_insect.rod_inse]=true

# Color table
[chunk/common/ui/ui_colortable.uct]=true

# Item string data
[chunk/common/text/steam/item_ara.gmd]=true
[chunk/common/text/steam/item_chS.gmd]=true
[chunk/common/text/steam/item_chT.gmd]=true
[chunk/common/text/steam/item_eng.gmd]=true
[chunk/common/text/steam/item_fre.gmd]=true
[chunk/common/text/steam/item_ger.gmd]=true
[chunk/common/text/steam/item_ita.gmd]=true
[chunk/common/text/steam/item_jpn.gmd]=true
[chunk/common/text/steam/item_kor.gmd]=true
[chunk/common/text/steam/item_pol.gmd]=true
[chunk/common/text/steam/item_ptB.gmd]=true
[chunk/common/text/steam/item_rus.gmd]=true
[chunk/common/text/steam/item_spa.gmd]=true

# Armor string data
[chunk/common/text/steam/armor_ara.gmd]=true
[chunk/common/text/steam/armor_chT.gmd]=true
[chunk/common/text/steam/armor_eng.gmd]=true
[chunk/common/text/steam/armor_fre.gmd]=true
[chunk/common/text/steam/armor_ger.gmd]=true
[chunk/common/text/steam/armor_ita.gmd]=true
[chunk/common/text/steam/armor_jpn.gmd]=true
[chunk/common/text/steam/armor_kor.gmd]=true
[chunk/common/text/steam/armor_pol.gmd]=true
[chunk/common/text/steam/armor_ptB.gmd]=true
[chunk/common/text/steam/armor_rus.gmd]=true
[chunk/common/text/steam/armor_spa.gmd]=true

# Weapon string data
[chunk/common/text/steam/bow_ara.gmd]=true
[chunk/common/text/steam/bow_chS.gmd]=true
[chunk/common/text/steam/bow_chT.gmd]=true
[chunk/common/text/steam/bow_eng.gmd]=true
[chunk/common/text/steam/bow_fre.gmd]=true
[chunk/common/text/steam/bow_ger.gmd]=true
[chunk/common/text/steam/bow_ita.gmd]=true
[chunk/common/text/steam/bow_jpn.gmd]=true
[chunk/common/text/steam/bow_kor.gmd]=true
[chunk/common/text/steam/bow_pol.gmd]=true
[chunk/common/text/steam/bow_ptB.gmd]=true
[chunk/common/text/steam/bow_rus.gmd]=true
[chunk/common/text/steam/bow_spa.gmd]=true

[chunk/common/text/steam/c_axe_ara.gmd]=true
[chunk/common/text/steam/c_axe_chS.gmd]=true
[chunk/common/text/steam/c_axe_chT.gmd]=true
[chunk/common/text/steam/c_axe_eng.gmd]=true
[chunk/common/text/steam/c_axe_fre.gmd]=true
[chunk/common/text/steam/c_axe_ger.gmd]=true
[chunk/common/text/steam/c_axe_ita.gmd]=true
[chunk/common/text/steam/c_axe_jpn.gmd]=true
[chunk/common/text/steam/c_axe_kor.gmd]=true
[chunk/common/text/steam/c_axe_pol.gmd]=true
[chunk/common/text/steam/c_axe_ptB.gmd]=true
[chunk/common/text/steam/c_axe_rus.gmd]=true
[chunk/common/text/steam/c_axe_spa.gmd]=true

[chunk/common/text/steam/g_lance_ara.gmd]=true
[chunk/common/text/steam/g_lance_chS.gmd]=true
[chunk/common/text/steam/g_lance_chT.gmd]=true
[chunk/common/text/steam/g_lance_eng.gmd]=true
[chunk/common/text/steam/g_lance_fre.gmd]=true
[chunk/common/text/steam/g_lance_ger.gmd]=true
[chunk/common/text/steam/g_lance_ita.gmd]=true
[chunk/common/text/steam/g_lance_jpn.gmd]=true
[chunk/common/text/steam/g_lance_kor.gmd]=true
[chunk/common/text/steam/g_lance_pol.gmd]=true
[chunk/common/text/steam/g_lance_ptB.gmd]=true
[chunk/common/text/steam/g_lance_rus.gmd]=true
[chunk/common/text/steam/g_lance_spa.gmd]=true

[chunk/common/text/steam/hammer_ara.gmd]=true
[chunk/common/text/steam/hammer_chS.gmd]=true
[chunk/common/text/steam/hammer_chT.gmd]=true
[chunk/common/text/steam/hammer_eng.gmd]=true
[chunk/common/text/steam/hammer_fre.gmd]=true
[chunk/common/text/steam/hammer_ger.gmd]=true
[chunk/common/text/steam/hammer_ita.gmd]=true
[chunk/common/text/steam/hammer_jpn.gmd]=true
[chunk/common/text/steam/hammer_kor.gmd]=true
[chunk/common/text/steam/hammer_pol.gmd]=true
[chunk/common/text/steam/hammer_ptB.gmd]=true
[chunk/common/text/steam/hammer_rus.gmd]=true
[chunk/common/text/steam/hammer_spa.gmd]=true

[chunk/common/text/steam/hbg_ara.gmd]=true
[chunk/common/text/steam/hbg_chS.gmd]=true
[chunk/common/text/steam/hbg_chT.gmd]=true
[chunk/common/text/steam/hbg_eng.gmd]=true
[chunk/common/text/steam/hbg_fre.gmd]=true
[chunk/common/text/steam/hbg_ger.gmd]=true
[chunk/common/text/steam/hbg_ita.gmd]=true
[chunk/common/text/steam/hbg_jpn.gmd]=true
[chunk/common/text/steam/hbg_kor.gmd]=true
[chunk/common/text/steam/hbg_pol.gmd]=true
[chunk/common/text/steam/hbg_ptB.gmd]=true
[chunk/common/text/steam/hbg_rus.gmd]=true
[chunk/common/text/steam/hbg_spa.gmd]=true

[chunk/common/text/steam/lance_ara.gmd]=true
[chunk/common/text/steam/lance_chS.gmd]=true
[chunk/common/text/steam/lance_chT.gmd]=true
[chunk/common/text/steam/lance_eng.gmd]=true
[chunk/common/text/steam/lance_fre.gmd]=true
[chunk/common/text/steam/lance_ger.gmd]=true
[chunk/common/text/steam/lance_ita.gmd]=true
[chunk/common/text/steam/lance_jpn.gmd]=true
[chunk/common/text/steam/lance_kor.gmd]=true
[chunk/common/text/steam/lance_pol.gmd]=true
[chunk/common/text/steam/lance_ptB.gmd]=true
[chunk/common/text/steam/lance_rus.gmd]=true
[chunk/common/text/steam/lance_spa.gmd]=true

[chunk/common/text/steam/lbg_ara.gmd]=true
[chunk/common/text/steam/lbg_chS.gmd]=true
[chunk/common/text/steam/lbg_chT.gmd]=true
[chunk/common/text/steam/lbg_eng.gmd]=true
[chunk/common/text/steam/lbg_fre.gmd]=true
[chunk/common/text/steam/lbg_ger.gmd]=true
[chunk/common/text/steam/lbg_ita.gmd]=true
[chunk/common/text/steam/lbg_jpn.gmd]=true
[chunk/common/text/steam/lbg_kor.gmd]=true
[chunk/common/text/steam/lbg_pol.gmd]=true
[chunk/common/text/steam/lbg_ptB.gmd]=true
[chunk/common/text/steam/lbg_rus.gmd]=true
[chunk/common/text/steam/lbg_spa.gmd]=true

[chunk/common/text/steam/l_sword_ara.gmd]=true
[chunk/common/text/steam/l_sword_chS.gmd]=true
[chunk/common/text/steam/l_sword_chT.gmd]=true
[chunk/common/text/steam/l_sword_eng.gmd]=true
[chunk/common/text/steam/l_sword_fre.gmd]=true
[chunk/common/text/steam/l_sword_ger.gmd]=true
[chunk/common/text/steam/l_sword_ita.gmd]=true
[chunk/common/text/steam/l_sword_jpn.gmd]=true
[chunk/common/text/steam/l_sword_kor.gmd]=true
[chunk/common/text/steam/l_sword_pol.gmd]=true
[chunk/common/text/steam/l_sword_ptB.gmd]=true
[chunk/common/text/steam/l_sword_rus.gmd]=true
[chunk/common/text/steam/l_sword_spa.gmd]=true

[chunk/common/text/steam/rod_ara.gmd]=true
[chunk/common/text/steam/rod_chS.gmd]=true
[chunk/common/text/steam/rod_chT.gmd]=true
[chunk/common/text/steam/rod_eng.gmd]=true
[chunk/common/text/steam/rod_fre.gmd]=true
[chunk/common/text/steam/rod_ger.gmd]=true
[chunk/common/text/steam/rod_ita.gmd]=true
[chunk/common/text/steam/rod_jpn.gmd]=true
[chunk/common/text/steam/rod_kor.gmd]=true
[chunk/common/text/steam/rod_pol.gmd]=true
[chunk/common/text/steam/rod_ptB.gmd]=true
[chunk/common/text/steam/rod_rus.gmd]=true
[chunk/common/text/steam/rod_spa.gmd]=true

[chunk/common/text/steam/sword_ara.gmd]=true
[chunk/common/text/steam/sword_chS.gmd]=true
[chunk/common/text/steam/sword_chT.gmd]=true
[chunk/common/text/steam/sword_eng.gmd]=true
[chunk/common/text/steam/sword_fre.gmd]=true
[chunk/common/text/steam/sword_ger.gmd]=true
[chunk/common/text/steam/sword_ita.gmd]=true
[chunk/common/text/steam/sword_jpn.gmd]=true
[chunk/common/text/steam/sword_kor.gmd]=true
[chunk/common/text/steam/sword_pol.gmd]=true
[chunk/common/text/steam/sword_ptB.gmd]=true
[chunk/common/text/steam/sword_rus.gmd]=true
[chunk/common/text/steam/sword_spa.gmd]=true

[chunk/common/text/steam/s_axe_ara.gmd]=true
[chunk/common/text/steam/s_axe_chS.gmd]=true
[chunk/common/text/steam/s_axe_chT.gmd]=true
[chunk/common/text/steam/s_axe_eng.gmd]=true
[chunk/common/text/steam/s_axe_fre.gmd]=true
[chunk/common/text/steam/s_axe_ger.gmd]=true
[chunk/common/text/steam/s_axe_ita.gmd]=true
[chunk/common/text/steam/s_axe_jpn.gmd]=true
[chunk/common/text/steam/s_axe_kor.gmd]=true
[chunk/common/text/steam/s_axe_pol.gmd]=true
[chunk/common/text/steam/s_axe_ptB.gmd]=true
[chunk/common/text/steam/s_axe_rus.gmd]=true
[chunk/common/text/steam/s_axe_spa.gmd]=true

[chunk/common/text/steam/tachi_ara.gmd]=true
[chunk/common/text/steam/tachi_chS.gmd]=true
[chunk/common/text/steam/tachi_chT.gmd]=true
[chunk/common/text/steam/tachi_eng.gmd]=true
[chunk/common/text/steam/tachi_fre.gmd]=true
[chunk/common/text/steam/tachi_ger.gmd]=true
[chunk/common/text/steam/tachi_ita.gmd]=true
[chunk/common/text/steam/tachi_jpn.gmd]=true
[chunk/common/text/steam/tachi_kor.gmd]=true
[chunk/common/text/steam/tachi_pol.gmd]=true
[chunk/common/text/steam/tachi_ptB.gmd]=true
[chunk/common/text/steam/tachi_rus.gmd]=true
[chunk/common/text/steam/tachi_spa.gmd]=true

[chunk/common/text/steam/whistle_ara.gmd]=true
[chunk/common/text/steam/whistle_chS.gmd]=true
[chunk/common/text/steam/whistle_chT.gmd]=true
[chunk/common/text/steam/whistle_eng.gmd]=true
[chunk/common/text/steam/whistle_fre.gmd]=true
[chunk/common/text/steam/whistle_ger.gmd]=true
[chunk/common/text/steam/whistle_ita.gmd]=true
[chunk/common/text/steam/whistle_jpn.gmd]=true
[chunk/common/text/steam/whistle_kor.gmd]=true
[chunk/common/text/steam/whistle_pol.gmd]=true
[chunk/common/text/steam/whistle_ptB.gmd]=true
[chunk/common/text/steam/whistle_rus.gmd]=true
[chunk/common/text/steam/whistle_spa.gmd]=true

[chunk/common/text/steam/w_sword_ara.gmd]=true
[chunk/common/text/steam/w_sword_chS.gmd]=true
[chunk/common/text/steam/w_sword_chT.gmd]=true
[chunk/common/text/steam/w_sword_eng.gmd]=true
[chunk/common/text/steam/w_sword_fre.gmd]=true
[chunk/common/text/steam/w_sword_ger.gmd]=true
[chunk/common/text/steam/w_sword_ita.gmd]=true
[chunk/common/text/steam/w_sword_jpn.gmd]=true
[chunk/common/text/steam/w_sword_kor.gmd]=true
[chunk/common/text/steam/w_sword_pol.gmd]=true
[chunk/common/text/steam/w_sword_ptB.gmd]=true
[chunk/common/text/steam/w_sword_rus.gmd]=true
[chunk/common/text/steam/w_sword_spa.gmd]=true

# Kinsect string data
[chunk/common/text/vfont/rod_insect_ara.gmd]=true
[chunk/common/text/vfont/rod_insect_chS.gmd]=true
[chunk/common/text/vfont/rod_insect_chT.gmd]=true
[chunk/common/text/vfont/rod_insect_eng.gmd]=true
[chunk/common/text/vfont/rod_insect_fre.gmd]=true
[chunk/common/text/vfont/rod_insect_ger.gmd]=true
[chunk/common/text/vfont/rod_insect_ita.gmd]=true
[chunk/common/text/vfont/rod_insect_jpn.gmd]=true
[chunk/common/text/vfont/rod_insect_kor.gmd]=true
[chunk/common/text/vfont/rod_insect_pol.gmd]=true
[chunk/common/text/vfont/rod_insect_ptB.gmd]=true
[chunk/common/text/vfont/rod_insect_rus.gmd]=true
[chunk/common/text/vfont/rod_insect_spa.gmd]=true
)

IFS=$'\n'
find "chunk/" -type f |
while read -r FILE; do
  TARGET="${FILE##*/}"
  EXT="${FILE##*.}"
  FILEPATH="${FILE%/*}"
  MAPPATH="map/$EXT/$FILEPATH"
  MAPFILE="map/$EXT/$FILEPATH/$TARGET"

  if $DOMAP; then
    mkdir -p "$MAPPATH"
    ln "$FILE" "$MAPFILE"
  fi

  if [ "${noteworthy_files[$FILE]+abc}" ]; then
    echo "$FILE"
    mkdir -p "noteworthy/$FILEPATH"
    ln "$FILE" "noteworthy/$FILEPATH/$TARGET"
  fi
done
unset IFS
