#pragma once

#include "../../types/mhw_save.h"

class SaveLoader {
protected:
  mhw_save_raw* mhwSave = nullptr;
  mhw_ib_save* mhwSaveIB = nullptr;
  mhw_save_slot* mhwSaveSlot = nullptr;
  int _mhwSaveIndex = 0;

  bool loading = false;

public:
  virtual ~SaveLoader() {}

  virtual void Load(mhw_save_raw* mhwSave, int slotIndex)
  {
    loading = true;
    this->_mhwSaveIndex = slotIndex;

    this->mhwSave = (mhwSave) ? mhwSave : nullptr;
    this->mhwSaveIB = (mhwSave) ? &mhwSave->save : nullptr;
    this->mhwSaveSlot= (mhwSaveIB) ? &mhwSaveIB->section3.saves[_mhwSaveIndex] : nullptr;
  }

  virtual void FinishLoad()
  {
    loading = false;
  }
};