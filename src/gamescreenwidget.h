#ifndef GAMESCREENWIDGET_H
#define GAMESCREENWIDGET_H

#include <QWidget>
#include "server.h"
#include "client.h"
#include <QPointer>
#include "table.h"

namespace Ui {
class GameScreenWidget;
}

class GameScreenWidget : public QWidget
{
    Q_OBJECT

private:
    Table table;
    QPointer<Server> server;
    QPointer<Client> client;
    bool isServer;

public:
    explicit GameScreenWidget(QWidget *parent, Server *server, Client *client, bool isServer);
    ~GameScreenWidget();

private slots:
    void on_DrawButton_clicked();

    void onServerDisconnected(Player const * const player);
    void onServerAfterDisconnected();
    void onServerError(QAbstractSocket::SocketError socketError);
    void onServerDataReceived(Player const * const sender, int messageType, QString message);

    void onClientDisconnected();
    void onClientError(QAbstractSocket::SocketError socketError);
    void onClientDataReceived(int messageType, QString message);

private:
    Ui::GameScreenWidget *ui;
    void log(QString message);
};

#endif // GAMESCREENWIDGET_H
