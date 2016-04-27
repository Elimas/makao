#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>
#include <QPointer>

class Player : public QObject
{
	Q_OBJECT

friend class Client;
friend class Server;

private:
	QString name;
    QPointer<QTcpSocket> socket;

public:
    Player(QObject *parent, QString name);
	~Player();
    QString getName() const { return name; }
    QPointer<QTcpSocket> getSocket() const { return socket; }
};

#endif // PLAYER_H
