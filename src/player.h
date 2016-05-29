#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>
#include <QPointer>
#include "card.h"

class Player : public QObject
{
	Q_OBJECT

friend class Client;
friend class Server;

private:
    int id;
	QString name;
    QPointer<QTcpSocket> socket;
    std::vector<Card> cards;

public:
    int cardsCount;
    bool ready;
    int waitingTurns;
    int rankingPosition;

    Player(QObject *parent, QString name);
	~Player();
    int getId() const { return id; }
    void setId(int id) { this->id = id; }
    QString getName() const { return name; }
    QPointer<QTcpSocket> getSocket() const { return socket; }
    void addCard(Card card) { cards.push_back(card); cardsCount++; }
    void removeCard(int index) { cards.erase(cards.begin()+index); cardsCount--; }
    Card& cardAt(int index) { return cards[index]; }
};

#endif // PLAYER_H
