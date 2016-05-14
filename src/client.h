#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include "player.h"

class Client : public QObject
{
	Q_OBJECT

private:
	Player *player;
    QList<Player*> otherPlayers;

public:
    Client(QObject *parent, Player *player);
    ~Client();
	void connectToHost(QString ip, quint16 port);
	bool isConnected();
	void sendMessage(int messageType, QString message);
    void disconnectFromHost();
	const QTcpSocket* getSocket() const { return player->socket; }
	const Player* getPlayer() const { return player; }
    QList<Player*> getOtherPlayers() { return otherPlayers; }

signals:
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError socketError);
	void onDataReceived(int messageType, QString message);

private slots:
	void readData();
	void connected();
	void disconnected();
	void socketError(QAbstractSocket::SocketError socketError);
};

#endif // CLIENT_H
