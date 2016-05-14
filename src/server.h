#ifndef SERVER_H
#define SERVER_H

#include <QList>
#include <QTcpServer>
#include "player.h"

class Server : public QTcpServer
{
	Q_OBJECT

private:
	Player *hostPlayer;
	QList<Player*> otherPlayers;
	Player* findPlayer(QTcpSocket* socket);
    int nextId;

public:
	Server(QObject *parent, Player *hostPlayer);
	~Server();
	const Player* getHostPlayer() const { return hostPlayer; }
	const QList<Player*> getOtherPlayers() const { return otherPlayers; }
	void sendMessage(Player const * const player, int messageType, QString message);
	void sendMessage(QTcpSocket* socket, int messageType, QString message);
	void sendMessageToAllPlayers(int messageType, QString message);
    void shutdown();

protected:
	void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private slots:
	void readData();
	void disconnected();
	void socketError(QAbstractSocket::SocketError socketError);

signals:
	void onConnected(Player const * const player);
	void onDisconnected(Player const * const player);
    void afterDisconnected();
	void onError(QAbstractSocket::SocketError socketError);
	void onDataReceived(Player const * const sender, int messageType, QString message);

};

#endif // SERVER_H
