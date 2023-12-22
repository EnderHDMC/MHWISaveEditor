#include "steamuserselect.h"

#include "../../types/types.h"
#include "../../types/constants.h"
#include "../../utility/system/paths.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QToolButton>
#include <QFormLayout>
#include <QSignalMapper>

SteamUserSelect::SteamUserSelect(QWidget* parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  GetUsers();
}

void SteamUserSelect::GetUsers()
{
  const char* steamKey = STEAM_API_KEY_EMBED.c_str();
  const char* endpoint = "https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key=%1&steamids=%2";
  QStringList users = Paths::GetSteamUsers();

  int userCount = users.size();
  userCount = (userCount > 100) ? 100 : userCount;
  for (int i = 0; i < userCount; ++i) {
    QString userIDRaw = users.at(i);
    u32 userID = userIDRaw.toUInt();
    SteamSpecID spec = SteamIDFrom32(userID);
    u64 steamID = spec.full;
    users[i] = QString::number(steamID);
  }
  QString allUsers = users.join(',');

  QString query = tr(endpoint).arg(steamKey).arg(allUsers);
  NetworkQuery* network = new NetworkQuery();
  int status = network->get(query);
  if (status == 0) ProcessResponse(network);
}

void SteamUserSelect::ProcessResponse(NetworkQuery* reply)
{
  QByteArray loadData = reply->recievedData();
  QJsonParseError jsonError;
  QJsonDocument request = QJsonDocument::fromJson(loadData, &jsonError);

  if (jsonError.error != QJsonParseError::NoError) {
    qDebug() << "Error: " << jsonError.errorString();
    return;
  }

  const QJsonObject root = request.object();
  const QJsonArray players = root.value("response").toObject().value("players").toArray();
  QHBoxLayout* layout = ui.horizontalLayout;

  QSignalMapper* slotSignalMapper = new QSignalMapper(this);

  for (int i = 0; i < players.size(); ++i) {
    const QJsonObject playerInfo = players.at(i).toObject();
    const QString personaName = playerInfo.value("personaname").toString();
    const QString avatarUrl = playerInfo.value("avatarfull").toString();
    const QString steamId = playerInfo.value("steamid").toString();

    NetworkQuery* network = new NetworkQuery();
    int status = network->get(avatarUrl);
    if (status) continue;

    QByteArray imageData = network->recievedData();
    QPixmap pixmap;
    pixmap.loadFromData(imageData, "JPG");
    QIcon icon = QIcon(pixmap);
    QSize size = pixmap.size();

    QToolButton* button = new QToolButton(this);
    button->setText(personaName);
    button->setIcon(icon);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(size);
    layout->addWidget(button);

    connect(button, SIGNAL(clicked()), slotSignalMapper, SLOT(map()));
    slotSignalMapper->setMapping(button, steamId);

    qInfo().nospace() << "Steam user: " << personaName << "(" << steamId << ")";
  }

  connect(slotSignalMapper, SIGNAL(mappedString(const QString&)), this, SLOT(SetUserId(const QString&)));
  adjustSize();
}

void SteamUserSelect::SetUserId(const QString& userId)
{
  qInfo() << "Selected:" << userId;
  this->userId = userId.toULongLong();
  this->accept();
}
