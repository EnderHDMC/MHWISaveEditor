#pragma once

#include <QString>

#include "../../types/mhw_save.h"
#include "../../data/ItemDB.h"
#include "../../data/BitmapDB.h"

#define MHW_SAVE_CHECK MHW_SaveCheck()
#define MHW_SAVE_GUARD_CHECK (!MHW_SAVE_CHECK)
#define MHW_SAVE_GUARD if MHW_SAVE_GUARD_CHECK return
#define MHW_LOADING_CHECK (EditorLoading() && MHW_SAVE_CHECK)
#define MHW_LOADING_GUARD_CHECK (!MHW_LOADING_CHECK)
#define MHW_LOADING_GUARD if (MHW_LOADING_CHECK) return

class SaveLoader {
private:
  // You really don't want to set any of these directly.
  mhw_ib_save* mhwSaveIB = nullptr;
  mhw_ps4_save* mhwSavePS4 = nullptr;
  bool isPS4 = false;

  int mhwSaveIndex = 0;

  QString file;
  bool loading = false;

protected:
  ItemDB* itemDB = nullptr;
  BitmapDB* bitmapDB = nullptr;

public:
  virtual ~SaveLoader() {}

  virtual void Load(mhw_ib_save* mhwSave, mhw_ps4_save* ps4, int slotIndex)
  {
    loading = true;
    this->mhwSaveIB = mhwSave;
    this->mhwSavePS4 = ps4;

    mhw_section1* mhwSection1 = MHW_Section1();
    mhw_section3* mhwSection3 = MHW_Section3();

    this->mhwSaveIndex = slotIndex;
    if (slotIndex == -1 && mhwSaveIB)
      mhwSaveIndex = mhwSection1->last_active_slot;
  }

  void Refresh() {
    mhw_ib_save* mhwSave = MHW_SaveIB();
    mhw_ps4_save* ps4 = MHWS_SavePS4();
    int mhwSaveIndex = MHW_SaveIndex();
    
    Load(mhwSave, ps4, mhwSaveIndex);
  }

  virtual void LoadFile(const QString& file, bool isPS4)
  {
    this->file = file;
    this->isPS4 = isPS4;
  }

  virtual void LoadIndex(int index)
  {
    loading = true;
  }

  virtual void LoadSlot(int slotIndex)
  {
    loading = true;
    mhw_section3* mhwSection3 = MHW_Section3();
    this->mhwSaveIndex = slotIndex;
  }

  virtual void Unload(bool freeMem = false)
  {
    loading = true;
    if (freeMem) free(this->mhwSaveIB);

    this->mhwSaveIB = nullptr;
  }

  virtual void FinishLoad()
  {
    loading = false;
  }

  virtual void LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB) {
    this->itemDB = itemDB;
    this->bitmapDB = bitmapDB;
  }

  // All of these should be preceeded by a guard.
  // They all assert that the values are set.
  bool MHW_SaveCheck() { return mhwSaveIB; }
  mhw_ib_save* MHW_SaveIB() { Q_ASSERT(mhwSaveIB); return mhwSaveIB; }
  mhw_section1* MHW_Section1() { Q_ASSERT(mhwSaveIB); return &mhwSaveIB->section1; }
  mhw_section3* MHW_Section3() { Q_ASSERT(mhwSaveIB); return &mhwSaveIB->section3; }

  mhw_save_slot* MHW_SaveSlot() { return mhwSaveIB ? &mhwSaveIB->section3.saves[mhwSaveIndex] : nullptr; }
  int MHW_SaveIndex() { Q_ASSERT(mhwSaveIndex >= 0 && mhwSaveIndex <= 2); return mhwSaveIndex; }

  // These are safe to call,
  // but null/false/empty values should still be handled by the caller.
  // You can do a guard check after calling these as well.
  mhw_ib_save* MHWS_SaveIB() { return mhwSaveIB; }
  mhw_ps4_save* MHWS_SavePS4() { return mhwSavePS4; }

  mhw_ib_save** MHWS_SaveIBPtr() { return &mhwSaveIB; }
  mhw_ps4_save** MHWS_SavePS4Ptr() { return &mhwSavePS4; }
  bool MHWS_IsPS4() { return isPS4; }

  QString EditorFile() { return file; }
  bool EditorLoading() { return loading; }
};
