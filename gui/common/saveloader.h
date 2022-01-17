#pragma once

#include "../../types/mhw_save.h"

class SaveLoader {
protected:
  mhw_save_raw* mhwSave = nullptr;
  mhw_ib_save* mhwSaveIB = nullptr;
  int mhwSaveSlot = 0;

public:
  virtual ~SaveLoader() {}

  virtual void Load(mhw_save_raw* mhwSave, int mhwSaveSlot)
  {
    this->mhwSave = mhwSave;
    this->mhwSaveIB = (mhwSave) ? &mhwSave->save : nullptr;
    this->mhwSaveSlot = mhwSaveSlot;
  }
};