#pragma once

#include <QWidget>

#include "inventoryeditortab.h"

namespace Ui { class InventoryEditor; };

class InventoryEditor : public QWidget
{
    Q_OBJECT

public:
    InventoryEditor(QWidget *parent = Q_NULLPTR);
    ~InventoryEditor();

    void Load(mhw_save_raw* save, int saveslot);

private:
    Ui::InventoryEditor *ui;

    QList<InventoryEditorTab*> editorTabs;
};
