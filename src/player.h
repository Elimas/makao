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
    int id;
	QString name;
    QPointer<QTcpSocket> socket;

public:
    Player(QObject *parent, QString name);
	~Player();
    int getId() const { return id; }
    void setId(int id) { id = id; }
    QString getName() const { return name; }
    QPointer<QTcpSocket> getSocket() const { return socket; }
};

#endif // PLAYER_H
