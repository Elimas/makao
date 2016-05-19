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

public:
    int cardsCount;
    QList<Card> cards;
    bool ready;

    Player(QObject *parent, QString name);
	~Player();
    int getId() const { return id; }
    void setId(int id) { id = id; }
    QString getName() const { return name; }
    QPointer<QTcpSocket> getSocket() const { return socket; }
    void addCard(Card card) { cards.append(card); cardsCount++; }
    void removeCard(int index) { cards.removeAt(index); cardsCount--; }
};

#endif // PLAYER_H
