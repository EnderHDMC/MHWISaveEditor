#pragma once

#include <QDialog>
#include "ui_steamuserselect.h"

#include "../../utility/system/network.h"
#include "../../types/types.h"

class SteamUserSelect : public QDialog
{
  Q_OBJECT

public:
  SteamUserSelect(QWidget* parent = nullptr);

  uint64 userId;

private slots:
  void SetUserId(const QString& userId);

private:
  void ProcessResponse(NetworkQuery* reply);

  Ui::SteamUserSelectClass ui;

  void GetUsers();
};
