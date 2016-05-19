#include "player.h"

Player::Player(QObject *parent, QString name) :
    QObject(parent), name(name), socket(new QTcpSocket(this)), id(0), cardsCount(0), ready(false)
{

}

Player::~Player()
{
    if (!socket.isNull() && socket->state() == QTcpSocket::ConnectedState)
	{
        socket->disconnectFromHost();
		socket->waitForDisconnected();
	}
    if (!socket.isNull()) socket->deleteLater();
}
