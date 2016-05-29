#include "gamescreenwidget.h"
#include "ui_gamescreenwidget.h"
#include "messagetype.h"
#include "opponentcardswidget.h"
#include <QDesktopWidget>
#include "time.h"
#include <QMessageBox>
#include "utils.h"
#include "custombuttonsdialog.h"

GameScreenWidget::GameScreenWidget(QWidget *parent, Server *server, Client *client, bool isServer) :
    QWidget(parent), server(server), client(client), isServer(isServer), currentPlayerId(0), currentPlayerIndex(0), waitTurns(0), rankingPosition(1),
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
    Utils::showNotBlockingMessageBox(NULL, QString("Gracz się rozłączył"), QString("Jeden z graczy się rozłączył przez co nie można kontynuować gry. Wracanie do menu."), QMessageBox::Icon::Critical);
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
            if (waitTurns > 0)
            {
                sender->waitingTurns = waitTurns;
                table.reset4Flag();
                waitTurns = 0;
                refreshCardButton();
                server->sendMessageToAllPlayers(MessageType::ResetTurnWaitCounter, "");
            }
            else
            {
                int cardsCount = 1;
                if (table.getIsWar())
                {
                    cardsCount = table.getWarCardsCount();
                    table.resetWarFlag();
                    log(QString("Gracz: %1 przegrał wojnę. Dostaje %2 kart").arg(sender->getName()).arg(cardsCount));
                }

                for (int i = 0; i < cardsCount; i++)
                {
                    Card c = table.getNewCard();
                    sender->addCard(c);
                    server->sendMessage(sender, MessageType::AddCard, c.serialize());
                }
                server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(sender->getId()).arg(sender->cardsCount));
                int i = server->getOtherPlayers().indexOf(sender);
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
                    o->setCardsNumber(sender->cardsCount);
                }
            }
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
                log(QString("Gracz %1 gra kartą %2 %3").arg(sender->getName()).arg(card.getSuitAsText()).arg(card.getPipAsText()));
                if (card.getPip() == Card::Pip::Card4)
                {
                    waitTurns++;
                    refreshCardButton();
                }
                table.PlayCard(card);
                sender->removeCard(index);
                setTableCard(table.topCard());
                server->sendMessageToAllPlayers(MessageType::SetTableCard, table.topCard().serialize());
                server->sendMessage(sender, MessageType::RemoveCard, QString::number(index));
                server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(sender->getId()).arg(sender->cardsCount));
                for (int i = 0; i < server->getOtherPlayers().size(); i++)
                {
                    Player* p = server->getOtherPlayers().at(i);
                    if (sender->getId() == p->getId())
                    {
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
                        break;
                    }
                }

                if (sender->cardsCount == 0)
                {
                    sender->rankingPosition = rankingPosition;
                    if (rankingPosition < server->getOtherPlayers().size() + 1)
                    {
                        server->sendMessageToAllPlayers(MessageType::PlayerWin, QString("%1;%2;%3").arg(sender->getId()).arg(sender->getName()).arg(rankingPosition));
                        if (rankingPosition == server->getOtherPlayers().size())
                        {
                            //został jeden gracz
                            if (server->getHostPlayer()->rankingPosition == 0)
                            {
                                rankingPosition++;
                                server->getHostPlayer()->rankingPosition = rankingPosition;
                                Utils::showNotBlockingMessageBox(NULL, QString("Koniec gry"), QString("Przegrałeś! Masz %1 miejsce na %2 graczy.").arg(rankingPosition).arg(server->getOtherPlayers().size() + 1), QMessageBox::Icon::Information);
                                server->sendMessageToAllPlayers(MessageType::PlayerWin, QString("%1;%2;%3").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->getName()).arg(rankingPosition));
                                ui->cardButton->setEnabled(false);
                                ui->CurrentPlayer->setEnabled(false);
                            }
                            else
                            {
                                for (int i = 0; i < server->getOtherPlayers().size(); i++)
                                {
                                    Player* p = server->getOtherPlayers().at(i);
                                    if (p->rankingPosition == 0)
                                    {
                                        p->rankingPosition = rankingPosition + 1;
                                        server->sendMessageToAllPlayers(MessageType::PlayerWin, QString("%1;%2;%3").arg(p->getId()).arg(p->getName()).arg(p->rankingPosition));
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        server->sendMessageToAllPlayers(MessageType::PlayerWin, QString("%1;%2;%3").arg(sender->getId()).arg(sender->getName()).arg(rankingPosition));
                    }
                    rankingPosition++;
                    refreshRanking();
                }

                if (card.getPip() == Card::Pip::King && card.getSuit() == Card::Suit::Spade)
                {
                    previousPlayerTurn();
                }
                else
                {
                    nextPlayerTurn();
                }
            }
            else
            {
                server->sendMessage(sender, MessageType::CantPlayCard, QString::number(index));
            }
        }
    }
	else if (messageType == MessageType::AceChangedSuit)
	{
		handleAceSuitChangedOnServer(message);
		server->sendMessageToAllPlayers(MessageType::AceChangedSuit, message);
		Utils::showNotBlockingMessageBox(nullptr, "Zagrano asem", "Zmieniono kolor na: " + message, QMessageBox::Icon::Information);
	}
}
//slots in client
void GameScreenWidget::onClientDisconnected()
{
    /*QMessageBox msgBox;
    QString msg = QString("Utracono połączenie z hostem. Wracanie do menu.");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Icon::Critical);
    msgBox.exec();*/
    Utils::showNotBlockingMessageBox(NULL, QString("Utracono połączenie"), QString("Utracono połączenie z hostem. Wracanie do menu."), QMessageBox::Icon::Critical);
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
        Card card = Card(static_cast<Card::Suit>(a.value(0).toInt()), static_cast<Card::Pip>(a.value(1).toInt()));
        if (card.getPip() == Card::Pip::Card4)
        {
            waitTurns++;
            refreshCardButton();
        }
        setTableCard(card);
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
        Utils::showNotBlockingMessageBox(NULL, QString("Nie możesz teraz zagrać tą kartą."), QString("Nie możesz teraz zagrać tą kartą."), QMessageBox::Icon::Warning);
        ui->CurrentPlayer->setEnabled(true);
    }
    else if (messageType == MessageType::ResetTurnWaitCounter)
    {
        waitTurns = 0;
        refreshCardButton();
    }
    else if (messageType == MessageType::PlayerWin)
    {
        QStringList l = message.split(";");
        int playerId = l.value(0).toInt();
        QString playerName = l.value(1);
        int playerRanking = l.value(2).toInt();
        if (client->getPlayer()->getId() == playerId)
        {
            client->getPlayer()->rankingPosition = playerRanking;
            if (playerRanking < client->getOtherPlayers().size() + 1)
            {
                Utils::showNotBlockingMessageBox(NULL, QString("Koniec gry"), QString("Gratulacje! Wygrałeś! Masz %1 miejsce na %2 graczy.").arg(playerRanking).arg(client->getOtherPlayers().size() + 1), QMessageBox::Icon::Information);
            }
            else
            {
                Utils::showNotBlockingMessageBox(NULL, QString("Koniec gry"), QString("Przegrałeś! Masz %1 miejsce na %2 graczy.").arg(playerRanking).arg(client->getOtherPlayers().size() + 1), QMessageBox::Icon::Information);
            }
            ui->cardButton->setEnabled(false);
            ui->CurrentPlayer->setEnabled(false);
        }
        else
        {
            for (int i = 0; i < client->getOtherPlayers().size(); i++)
            {
                Player* p = client->getOtherPlayers().at(i);
                if (p->getId() == playerId)
                {
                   p->rankingPosition = playerRanking;
                   break;
                }
            }
        }
        refreshRanking();
    }
    	else if (messageType == MessageType::AceChangedSuit)
	{
		Utils::showNotBlockingMessageBox(nullptr, "Zagrano asem", "Zmieniono kolor na: " + message, QMessageBox::Icon::Information);
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
    bool allPlayersEnded = true;
    if (isServer)
    {
        if (server->getHostPlayer()->rankingPosition == 0) allPlayersEnded = false;
        for (int i = 0; i < server->getOtherPlayers().size(); i++)
        {
            if (server->getOtherPlayers().at(i)->rankingPosition == 0) allPlayersEnded = false;
        }
    }
    else
    {
        if (client->getPlayer()->rankingPosition == 0) allPlayersEnded = false;
        for (int i = 0; i < client->getOtherPlayers().size(); i++)
        {
            if (client->getOtherPlayers().at(i)->rankingPosition == 0) allPlayersEnded = false;
        }
    }
    if (!allPlayersEnded)
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
    else
    {
        this->hide();
        this->deleteLater();
    }
}

void GameScreenWidget::on_cardButton_clicked()
{
    if (isServer)
    {
        if (waitTurns > 0)
        {
            server->getHostPlayer()->waitingTurns = waitTurns;
            table.reset4Flag();
            waitTurns = 0;
            refreshCardButton();
            server->sendMessageToAllPlayers(MessageType::ResetTurnWaitCounter, "");
        }
        else
        {
            int cardsCount = 1;
            if (table.getIsWar())
            {
                cardsCount = table.getWarCardsCount();
                table.resetWarFlag();
                log(QString("Gracz: %1 przegrał wojnę. Dostaje %2 kart").arg(server->getHostPlayer()->getName()).arg(cardsCount));
            }
            for (int i = 0; i < cardsCount; i++)
            {
                Card c = table.getNewCard();
                server->getHostPlayer()->addCard(c);
                ui->CurrentPlayer->addCard(c);
                server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->cardsCount));
            }
        }
        nextPlayerTurn();
    }
    else
    {
        client->sendMessage(MessageType::TakeNewCard, "");
        if (waitTurns > 0)
        {
            waitTurns = 0;
            refreshCardButton();
        }
    }
}

void GameScreenWidget::nextPlayerTurn()
{
    log("[DEBUG] nextPlayerTurn()");
    log("[DEBUG] other players size: " + QString::number(server->getOtherPlayers().size()));
    if (rankingPosition < server->getOtherPlayers().size() + 1)
    {
        currentPlayerIndex++;
        if (currentPlayerIndex >= server->getOtherPlayers().size())
            currentPlayerIndex = -1;
        Player* currentPlayer = NULL;
        if (currentPlayerIndex == -1)
        {
            currentPlayerId = 1;
            ui->CurrentPlayer->setEnabled(true);
            currentPlayer = server->getHostPlayer();
        }
        else
        {
            currentPlayerId = server->getOtherPlayers().at(currentPlayerIndex)->getId();
            ui->CurrentPlayer->setEnabled(false);
            currentPlayer = server->getOtherPlayers().at(currentPlayerIndex);
        }
        if (currentPlayer->rankingPosition > 0)
        {
            ui->CurrentPlayer->setEnabled(false);
            nextPlayerTurn();
        }
        if (currentPlayer->waitingTurns > 0)
        {
            log(QString("Gracz %1 czeka teraz %2 turę").arg(currentPlayer->getName()).arg(currentPlayer->waitingTurns));
            currentPlayer->waitingTurns--;
            nextPlayerTurn();
        }
        else
        {
            server->sendMessageToAllPlayers(MessageType::PlayerTurn, QString::number(currentPlayerId));
            log(QString("Tura gracza Index: %1, ID: %2, Nick: %3").arg(currentPlayerIndex).arg(currentPlayerId).arg(currentPlayer->getName()));
            refreshTurnLabels();
        }
    }
}

//używane jeśli jest karta: Król Pik (Spades)
void GameScreenWidget::previousPlayerTurn()
{
    if (rankingPosition < server->getOtherPlayers().size() + 1)
    {
        currentPlayerIndex--;
        if (currentPlayerIndex < -1)
            currentPlayerIndex = server->getOtherPlayers().size() - 1;
        Player* currentPlayer = NULL;
        if (currentPlayerIndex == -1)
        {
            currentPlayerId = 1;
            ui->CurrentPlayer->setEnabled(true);
            currentPlayer = server->getHostPlayer();
        }
        else
        {
            currentPlayerId = server->getOtherPlayers().at(currentPlayerIndex)->getId();
            ui->CurrentPlayer->setEnabled(false);
            currentPlayer = server->getOtherPlayers().at(currentPlayerIndex);
        }
        if (currentPlayer->rankingPosition > 0)
        {
            ui->CurrentPlayer->setEnabled(false);
            previousPlayerTurn();
        }
        if (currentPlayer->waitingTurns > 0)
        {
            log(QString("Gracz %1 czeka teraz %2 turę").arg(currentPlayer->getName()).arg(currentPlayer->waitingTurns));
            currentPlayer->waitingTurns--;
            previousPlayerTurn();
        }
        else
        {
            server->sendMessageToAllPlayers(MessageType::PlayerTurn, QString::number(currentPlayerId));
            log(QString("Tura gracza Index: %1, ID: %2, Nick: %3").arg(currentPlayerIndex).arg(currentPlayerId).arg(currentPlayer->getName()));
            refreshTurnLabels();
        }
    }
}

void GameScreenWidget::refreshTurnLabels()
{
    Player* currentPlayer = NULL;
    if (isServer)
    {
        if (currentPlayerIndex == -1) currentPlayer = server->getHostPlayer();
        else currentPlayer = server->getOtherPlayers().at(currentPlayerIndex);
    }
    else
    {
        if (currentPlayerIndex == -1) currentPlayer = client->getPlayer();
        else currentPlayer = client->getOtherPlayers().at(currentPlayerIndex);
    }
    if (currentPlayerIndex == -1)
    {
        if (currentPlayer->rankingPosition == 0) ui->cardButton->setEnabled(true);
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
    if(card.getPip() == Card::Pip::Ace)
    {
        handleAce();
    }

    if (isServer)
    {
        if (table.CanPlayCard(card))
        {
            ui->CurrentPlayer->setEnabled(false);
            if (card.getPip() == Card::Pip::Card4)
            {
                waitTurns++;
                refreshCardButton();
            }
            server->getHostPlayer()->removeCard(cardIndex);
            ui->CurrentPlayer->removeCard(cardIndex);
            table.PlayCard(card);
            setTableCard(table.topCard());
            server->sendMessageToAllPlayers(MessageType::SetTableCard, table.topCard().serialize());
            server->sendMessageToAllPlayers(MessageType::CardsNumber, QString("%1;%2").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->cardsCount));
            if (server->getHostPlayer()->cardsCount == 0)
            {
                QString rankingStr =  QString(" ( miejsce %1)").arg(rankingPosition);
                ui->labelCurrentPlayer->setText(server->getHostPlayer()->getName() + rankingStr);
                server->getHostPlayer()->rankingPosition = rankingPosition;
                if (rankingPosition < server->getOtherPlayers().size() + 1)
                {
                    Utils::showNotBlockingMessageBox(NULL, QString("Koniec gry"), QString("Gratulacje! Wygrałeś! Masz %1 miejsce na %2 graczy.").arg(rankingPosition).arg(server->getOtherPlayers().size() + 1), QMessageBox::Icon::Information);
                    server->sendMessageToAllPlayers(MessageType::PlayerWin, QString("%1;%2;%3").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->getName()).arg(rankingPosition));
                    if (rankingPosition == server->getOtherPlayers().size())
                    {
                        //został jeden gracz
                        for (int i = 0; i < server->getOtherPlayers().size(); i++)
                        {
                            Player* p = server->getOtherPlayers().at(i);
                            if (p->rankingPosition == 0)
                            {
                                rankingPosition++;
                                p->rankingPosition = rankingPosition;
                                server->sendMessageToAllPlayers(MessageType::PlayerWin, QString("%1;%2;%3").arg(p->getId()).arg(p->getName()).arg(p->rankingPosition));
                                break;
                            }
                        }
                    }
                }
                else
                {
                    Utils::showNotBlockingMessageBox(NULL, QString("Koniec gry"), QString("Przegrałeś! Masz %1 miejsce na %2 graczy.").arg(rankingPosition).arg(server->getOtherPlayers().size() + 1), QMessageBox::Icon::Information);
                    server->sendMessageToAllPlayers(MessageType::PlayerWin, QString("%1;%2;%3").arg(server->getHostPlayer()->getId()).arg(server->getHostPlayer()->getName()).arg(rankingPosition));
                }
                rankingPosition++;
                refreshRanking();
                ui->cardButton->setEnabled(false);
                ui->CurrentPlayer->setEnabled(false);
            }
            if (card.getPip() == Card::Pip::King && card.getSuit() == Card::Suit::Spade)
            {
                previousPlayerTurn();
            }
            else
            {
                nextPlayerTurn();
            }
        }
        else
        {
            Utils::showNotBlockingMessageBox(NULL, QString("Nie możesz teraz zagrać tą kartą."), QString("Nie możesz teraz zagrać tą kartą."), QMessageBox::Icon::Warning);
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

void GameScreenWidget::refreshCardButton()
{
    if (waitTurns > 0)
    {
        ui->cardButton->setText(QString("Czekaj %1 tur").arg(waitTurns));
    }
    else
    {
        ui->cardButton->setText(QString("Ciągnij kartę"));
    }
}

void GameScreenWidget::refreshRanking()
{
    Player* currentPlayer = NULL;
    QList<Player*>* players;
    if (isServer)
    {
        currentPlayer = server->getHostPlayer();
        players = server->getOtherPlayersPtr();
    }
    else
    {
        currentPlayer = client->getPlayer();
        players = client->getOtherPlayersPtr();
    }

    if (currentPlayer->rankingPosition > 0)
    {
        QString str = QString(" (miejsce %1)").arg(currentPlayer->rankingPosition);
        ui->labelCurrentPlayer->setText(currentPlayer->getName() + str);
    }
    for (int i = 0; i < players->size(); i++)
    {
        Player* p = players->at(i);
        log(QString("PlayerId: %1, Ranking: %2").arg(p->getId()).arg(p->rankingPosition));
        if (p->rankingPosition > 0)
        {
            QString str = QString(" (miejsce %1)").arg(p->rankingPosition);
            switch(i)
            {
            case 0:
               ui->labelPlayer2->setText(p->getName() + str);
               break;
            case 1:
               ui->labelPlayer3->setText(p->getName() + str);
               break;
            case 2:
               ui->labelPlayer4->setText(p->getName() + str);
               break;
            }
        }
    }
}

const QString heartText = "Kier";
const QString diamondText = "Karo";
const QString clubText = "Trefl";
const QString spadeText = "Pik";

void GameScreenWidget::handleAce()
{
    CustomButtonsDialog buttonsDialog;
	buttonsDialog.setMessage("Na jaki kolor zmieniasz?");
    
    buttonsDialog.addButton(heartText);
    buttonsDialog.addButton(diamondText);
    buttonsDialog.addButton(clubText);
    buttonsDialog.addButton(spadeText);
    buttonsDialog.exec();

	if(isServer)
	{
		handleAceSuitChangedOnServer(buttonsDialog.getSelectedButtonText());
		server->sendMessageToAllPlayers(MessageType::AceChangedSuit, buttonsDialog.getSelectedButtonText());
	}
	else
	{
		client->sendMessage(MessageType::AceChangedSuit, buttonsDialog.getSelectedButtonText());
	}
}

void GameScreenWidget::handleAceSuitChangedOnServer(const QString& suitName)
{
	if(suitName == heartText)
	{
		table.setAceChangedSuit(Card::Suit::Heart);
	} 
	else if(suitName == diamondText)
	{
		table.setAceChangedSuit(Card::Suit::Diamond);
	}
	else if (suitName == clubText)
	{
		table.setAceChangedSuit(Card::Suit::Club);
	}
	else if (suitName == spadeText)
	{
		table.setAceChangedSuit(Card::Suit::Spade);
	}
}
