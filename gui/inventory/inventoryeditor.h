#pragma once

#include <QWidget>

#include "inventoryeditortab.h"

namespace Ui { class InventoryEditor; };

class InventoryEditor : public QWidget, public SaveLoader
{
  Q_OBJECT

public slots:
  void SearchIndexChange(int index);
  void ItemAdd();

public:
  InventoryEditor(QWidget* parent = Q_NULLPTR);
  ~InventoryEditor();

  // Inherited via SaveLoader
  virtual void Load(mhw_ib_save* mhwSave, mhw_ps4_save* ps4, int slotIndex) override;
  virtual void LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB) override;

private:
  Ui::InventoryEditor* ui;

  QList<InventoryEditorTab*> editorTabs;
};
