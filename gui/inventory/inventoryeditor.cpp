#include "inventoryeditor.h"
#include "ui_inventoryeditor.h"

#include <QGridLayout>
#include "../common/Notification.h"
#include "../../utility/common/Settings.h"
#include "../../utility/mhw_save_utils.h"

InventoryEditor::InventoryEditor(QWidget* parent)
  : QWidget(parent), SaveLoader()
{
  ui = new Ui::InventoryEditor();
  ui->setupUi(this);

  QStringList names;
  names << tr("Item Pouch", "Title for the 'Item Pouch' tab of the inventory editor.");
  names << tr("Ammo Pouch", "Title for the 'Ammo Pouch' tab of the inventory editor.");
  names << tr("Item Box", "Title for the 'Item Box' tab of the inventory editor.");
  names << tr("Ammo Box", "Title for the 'Ammo Box' tab of the inventory editor.");
  names << tr("Material Box", "Title for the 'Material Box' tab of the inventory editor.");
  names << tr("Deco Box", "Title for the 'Deco Box' tab of the inventory editor.");

  int areaCount = COUNTOF(inventory_areas);
  Q_ASSERT(areaCount == names.length());
  editorTabs.resize(areaCount);
  for (size_t i = 0; i < areaCount; i++)
  {
    InventoryEditorTab* editor = new InventoryEditorTab(&inventory_areas[i]);
    editorTabs[i] = editor;

    ui->tabEditors->addTab(editor, names[i]);
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
  itm_entry* info = data.value<itm_entry*>();
  if (!info || index == -1) return;

  int areaCount = COUNTOF(inventory_areas);
  int tabIndex = ui->tabEditors->currentIndex();
  bool allTabs = ui->chkAllTabs->isChecked();
  int startTab = allTabs ? 0 : tabIndex;
  int endTab = allTabs ? areaCount : tabIndex + 1;
  int areaIndex = -1;
  index = -1;

  Settings* settings = settings->GetInstance();
  bool searchAllTabsIncludeItemPouch = settings->GetItemPouchSearchAll();

  mhw_item_slot* baseItemSlot = nullptr;
  mhw_item_slot* findItem = nullptr;
  for (int tab = startTab; tab < endTab; tab++) {
    const inventory_area* area = &inventory_areas[tab];
    if (allTabs && !searchAllTabsIncludeItemPouch && !area->storage) continue;

    u8* slot = ((u8*)mhwSaveSlot) + area->localoffset;
    baseItemSlot = (mhw_item_slot*)slot;

    findItem = MHWSaveUtils::FindItem(baseItemSlot, area->count, info->id);
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
    notif->ShowMessage(tr("Failed to find item: %1", "Indicate failed item search, %1 is the item that was being searched for.").arg(itemDB->ItemName(info)));
  }
}

void InventoryEditor::ItemAdd()
{
  MHW_SAVE_GUARD;
  mhw_save_slot* mhwSaveSlot = MHW_SaveSlot();

  QVariant data = ui->cmbSearchItem->currentData();
  itm_entry* info = data.value<itm_entry*>();
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
    notif->ShowMessage(tr("Failed to find place to add: %1", "Indicate failed item add, %1 is the item to be added.").arg(itemDB->ItemName(info)));
    return;
  }
  u8* slot = ((u8*)mhwSaveSlot) + storageArea->localoffset;
  mhw_item_slot* baseItemSlot = (mhw_item_slot*)slot;
  mhw_item_slot* findItem = MHWSaveUtils::FindCategoryItemOrEmpty(mhwSaveSlot, info);

  if (findItem) {
    MHWSaveUtils::DiscoverItem(mhwSaveSlot, info);
    MHWSaveUtils::GiveItem(findItem, info);

    int index = findItem - baseItemSlot;
    ui->tabEditors->setCurrentIndex(areaIndex);
    QWidget* current = ui->tabEditors->currentWidget();
    InventoryEditorTab* editor = dynamic_cast<InventoryEditorTab*>(current);
    editor->LoadIndex(index);
    editor->ScrollToIndex(index);
    notif->ShowMessage(tr("Added item: %1", "Added an item, %1 is the name of the item that was added.").arg(itemDB->ItemName(info)));
  }
  else {
    notif->ShowMessage(tr("Failed to find place to add: %1", "Indicate failed item add, %1 is the item to be added.").arg(itemDB->ItemName(info)));
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

void InventoryEditor::LoadResources(ItemDB* itemDB, BitmapDB* bitmapDB)
{
  SaveLoader::LoadResources(itemDB, bitmapDB);

  ui->cmbSearchItem->clear();
  Settings* settings = settings->GetInstance();
  bool showUnobtainable = settings->GetShowUnobtainable();

  for (int i = 0; i < itemDB->count(); i++)
  {
    itm_entry* info = itemDB->GetItemById(i);
    if (info->type == (u32)itemCategory::Furniture) continue;
    if (info->type == (u32)itemCategory::Account) continue;
    if (info->flags & (u32)itemFlag::RejectFlag && info->id != ItemDB::IgniSignID) continue;

    QString itemName = itemDB->ItemName(info);
    if (!(info->flags & (u32)itemFlag::CustomObtainable) && info->id) {
      if (info->flags & (u32)itemFlag::CustomSlingerAmmo) continue;

      // Truly unobtainable items.
      if (!(info->flags & (u32)itemFlag::CustomDiscoverable)) {
        qInfo().nospace() << "Unobtainable Item[" << info->id << "](" << info->type << "): " << itemName;
        if (!showUnobtainable) continue;
      }
    }

    QIcon* icon = bitmapDB->ItemIcon(itemDB, info);
    QVariant pass = QVariant::fromValue(info);

    ui->cmbSearchItem->addItem(*icon, itemName, pass);
  }

  for (size_t i = 0; i < editorTabs.count(); i++)
  {
    InventoryEditorTab* editor = editorTabs[i];
    editor->LoadResources(itemDB, bitmapDB);
  }
}
