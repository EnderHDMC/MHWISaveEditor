#include "inventoryeditor.h"
#include "ui_inventoryeditor.h"

#include <QGridLayout>
#include "../common/Settings.h"
#include "../common/Notification.h"
#include "../../utility/mhw_save_utils.h"

InventoryEditor::InventoryEditor(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::InventoryEditor();
  ui->setupUi(this);

  ItemDB* itemDB = itemDB->GetInstance();
  BitmapDB* bitmapDB = bitmapDB->GetInstance();
  Settings* settings = settings->GetInstance();
  bool showUnobtainable = settings->showUnobtainable;

  for (int i = 0; i < itemDB->count(); i++)
  {
    itemInfo* info = itemDB->GetItemByIndex(i);
    if (info->type == (u32)item_type::Furniture) continue;
    if (info->type == (u32)item_type::Account) continue;
    if (info->flags & (u32)itemFlag::RejectFlag) continue;

    QString itemName = itemDB->ItemName(info);
    if (!(info->flags & (u32)itemFlag::CustomObtainable) && info->id) {
      if (info->flags & (u32)itemFlag::CustomSlingerAmmo) continue;

      // Truly unobtainable items.
      if (!(info->flags & (u32)itemFlag::CustomDiscoverable)) {
        qInfo() << "Unobtainable Item[" << info->id << ":" << info->type << "]: " << itemName;
        if (!showUnobtainable) continue;
      }
    }

    QIcon* icon = bitmapDB->ItemIcon(info);
    QVariant pass = QVariant::fromValue(info);

    ui->cmbSearchItem->addItem(*icon, itemName, pass);
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
  ItemDB* itemDB = itemDB->GetInstance();

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

  Settings* settings = settings->GetInstance();
  bool searchAllTabsIncludeItemPouch = settings->searchAllTabsIncludeItemPouch;

  mhw_item_slot* baseItemSlot = nullptr;
  mhw_item_slot* findItem = nullptr;
  for (int tab = startTab; tab < endTab; tab++) {
    const inventory_area* area = &inventory_areas[tab];
    if (allTabs && !searchAllTabsIncludeItemPouch && !area->storage) continue;

    u8* slot = ((u8*)mhwSaveSlot) + area->localoffset;
    baseItemSlot = (mhw_item_slot*)slot;

    findItem = FindItem(baseItemSlot, area->count, info->id);
    if (findItem) {
      areaIndex = tab;
      break;
    }
  }

  if (findItem) {
    int index = findItem - baseItemSlot;
    ui->tabEditors->setCurrentIndex(areaIndex);
    QWidget* current = ui->tabEditors->currentWidget();
    InventoryEditorTab* editor = dynamic_cast<InventoryEditorTab*>(current);
    editor->ScrollToIndex(index);
  }
  else {
  Notification* notif = notif->GetInstance();
  notif->ShowMessage("Failed to find item: " + itemDB->ItemName(info));
  }
}

void InventoryEditor::ItemAdd()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();
  ItemDB* itemDB = itemDB->GetInstance();

  QVariant data = ui->cmbSearchItem->currentData();
  itemInfo* info = data.value<itemInfo*>();
  if (!info || !info->id) return;

  const inventory_area* storageArea = nullptr;
  int areaCount = COUNTOF(inventory_areas);
  int areaIndex = -1;

  for (int tab = 0; tab < areaCount; tab++) {
    const inventory_area* area = &inventory_areas[tab];
    if (!area->storage) continue;
    if ((u32)area->type != info->type) continue;
    storageArea = area;
    areaIndex = tab;
  }

  Notification* notif = notif->GetInstance();
  if (!storageArea) {
    notif->ShowMessage("Failed to find place to add: " + itemDB->ItemName(info));
    return;
  }
  u8* slot = ((u8*)mhwSaveSlot) + storageArea->localoffset;
  mhw_item_slot* baseItemSlot = (mhw_item_slot*)slot;
  mhw_item_slot* findItem = FindItemOrEmpty(baseItemSlot, storageArea->count, info->id);

  if (findItem) {
    findItem->id = info->id;
    findItem->amount++;
    if (findItem->amount > 9999) findItem->amount = 9999;

    int index = findItem - baseItemSlot;
    ui->tabEditors->setCurrentIndex(areaIndex);
    QWidget* current = ui->tabEditors->currentWidget();
    InventoryEditorTab* editor = dynamic_cast<InventoryEditorTab*>(current);
    editor->LoadIndex(index);
    editor->ScrollToIndex(index);
    notif->ShowMessage("Added item: " + itemDB->ItemName(info));
  }
  else {
    notif->ShowMessage("Failed to find place to add: " + itemDB->ItemName(info));
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
