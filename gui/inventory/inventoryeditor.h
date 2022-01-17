#pragma once

#include <QWidget>

#include "inventoryeditortab.h"

namespace Ui { class InventoryEditor; };

class InventoryEditor : public QWidget, public SaveLoader
{
    Q_OBJECT

public:
    InventoryEditor(QWidget *parent = Q_NULLPTR);
    ~InventoryEditor();

    // Inherited via SaveLoader
    virtual void Load(mhw_save_raw* mhwSave, int mhwSaveSlot) override;

private:
    Ui::InventoryEditor *ui;

    QList<InventoryEditorTab*> editorTabs;
};
