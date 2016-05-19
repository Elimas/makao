#include "gamescreenwidget.h"
#include "ui_gamescreenwidget.h"
#include "messagetype.h"
#include "opponentcardswidget.h"
#include <QDesktopWidget>
#include "time.h"
#include <QMessageBox>

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
        connect(server, SIGNAL(onDataReceived(Player*,int,QString)), this, SLOT(onServerDataReceived(Player*,int,QString)));
        connect(server, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onServerError(QAbstractSocket::SocketError)));
    }
    if (ui->TableCard != NULL) delete ui->TableCard;
    ui->TableCard = new SingleCardWidget(table.topCard(), this);
    ui->TableCard->move(ui->Game->rect().center() - ui->TableCard->rect().center());
    ui->TableCard->show();
    if (!isServer)
    {
        client->sendMessage(MessageType::Ready, "");
    }
    if (isServer)
        ui->labelCurrentPlayer->setText(server->getHostPlayer()->getName());
    else
        ui->labelCurrentPlayer->setText(client->getPlayer()->getName());
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
    QMessageBox msgBox;
    QString msg = QString("Jeden z graczy się rozłączył przez co nie można kontynuować gry. Wracanie do menu.");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Icon::Critical);
    msgBox.exec();
    this->deleteLater();
}
void GameScreenWidget::onServerAfterDisconnected()
{

}
void GameScreenWidget::onServerError(QAbstractSocket::SocketError socketError)
{

}
void GameScreenWidget::onServerDataReceived(Player* sender, int messageType, QString message)
{
    log(QString("[DEBUG][Game] Type: %1 Msg: %2").arg(messageType).arg(message));
    if (messageType == MessageType::Ready)
    {
        sender->ready = true;
        bool allReady = true;
        for (int i = 0; i < server->getOtherPlayers().size(); i++)
        {
            Player* p = server->getOtherPlayers().at(i);
            if (!p->ready) { allReady = false; break; }
        }
        if (allReady) startGame();
    }
}
//slots in client
void GameScreenWidget::onClientDisconnected()
{
    QMessageBox msgBox;
    QString msg = QString("Utracono połączenie z hostem. Wracanie do menu.");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Icon::Critical);
    msgBox.exec();
    this->deleteLater();
}
void GameScreenWidget::onClientError(QAbstractSocket::SocketError socketError)
{
    log("Socket error");
}
void GameScreenWidget::onClientDataReceived(int messageType, QString message)
{
    log(QString("[DEBUG][Game] Type: %1 Msg: %2").arg(messageType).arg(message));
    qDebug() << MessageType::AddCard;
    if (messageType == MessageType::SetTableCard)
    {
        QStringList a = message.split(";");
        if (ui->TableCard != NULL) delete ui->TableCard;
        ui->TableCard = new SingleCardWidget(Card(static_cast<Card::Suit>(a.value(0).toInt()), static_cast<Card::Pip>(a.value(1).toInt())), (QWidget*) this);
        ui->TableCard->move(ui->Game->rect().center() - ui->TableCard->rect().center());
        ui->TableCard->show();
    }
    else if (messageType == MessageType::AddCard)
    {
        log("test");
        Card c = Card::deserialize(message);
        client->getPlayer()->addCard(c);
        ui->CurrentPlayer->addCard(c);
    }
    else if (messageType == MessageType::CardsNumber)
    {
        QStringList l = message.split(";");
        int playerId = l.value(0).toInt();
        int cardsCount = l.value(1).toInt();
        for (int i = 0; i < client->getOtherPlayers().size(); i++)
        {
            Player* p = client->getOtherPlayers().at(i);
            if (p->getId() == playerId) p->cardsCount = cardsCount;
        }
    }
    else if (messageType == MessageType::PlayerTurn)
    {
        int playerId = message.toInt();
        if (client->getPlayer()->getId() == playerId)
        {
            //TODO ruch aktualnego gracza
        }
        else
        {
            //TODO tura innego gracza
        }
    }
}

void GameScreenWidget::log(QString message)
{
    qDebug() << message;
}

void GameScreenWidget::startGame()
{
    server->sendMessageToAllPlayers(MessageType::SetTableCard, table.topCard().serialize());
    //rozdajemy karty graczom
    for (int i = 0; i < 5; i++)
    {
        Card c = table.getNewCard();
        server->getHostPlayer()->addCard(c);
        ui->CurrentPlayer->addCard(c);
        for (int j = 0; j < server->getOtherPlayers().size(); j++)
        {
            c = table.getNewCard();
            Player* p = server->getOtherPlayers().at(j);
            p->addCard(c);
            server->sendMessage(p, MessageType::AddCard, c.serialize());
        }
    }
    for (int j = 0; j < server->getOtherPlayers().size(); j++)
    {
        Player* p = server->getOtherPlayers().at(j);
        server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(p->getId()).arg(p->cardsCount));
    }
    //losujemy gracza który zaczyna grę
    srand(time(NULL));
    int i = rand() % (server->getOtherPlayers().size() + 1);
    i--;
    int id;
    if (i == -1)
    {
        id = 1;
    }
    else
    {
        id = server->getOtherPlayers().at(i)->getId();
    }
    server->sendMessageToAllPlayers(MessageType::PlayerTurn, QString::number(id));
}

void GameScreenWidget::on_exitButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Czy chcesz zakończyć grę? Wszyscy gracze zostaną rozłączeni.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Icon::Question);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        this->hide();
        this->deleteLater();
    }
}
