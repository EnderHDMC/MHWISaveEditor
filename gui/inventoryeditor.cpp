#include "inventoryeditor.h"
#include "ui_inventoryeditor.h"

#include <QGridLayout>

InventoryEditor::InventoryEditor(QWidget *parent)
    : QWidget(parent)
{
    ui = new Ui::InventoryEditor();
    ui->setupUi(this);

    int areaCount = COUNTOF(inventory_areas);
    editorTabs.resize(areaCount);
    for (size_t i = 0; i < areaCount; i++)
    {
      InventoryEditorTab* editor = new InventoryEditorTab(&inventory_areas[i]);
      editorTabs[i] = editor;

      ui->tabWidget->addTab(editor, tr(inventory_areas[i].area));
    }
}

InventoryEditor::~InventoryEditor()
{
    delete ui;
}

void InventoryEditor::Load(mhw_save_raw* save, int saveslot)
{
  for (size_t i = 0; i < editorTabs.count(); i++)
  {
    InventoryEditorTab* editor = editorTabs[i];
    editor->Load(save, saveslot);
  }
}
