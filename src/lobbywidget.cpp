#include "lobbywidget.h"
#include "ui_lobbywidget.h"
#include "messagetype.h"
#include "gamescreenwidget.h"

LobbyWidget::LobbyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LobbyWidget)
{
    ui->setupUi(this);
}

LobbyWidget::~LobbyWidget()
{
    if (deleteServerFlag)
    {
        if (server) server->deleteLater();
        if (client) client->deleteLater();
    }
    delete ui;
}

LobbyWidget* LobbyWidget::createServerLobby(QWidget *parent, Server *server, int port)
{
    LobbyWidget *l = new LobbyWidget(parent);
    l->mainWindowParent=parent;
    l->isServer = true;
    l->server = server;
    l->client = NULL;
    l->port = port;
    l->serverIP = QString();
    l->deleteServerFlag = true;
    return l;
}

LobbyWidget* LobbyWidget::createClientLobby(QWidget *parent, Client *client, QString serverIP, int port)
{
    LobbyWidget *l = new LobbyWidget(parent);
    l->mainWindowParent=parent;
    l->isServer = false;
    l->client = client;
    l->server = NULL;
    l->serverIP = serverIP;
    l->port = port;
    l->ui->buttonStart->setEnabled(false);
    l->deleteServerFlag = true;
    return l;
}

void LobbyWidget::startServer()
{
    if (isServer)
    {
        connect(server, SIGNAL(onConnected(Player*const)), this, SLOT(onServerConnected(Player*const)));
        connect(server, SIGNAL(onDisconnected(Player*const)), this, SLOT(onServerDisconnected(Player*const)));
        connect(server, SIGNAL(afterDisconnected()), this, SLOT(onServerAfterDisconnected()));
        connect(server, SIGNAL(onDataReceived(Player*,int,QString)), this, SLOT(onServerDataReceived(Player*,int,QString)));
        connect(server, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onServerError(QAbstractSocket::SocketError)));
        if (server->listen(QHostAddress::Any, port))
        {
            log(QString("Server started on port %1").arg(port));
            log("Waiting for players to join");
            updatePlayersList();
        } else {
            QMessageBox msgBox;
            QString msg = QString("Failed binding TCP Socket on port %1").arg(port);
            log(msg);
            msgBox.setText(msg);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    } else qDebug() << "Error client config specified";
}

void LobbyWidget::startClient()
{
    if (!isServer)
    {
        log("Connecting to server...");
        connect(client, SIGNAL(onConnected()), this, SLOT(onClientConnected()));
        connect(client, SIGNAL(onDisconnected()), this, SLOT(onClientDisconnected()));
        connect(client, SIGNAL(onDataReceived(int,QString)), this, SLOT(onClientDataReceived(int,QString)));
        connect(client, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onClientError(QAbstractSocket::SocketError)));
        client->connectToHost(serverIP, port);
        updatePlayersList();
    } else qDebug() << "Error server config specified";
}

void LobbyWidget::log(QString message)
{
    qDebug() << message;
    ui->textLog->appendPlainText(message);
}

void LobbyWidget::updatePlayersList()
{
    if (isServer)
    {
        ui->listPlayers->clear();
        ui->listPlayers->addItem(server->getHostPlayer()->getName());
        for (int i = 0; i < server->getOtherPlayers().size(); i++)
        {
            ui->listPlayers->addItem(server->getOtherPlayers().at(i)->getName());
        }
    } else {
        ui->listPlayers->clear();
        ui->listPlayers->addItem(client->getPlayer()->getName());
        for (int i = 0; i < client->getOtherPlayers().size(); i++)
        {
            ui->listPlayers->addItem(client->getOtherPlayers().at(i)->getName());
        }
    }
}

//slots in server
void LobbyWidget::onServerConnected(Player const * const player)
{
    log(QString("Player %1 connected").arg(player->getName()));
    updatePlayersList();
}
void LobbyWidget::onServerDisconnected(Player const * const player)
{
    log(QString("Player %1 disconnected").arg(player->getName()));
}
void LobbyWidget::onServerAfterDisconnected()
{
    updatePlayersList();
}
void LobbyWidget::onServerError(QAbstractSocket::SocketError socketError)
{
    log("Socket error");
}
void LobbyWidget::onServerDataReceived(Player* sender, int messageType, QString message)
{
    if (messageType == MessageType::SetName) {
        log(QString("Player set new name %1").arg(message));
        updatePlayersList();
    }
    log(QString("[DEBUG][Lobby] Type: %1 Msg: %2").arg(messageType).arg(message));
}
//slots in client
void LobbyWidget::onClientConnected()
{
    log("Connected to server");
}
void LobbyWidget::onClientDisconnected()
{
    log("Disconnected from server");
    QMessageBox msgBox;
    QString msg = QString("Utracono połączenie z hostem. Wracanie do menu.");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Icon::Critical);
    msgBox.exec();
    this->deleteLater();
}
void LobbyWidget::onClientError(QAbstractSocket::SocketError socketError)
{
    log("Socket error");
    if (socketError == QTcpSocket::SocketError::ConnectionRefusedError)
    {
        QMessageBox msgBox;
        QString msg = QString("Connection to server refused");
        log(msg);
        msgBox.setText(msg);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}
void LobbyWidget::onClientDataReceived(int messageType, QString message)
{
    log(QString("[DEBUG][Lobby] Type: %1 Msg: %2").arg(messageType).arg(message));
    if (messageType == MessageType::PlayerConnected || messageType == MessageType::PlayerDisconnected || messageType == MessageType::PlayerSetNewName)
    {
        updatePlayersList();
    }
    else if (messageType == MessageType::StartGame)
    {
        deleteServerFlag = false;
        GameScreenWidget *gameWindow = new GameScreenWidget(mainWindowParent, server, client, isServer);
        gameWindow->show();
        //client->disconnect();
        this->hide();
        this->deleteLater();
    }
}

void LobbyWidget::on_buttonBack_clicked()
{
    this->hide();
    this->deleteLater();
}
void LobbyWidget::on_buttonStart_clicked()
{
    if (isServer)
    {
        if (server->getOtherPlayers().size() > 0)
        {
            GameScreenWidget *gameWindow = new GameScreenWidget(mainWindowParent, server, client, isServer);
            server->sendMessageToAllPlayers(MessageType::StartGame, "");
            server->allowConnecting = false;
            gameWindow->show();
            deleteServerFlag = false;
            //server->disconnect();
            this->hide();
            this->deleteLater();
        }
        else
        {
            QMessageBox msgBox;
            QString msg = QString("Musi dołączyć co najmniej 1 gracz");
            msgBox.setText(msg);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
    }
}
