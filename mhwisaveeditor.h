#ifndef MHWISAVEEDITOR_H
#define MHWISAVEEDITOR_H

#include <QMainWindow>
#include <QSignalMapper>
#include "types/mhwSave.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MHWISaveEditor; }
QT_END_NAMESPACE

class MHWISaveEditor : public QMainWindow
{
  Q_OBJECT

public slots:
  void Open();
  void Save();
  void Slot(int slot);

public:
  MHWISaveEditor(QWidget *parent = nullptr);
  ~MHWISaveEditor();

private:
  Ui::MHWISaveEditor *ui;
  MHWSaveRaw *mhwRaw;

  QSignalMapper *slotSignalMapper;
};
#endif // MHWISAVEEDITOR_H
