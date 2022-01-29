#pragma once

#include <QString>

#include "../../types/mhw_save.h"

#define MHW_SAVE_CHECK (this->_mhwSave)
#define MHW_SAVE_GUARD_CHECK (!MHW_SAVE_CHECK)
#define MHW_SAVE_GUARD if MHW_SAVE_GUARD_CHECK return
#define MHW_LOADING_GUARD if (loading) return

class SaveLoader {
protected:
  mhw_save_raw* _mhwSave = nullptr;
  mhw_save_raw** mhwSavePtr = nullptr;
  mhw_ib_save* mhwSaveIB = nullptr;
  mhw_save_slot* mhwSaveSlot = nullptr;
  int _mhwSaveIndex = 0;

  QString file;
  bool loading = false;

public:
  virtual ~SaveLoader() {}

  virtual void Load(mhw_save_raw* mhwSave, int slotIndex)
  {
    loading = true;
    this->_mhwSaveIndex = slotIndex;

    this->_mhwSave = (mhwSave) ? mhwSave : nullptr;
    this->mhwSavePtr = (mhwSave) ? &this->_mhwSave : nullptr;
    this->mhwSaveIB = (mhwSave) ? &mhwSave->save : nullptr;
    this->mhwSaveSlot= (mhwSaveIB) ? &mhwSaveIB->section3.saves[_mhwSaveIndex] : nullptr;
  }

  virtual void LoadFile(const QString &file)
  {
    loading = true;
    this->file = file;
  }

  virtual void LoadSlot(int slotIndex)
  {
    loading = true;
    this->_mhwSaveIndex = slotIndex;
    this->mhwSaveSlot= (mhwSaveIB) ? &mhwSaveIB->section3.saves[_mhwSaveIndex] : nullptr;
  }

  virtual void Unload(bool freeMem = false)
  {
    loading = true;
    if (freeMem) free(this->_mhwSave);

    this->_mhwSave = nullptr;
    this->mhwSavePtr = nullptr;
    this->mhwSaveIB = nullptr;
    this->mhwSaveSlot = nullptr;
  }

  virtual void FinishLoad()
  {
    loading = false;
  }
};