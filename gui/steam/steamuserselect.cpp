#include "steamuserselect.h"

#include "../../types/types.h"
#include "../../types/constants.h"
#include "../../utility/system/paths.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QLabel>
#include <QFormLayout>

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
  for (int i = 0; i < users.size(); ++i) {
    QString userIDRaw = users.at(i);
    u32 userID = userIDRaw.toUInt();
    SteamSpecID spec = SteamIDFrom32(userID);
    u64 steamID = spec.full;
    users[i] = QString::number(steamID);
  }
  QString allUsers = users.join(',');

  QString query = tr(endpoint).arg(steamKey).arg(allUsers);
  qInfo() << query;

  NetworkQuery* network = new NetworkQuery();
  int status = network->get(query);
  if (status == 0) processResponse(network);
}

void SteamUserSelect::processResponse(NetworkQuery* reply)
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
  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setStretch(0, 2);

  NetworkQuery* network = new NetworkQuery();
  for (int i = 0; i < players.size(); ++i) {
    const QJsonObject playerInfo = players.at(i).toObject();
    const QString personaName = playerInfo.value("personaname").toString();
    const QString avatarUrl = playerInfo.value("avatarfull").toString();

    NetworkQuery* network = new NetworkQuery();
    int status = network->get(avatarUrl);

    QByteArray imageData = network->recievedData();
    QPixmap pixmap;
    pixmap.loadFromData(imageData, "JPG");

    QLabel* label = new QLabel(this);
    label->setText(personaName);
    label->setPixmap(pixmap);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    qInfo() << personaName;
  }
}
