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
  mhw_save_raw* mhwSave = nullptr;
  mhw_section0* mhwSection0 = nullptr;
  mhw_section1* mhwSection1 = nullptr;
  mhw_section2* mhwSection2 = nullptr;
  mhw_section3* mhwSection3 = nullptr;

  mhw_ib_save* mhwSaveIB = nullptr;
  mhw_save_slot* mhwSaveSlot = nullptr;
  int mhwSaveIndex = 0;

  QString file;
  bool loading = false;

protected:
  ItemDB* itemDB = nullptr;
  BitmapDB* bitmapDB = nullptr;

public:
  virtual ~SaveLoader() {}

  virtual void Load(mhw_save_raw* mhwSave, int slotIndex)
  {
    loading = true;

    if (this->mhwSave != mhwSave && !this->mhwSave) {
      free(this->mhwSave);
    }
    this->mhwSave = (mhwSave) ? mhwSave : nullptr;

    this->mhwSaveIB = (this->mhwSave) ? &this->mhwSave->save : nullptr;
    this->mhwSection0 = (mhwSaveIB) ? &mhwSaveIB->section0 : nullptr;
    this->mhwSection1 = (mhwSaveIB) ? &mhwSaveIB->section1 : nullptr;
    this->mhwSection2 = (mhwSaveIB) ? &mhwSaveIB->section2 : nullptr;
    this->mhwSection3 = (mhwSaveIB) ? &mhwSaveIB->section3 : nullptr;

    this->mhwSaveIndex = slotIndex;
    if (slotIndex == -1 && mhwSaveIB)
      mhwSaveIndex = mhwSection1->last_active_slot;
    this->mhwSaveSlot = (mhwSection3) ? &mhwSection3->saves[mhwSaveIndex] : nullptr;
  }

  virtual void LoadFile(const QString& file)
  {
    this->file = file;
  }

  virtual void LoadIndex(int index)
  {
    loading = true;
  }

  virtual void LoadSlot(int slotIndex)
  {
    loading = true;
    this->mhwSaveIndex = slotIndex;
    this->mhwSaveSlot = (mhwSection3) ? &mhwSection3->saves[mhwSaveIndex] : nullptr;
  }

  virtual void Unload(bool freeMem = false)
  {
    loading = true;
    if (freeMem) free(this->mhwSave);

    this->mhwSave = nullptr;
    this->mhwSection0 = nullptr;
    this->mhwSection1 = nullptr;
    this->mhwSection2 = nullptr;
    this->mhwSection3 = nullptr;

    this->mhwSaveIB = nullptr;
    this->mhwSaveSlot = nullptr;
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
  bool MHW_SaveCheck() { return mhwSave; }
  mhw_save_raw* MHW_Save() { Q_ASSERT(mhwSave); return mhwSave; }
  mhw_section0* MHW_Section0() { Q_ASSERT(mhwSection0); return mhwSection0; }
  mhw_section1* MHW_Section1() { Q_ASSERT(mhwSection1); return mhwSection1; }
  mhw_section2* MHW_Section2() { Q_ASSERT(mhwSection2); return mhwSection2; }
  mhw_section3* MHW_Section3() { Q_ASSERT(mhwSection3); return mhwSection3; }

  mhw_ib_save* MHW_SaveIB() { Q_ASSERT(mhwSaveIB); return mhwSaveIB; }
  mhw_save_slot* MHW_SaveSlot() { Q_ASSERT(mhwSaveSlot); return mhwSaveSlot; }
  int MHW_SaveIndex() { Q_ASSERT(mhwSaveIndex >= 0 && mhwSaveIndex <= 2); return mhwSaveIndex; }

  // These are safe to call,
  // but null/false/empty values should still be handled by the caller.
  // You can do a guard check after calling these as well.
  mhw_save_raw* MHWS_Save() { return mhwSave; }

  mhw_save_raw** MHWS_SavePtr() { return &mhwSave; }

  QString EditorFile() { return file; }
  bool EditorLoading() { return loading; }
};
