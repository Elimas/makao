#include "gamescreenwidget.h"
#include "ui_gamescreenwidget.h"
#include "messagetype.h"
#include "opponentcardswidget.h"
#include <QDesktopWidget>

GameScreenWidget::GameScreenWidget(QWidget *parent, Server *server, Client *client, bool isServer) :
    QWidget(parent), server(server), client(client), isServer(isServer),
    ui(new Ui::GameScreenWidget)
{
	ui->setupUi(this);

    ui->Player2->setOrientation(OpponentCardsWidget::Orientation::Vertical);
    ui->Player4->setOrientation(OpponentCardsWidget::Orientation::Vertical);

	//TODO: przyklad
	ui->Player2->setCardsNumber(3);
	ui->Player3->setCardsNumber(4);
	ui->Player4->setCardsNumber(1);

    if (!isServer)
    {
        connect(client, SIGNAL(onDisconnected()), this, SLOT(onClientDisconnected()));
        connect(client, SIGNAL(onDataReceived(int,QString)), this, SLOT(onClientDataReceived(int,QString)));
        connect(client, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onClientError(QAbstractSocket::SocketError)));
    }
    else
    {
        connect(server, SIGNAL(onDisconnected(Player*const)), this, SLOT(onServerDisconnected(Player*const)));
        connect(server, SIGNAL(afterDisconnected()), this, SLOT(onServerAfterDisconnected()));
        connect(server, SIGNAL(onDataReceived(Player*const,int,QString)), this, SLOT(onServerDataReceived(Player*const,int,QString)));
        connect(server, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onServerError(QAbstractSocket::SocketError)));
    }
    if (ui->TableCard != NULL) delete ui->TableCard;
    ui->TableCard = new SingleCardWidget(table.topCard(), parent);
    ui->TableCard->move(ui->Game->rect().center() - ui->TableCard->rect().center());
    ui->TableCard->show();
}

GameScreenWidget::~GameScreenWidget()
{
    if (server) server->deleteLater();
    if (client) client->deleteLater();
    delete ui;
}

//slots in server
void GameScreenWidget::onServerDisconnected(Player const * const player)
{

}
void GameScreenWidget::onServerAfterDisconnected()
{

}
void GameScreenWidget::onServerError(QAbstractSocket::SocketError socketError)
{

}
void GameScreenWidget::onServerDataReceived(Player const * const sender, int messageType, QString message)
{
    log(QString("[DEBUG] Type: %1 Msg: %2").arg(messageType).arg(message));
}
//slots in client
void GameScreenWidget::onClientDisconnected()
{

}
void GameScreenWidget::onClientError(QAbstractSocket::SocketError socketError)
{
    log("Socket error");
}
void GameScreenWidget::onClientDataReceived(int messageType, QString message)
{
    log(QString("[DEBUG] Type: %1 Msg: %2").arg(messageType).arg(message));
}

void GameScreenWidget::on_DrawButton_clicked()
{
    this->hide();
    this->deleteLater();
}

void GameScreenWidget::log(QString message)
{
    qDebug() << message;
}
