#include "gamescreenwidget.h"
#include "ui_gamescreenwidget.h"
#include "messagetype.h"
#include "opponentcardswidget.h"
#include <QDesktopWidget>
#include "time.h"
#include <QMessageBox>

GameScreenWidget::GameScreenWidget(QWidget *parent, Server *server, Client *client, bool isServer) :
    QWidget(parent), server(server), client(client), isServer(isServer), currentPlayerId(0), currentPlayerIndex(0),
    ui(new Ui::GameScreenWidget)
{
	ui->setupUi(this);

    ui->Player3->setOrientation(OpponentCardsWidget::Orientation::Vertical);
    ui->Player4->setOrientation(OpponentCardsWidget::Orientation::Vertical);

    if (!isServer)
    {
        connect(client, SIGNAL(onDisconnected()), this, SLOT(onClientDisconnected()));
        connect(client, SIGNAL(onDataReceived(int,QString)), this, SLOT(onClientDataReceived(int,QString)));
        connect(client, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onClientError(QAbstractSocket::SocketError)));
        ui->labelCurrentPlayer->setText(client->getPlayer()->getName());
        ui->labelPlayer2->hide();
        ui->labelPlayer3->hide();
        ui->labelPlayer4->hide();
        for (int i = 0; i < client->getOtherPlayers().size(); i++)
        {
            Player* p = client->getOtherPlayers().at(i);
            switch(i)
            {
            case 0:
                ui->labelPlayer2->show();
                ui->labelPlayer2->setText(p->getName());
                break;
            case 1:
                ui->labelPlayer3->show();
                ui->labelPlayer3->setText(p->getName());
                break;
            case 2:
                ui->labelPlayer4->show();
                ui->labelPlayer4->setText(p->getName());
                break;
            }
        }
    }
    else
    {
        connect(server, SIGNAL(onDisconnected(Player*const)), this, SLOT(onServerDisconnected(Player*const)));
        connect(server, SIGNAL(afterDisconnected()), this, SLOT(onServerAfterDisconnected()));
        connect(server, SIGNAL(onDataReceived(Player*,int,QString)), this, SLOT(onServerDataReceived(Player*,int,QString)));
        connect(server, SIGNAL(onError(QAbstractSocket::SocketError)), this, SLOT(onServerError(QAbstractSocket::SocketError)));
        ui->labelCurrentPlayer->setText(server->getHostPlayer()->getName());
        ui->labelPlayer2->hide();
        ui->labelPlayer3->hide();
        ui->labelPlayer4->hide();
        for (int i = 0; i < server->getOtherPlayers().size(); i++)
        {
            Player* p = server->getOtherPlayers().at(i);
            switch(i)
            {
            case 0:
                ui->labelPlayer2->show();
                ui->labelPlayer2->setText(p->getName());
                break;
            case 1:
                ui->labelPlayer3->show();
                ui->labelPlayer3->setText(p->getName());
                break;
            case 2:
                ui->labelPlayer4->show();
                ui->labelPlayer4->setText(p->getName());
                break;
            }
        }
    }
    setTableCard(table.topCard());
    if (!isServer)
    {
        client->sendMessage(MessageType::Ready, "");
    }
    if (isServer)
        ui->labelCurrentPlayer->setText(server->getHostPlayer()->getName());
    else
        ui->labelCurrentPlayer->setText(client->getPlayer()->getName());
    connect(ui->CurrentPlayer, SIGNAL(cardClicked(const Card&, int)), this, SLOT(cardClicked(const Card&,int)));
    ui->CurrentPlayer->setEnabled(false);
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
    else if (messageType == MessageType::TakeNewCard)
    {
        if (currentPlayerId == sender->getId())
        {
            Card c = table.getNewCard();
            sender->addCard(c);
            server->sendMessage(sender, MessageType::AddCard, c.serialize());
            server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(sender->getId()).arg(sender->cardsCount));
            nextPlayerTurn();
        }
    }
    else if (messageType == MessageType::PlayCard)
    {
        if (currentPlayerId == sender->getId())
        {
            int index = message.toInt();
            Card& card = sender->cardAt(index);
            if (table.CanPlayCard(card))
            {
                table.PlayCard(card);
                sender->removeCard(index);
                setTableCard(table.topCard());
                server->sendMessageToAllPlayers(MessageType::SetTableCard, table.topCard().serialize());
                server->sendMessage(sender, MessageType::RemoveCard, QString::number(index));
                server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(sender->getId()).arg(sender->cardsCount));
                nextPlayerTurn();
            }
            else
            {
                server->sendMessage(sender, MessageType::CantPlayCard, QString::number(index));
            }
        }
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
        setTableCard(Card(static_cast<Card::Suit>(a.value(0).toInt()), static_cast<Card::Pip>(a.value(1).toInt())));
    }
    else if (messageType == MessageType::AddCard)
    {
        Card c = Card::deserialize(message);
        client->getPlayer()->addCard(c);
        ui->CurrentPlayer->addCard(c);
    }
    else if (messageType == MessageType::RemoveCard)
    {
        int cardIndex = message.toInt();
        client->getPlayer()->removeCard(cardIndex);
        ui->CurrentPlayer->removeCard(cardIndex);
    }
    else if (messageType == MessageType::CardsNumber)
    {
        QStringList l = message.split(";");
        int playerId = l.value(0).toInt();
        int cardsCount = l.value(1).toInt();
        for (int i = 0; i < client->getOtherPlayers().size(); i++)
        {
            Player* p = client->getOtherPlayers().at(i);
            if (p->getId() == playerId)
            {
                OpponentCardsWidget *o = NULL;
                p->cardsCount = cardsCount;
                switch (i)
                {
                case 0:
                    o = ui->Player2;
                    break;
                case 1:
                    o = ui->Player3;
                    break;
                case 2:
                    o = ui->Player4;
                    break;
                }
                if (o != NULL)
                {
                    o->setCardsNumber(cardsCount);
                }
            }
        }
    }
    else if (messageType == MessageType::PlayerTurn)
    {
        int playerId = message.toInt();
        if (client->getPlayer()->getId() == playerId)
        {
            //TODO ruch aktualnego gracza
            currentPlayerIndex = -1;
            currentPlayerId = client->getPlayer()->getId();
            ui->CurrentPlayer->setEnabled(true);
        }
        else
        {
            //TODO tura innego gracza
            for (int i = 0; i < client->getOtherPlayers().size(); i++)
            {
                Player* p = client->getOtherPlayers().at(i);
                if (p->getId() == playerId)
                {
                    currentPlayerIndex = i;
                    currentPlayerId = p->getId();
                    break;
                }
            }
            ui->CurrentPlayer->setEnabled(false);
        }
        log(QString("ID: %1, Index: %2, playerId: %3").arg(currentPlayerId).arg(currentPlayerIndex).arg(playerId));
        refreshTurnLabels();
    }
    else if (messageType == MessageType::CantPlayCard)
    {
        QMessageBox msgBox;
        QString msg = QString("Nie możesz teraz zagrać tą kartą.");
        msgBox.setText(msg);
        msgBox.setIcon(QMessageBox::Icon::Warning);
        msgBox.exec();
        ui->CurrentPlayer->setEnabled(true);
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
    server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->cardsCount));
    for (int i = 0; i < server->getOtherPlayers().size(); i++)
    {
        Player* p = server->getOtherPlayers().at(i);
        server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(p->getId()).arg(p->cardsCount));

        OpponentCardsWidget *o = NULL;
        switch (i)
        {
        case 0:
            o = ui->Player2;
            break;
        case 1:
            o = ui->Player3;
            break;
        case 2:
            o = ui->Player4;
            break;
        }
        if (o != NULL)
        {
            o->setCardsNumber(p->cardsCount);
        }
    }
    //losujemy gracza który zaczyna grę
    srand(time(NULL));
    int i = rand() % (server->getOtherPlayers().size() + 1);
    i--;
    int id;
    if (i == -1)
    {
        id = 1;
        ui->CurrentPlayer->setEnabled(true);
    }
    else
    {
        id = server->getOtherPlayers().at(i)->getId();
        ui->CurrentPlayer->setEnabled(false);
    }
    currentPlayerIndex = i;
    currentPlayerId = id;
    server->sendMessageToAllPlayers(MessageType::PlayerTurn, QString::number(id));
    refreshTurnLabels();
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

void GameScreenWidget::on_cardButton_clicked()
{
    if (isServer)
    {
        Card c = table.getNewCard();
        server->getHostPlayer()->addCard(c);
        ui->CurrentPlayer->addCard(c);
        server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->cardsCount));
        nextPlayerTurn();
    }
    else
    {
        client->sendMessage(MessageType::TakeNewCard, "");
    }
}

void GameScreenWidget::nextPlayerTurn()
{
    currentPlayerIndex++;
    if (currentPlayerIndex >= server->getOtherPlayers().size())
        currentPlayerIndex = -1;
    if (currentPlayerIndex == -1)
    {
        currentPlayerId = 1;
        ui->CurrentPlayer->setEnabled(true);
    }
    else
    {
        currentPlayerId = server->getOtherPlayers().at(currentPlayerIndex)->getId();
        ui->CurrentPlayer->setEnabled(false);
    }
    server->sendMessageToAllPlayers(MessageType::PlayerTurn, QString::number(currentPlayerId));
    log(QString("Tura gracza Index: %1, ID: %2").arg(currentPlayerIndex).arg(currentPlayerId));
    refreshTurnLabels();
}

void GameScreenWidget::refreshTurnLabels()
{
    if (currentPlayerIndex == -1)
    {
        ui->cardButton->setEnabled(true);
        ui->labelCurrentPlayer->setStyleSheet("color: #99FFFF; font-weight: bold;");
        ui->labelPlayer2->setStyleSheet("");
        ui->labelPlayer3->setStyleSheet("");
        ui->labelPlayer4->setStyleSheet("");
    }
    else
    {
        ui->cardButton->setEnabled(false);
        ui->labelCurrentPlayer->setStyleSheet("");
        ui->labelPlayer2->setStyleSheet("");
        ui->labelPlayer3->setStyleSheet("");
        ui->labelPlayer4->setStyleSheet("");
        if (currentPlayerIndex == 0) ui->labelPlayer2->setStyleSheet("color: #99FFFF; font-weight: bold;");
        if (currentPlayerIndex == 1) ui->labelPlayer3->setStyleSheet("color: #99FFFF; font-weight: bold;");
        if (currentPlayerIndex == 2) ui->labelPlayer4->setStyleSheet("color: #99FFFF; font-weight: bold;");
    }
}

void GameScreenWidget::cardClicked(const Card &card, int cardIndex)
{
    if (isServer)
    {
        if (table.CanPlayCard(card))
        {
            ui->CurrentPlayer->setEnabled(false);
            server->getHostPlayer()->removeCard(cardIndex);
            ui->CurrentPlayer->removeCard(cardIndex);
            table.PlayCard(card);
            setTableCard(table.topCard());
            server->sendMessageToAllPlayers(MessageType::SetTableCard, table.topCard().serialize());
            server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->cardsCount));
            nextPlayerTurn();
        }
        else
        {
            QMessageBox msgBox;
            QString msg = QString("Nie możesz teraz zagrać tą kartą.");
            msgBox.setText(msg);
            msgBox.setIcon(QMessageBox::Icon::Warning);
            msgBox.exec();
        }
    }
    else
    {
        ui->CurrentPlayer->setEnabled(false);
        client->sendMessage(MessageType::PlayCard, QString::number(cardIndex));
    }
}

void GameScreenWidget::setTableCard(Card &card)
{
    if (ui->TableCard != NULL) delete ui->TableCard;
    ui->TableCard = new SingleCardWidget(card, this);
    ui->TableCard->move(ui->Game->rect().center() - ui->TableCard->rect().center());
    ui->TableCard->setEnabled(false);
    ui->TableCard->show();
}
