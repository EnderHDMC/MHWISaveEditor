#pragma once

#include "../types/types.h"

static void byteswap(u8 data[], int length) {
  u8 temp = 0;
  for (int i = 0; i < length; i += 4)
  {
    temp = data[i];
    data[i] = data[i + 3];
    data[i + 3] = temp;
    
    temp = data[i + 1];
    data[i + 1] = data[i + 2];
    data[i + 2] = temp;
  }
}
