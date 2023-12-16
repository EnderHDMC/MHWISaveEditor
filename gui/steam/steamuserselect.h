#pragma once

#include <QDialog>
#include "ui_steamuserselect.h"

#include "../../utility/system/network.h"

class SteamUserSelect : public QDialog
{
    Q_OBJECT

public:
    SteamUserSelect(QWidget *parent = nullptr);

private:
    Ui::SteamUserSelectClass ui;

    void GetUsers();

private slots:
    void processResponse(NetworkQuery* reply);
};
