#include "inventoryeditor.h"
#include "ui_inventoryeditor.h"

#include <QGridLayout>

InventoryEditor::InventoryEditor(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::InventoryEditor();
  ui->setupUi(this);

  ItemDB* itemDB = itemDB->GetInstance();
  BitmapDB* bitmapDB = bitmapDB->GetInstance();
  for (int i = 0; i < itemDB->count(); i++)
  {
    itemInfo* info = itemDB->GetItemByIndex(i);
    QIcon* icon = bitmapDB->ItemIcon(info);
    QVariant pass = QVariant::fromValue(info);

    ui->cmbSearchItem->addItem(*icon, QString::fromUtf8(info->name), pass);
  }

  int areaCount = COUNTOF(inventory_areas);
  editorTabs.resize(areaCount);
  for (size_t i = 0; i < areaCount; i++)
  {
    InventoryEditorTab* editor = new InventoryEditorTab(&inventory_areas[i]);
    editorTabs[i] = editor;

    ui->tabEditors->addTab(editor, tr(inventory_areas[i].area));
  }
}

InventoryEditor::~InventoryEditor()
{
  delete ui;
}

void InventoryEditor::SearchIndexChange(int index)
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  QVariant data = ui->cmbSearchItem->itemData(index);
  itemInfo* info = data.value<itemInfo*>();
  if (!info || index == -1) return;

  int areaCount = COUNTOF(inventory_areas);
  int tabIndex = ui->tabEditors->currentIndex();
  bool allTabs = ui->chkAllTabs->isChecked();
  int startTab = allTabs ? 0 : tabIndex;
  int endTab = allTabs ? areaCount : tabIndex + 1;
  int areaIndex = -1;
  index = -1;

  for (int tab = startTab; tab < endTab; tab++) {
    const inventory_area* area = &inventory_areas[tab];
    u8* slot = ((u8*)mhwSaveSlot) + area->localoffset;
    mhw_item_slot* baseItemSlot = (mhw_item_slot*)slot;

    for (int i = 0; i < area->count; i++)
    {
      mhw_item_slot* itemSlot = baseItemSlot + i;
      if (itemSlot->id == info->id) {
        areaIndex = tab;
        index = i;
        break;
      }
    }
    if (index != -1) break;
  }

  if (index != -1) {
    ui->tabEditors->setCurrentIndex(areaIndex);
    QWidget* current = ui->tabEditors->currentWidget();
    InventoryEditorTab* editor = dynamic_cast<InventoryEditorTab*>(current);
    editor->ScrollToIndex(index);
  }
}

void InventoryEditor::Load(mhw_save_raw* mhwSave, int slotIndex)
{
  SaveLoader::Load(mhwSave, slotIndex);

  for (size_t i = 0; i < editorTabs.count(); i++)
  {
    InventoryEditorTab* editor = editorTabs[i];
    editor->Load(mhwSave, slotIndex);
  }

  SaveLoader::FinishLoad();
}
