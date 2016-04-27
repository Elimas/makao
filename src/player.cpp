#include "player.h"

Player::Player(QObject *parent, QString name) :
    QObject(parent), name(name), socket(new QTcpSocket(this))
{

}

Player::~Player()
{
    if (!socket.isNull() && socket->state() == QTcpSocket::ConnectedState)
	{
        qDebug() << 2;
        socket->disconnectFromHost();
		socket->waitForDisconnected();
        qDebug() << 3;
	}
    if (!socket.isNull()) socket->deleteLater();
}
