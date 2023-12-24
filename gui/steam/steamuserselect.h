#pragma once

#include <QDialog>
#include "ui_steamuserselect.h"

#include "../../utility/system/network.h"
#include "../../types/types.h"

class SteamUserSelect : public QDialog
{
  Q_OBJECT

public:
  SteamUserSelect(QStringList& users, const QString& user = NULL, bool canCancel = false, QWidget* parent = nullptr);

  QString userId;

private slots:
  void SetUserId(const QString& userId);
  void UserIdChange(const QString& text);

private:
  void ProcessResponse(NetworkQuery* reply);

  Ui::SteamUserSelectClass ui;

  void GetUsers(QStringList& users);
};
