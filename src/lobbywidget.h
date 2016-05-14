#ifndef LOBBYWIDGET_H
#define LOBBYWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include "server.h"
#include "client.h"

namespace Ui {
class LobbyWidget;
}

class LobbyWidget : public QWidget
{
    Q_OBJECT

public:
    static LobbyWidget* createServerLobby(QWidget *parent, Server *server, int port);
    static LobbyWidget* createClientLobby(QWidget *parent, Client *client, QString serverIP, int port);
    ~LobbyWidget();
    void startServer();
    void startClient();

private:
    Ui::LobbyWidget *ui;
    QWidget *mainWindowParent;
    bool isServer;
    QString serverIP;
    int port;
    Server *server;
    Client *client;
    LobbyWidget(QWidget *parent = 0);
    void log(QString message);
    void updatePlayersList();

private slots:
    void onServerConnected(Player const * const player);
    void onServerDisconnected(Player const * const player);
    void onServerAfterDisconnected();
    void onServerError(QAbstractSocket::SocketError socketError);
    void onServerDataReceived(Player const * const sender, int messageType, QString message);

    void onClientConnected();
    void onClientDisconnected();
    void onClientError(QAbstractSocket::SocketError socketError);
    void onClientDataReceived(int messageType, QString message);

    void on_buttonBack_clicked();
    void on_buttonStart_clicked();
};

#endif // LOBBYWIDGET_H
