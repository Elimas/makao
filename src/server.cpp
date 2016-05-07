#include "server.h"
#include "messagetype.h"
#include <QtNetwork>
#include <QTcpSocket>

Server::Server(QObject *parent, Player *player)
    : QTcpServer(parent), hostPlayer(player), nextId(2)
{
	QTime now = QTime::currentTime();
	qsrand(now.msec());
    hostPlayer->id = 1;
}

Server::~Server()
{
    shutdown();
    if (hostPlayer != NULL) delete hostPlayer;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
	QTcpSocket *socket = new QTcpSocket(this);
	if (!socket->setSocketDescriptor(socketDescriptor)) {
		qDebug() << socket->error();
		return;
	}
	connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
	typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
	connect(socket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &Server::socketError);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    Player *player = new Player(this, QString("player%1").arg(qrand() % 10000));
    player->id = nextId;
    nextId++;
	player->socket = socket;
    sendMessageToAllPlayers(MessageType::PlayerConnected, QString("%1;%2").arg(player->id).arg(player->name));
	otherPlayers.append(player);
    qDebug() << "Player " << player->name << " connected to server";
    emit onConnected(player);
    sendMessage(socket, MessageType::WelcomeClientSetId, QString::number(player->id));
}

void Server::sendMessage(Player const * const player, int messageType, QString message)
{
	sendMessage(player->socket, messageType, message);
}

void Server::sendMessage(QTcpSocket* socket, int messageType, QString message)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << (quint16)0;
	out << messageType;
	out << message;
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));
	socket->write(block);
}

void Server::sendMessageToAllPlayers(int messageType, QString message)
{
	QList<Player*>::const_iterator it;
	for (it = otherPlayers.begin(); it != otherPlayers.end(); ++it)
		sendMessage((*it), messageType, message);
}

void Server::readData()
{
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
	QDataStream in(socket);
	in.setVersion(QDataStream::Qt_4_0);

    while (socket->bytesAvailable() > 0)
    {
        quint16 blockSize = 0;
        if (blockSize == 0) {
            if (socket->bytesAvailable() < (int)sizeof(quint16))
                return;
            in >> blockSize;
        }

        if (socket->bytesAvailable() < blockSize)
            return;

        int messageType;
        QString message;
        in >> messageType;
        in >> message;

        Player* sender = findPlayer(socket);
        qDebug() << "New message from client " << sender->name;
        qDebug() << "Type: " << messageType << " Message: " << message;
        if (messageType == MessageType::SetName)
        {
            sender->name = message;
            qDebug() << "Player set new name " << message;
            //send list of players to this player
            sendMessage(sender, MessageType::PlayerConnected, QString("%1;%2").arg(hostPlayer->id).arg(hostPlayer->name));
            QList<Player*>::const_iterator it;
            for (it = otherPlayers.begin(); it != otherPlayers.end(); ++it)
                if (sender != (*it)) sendMessage(sender, MessageType::PlayerConnected, QString("%1;%2").arg((*it)->id).arg((*it)->name));

            //send this player to other players
            for (it = otherPlayers.begin(); it != otherPlayers.end(); ++it)
                if (sender != (*it)) sendMessage((*it), MessageType::PlayerSetNewName, QString("%1;%2").arg(sender->id).arg(sender->name));
        }
        emit onDataReceived(sender, messageType, message);
    }
}

void Server::disconnected()
{
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
	Player* player = findPlayer(socket);
	qDebug() << "Player " << player->name << " disconnected from server";
    emit onDisconnected(player);
    QMutableListIterator<Player*> it(otherPlayers);
    while (it.hasNext())
    {
        if (it.next() == player) it.remove();
    }
    sendMessageToAllPlayers(MessageType::PlayerDisconnected, QString::number(player->id));
    player->socket->disconnect(player->socket, 0, 0, 0);
    player->deleteLater();
    emit afterDisconnected();
}

void Server::socketError(QAbstractSocket::SocketError socketError)
{
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
	qDebug() << "Server socket error: " << socket->errorString();
	emit onError(socketError);
}

Player* Server::findPlayer(QTcpSocket* socket)
{
	QList<Player*>::const_iterator it;
	for (it = otherPlayers.begin(); it != otherPlayers.end(); ++it)
		if ((*it)->socket == socket) return (*it);
	return NULL;
}

void Server::shutdown()
{
    qDebug() << "Shutting server down";
    close();
    QMutableListIterator<Player*> it(otherPlayers);
    while (it.hasNext())
    {
        Player* p = it.next();
        p->deleteLater();
        it.remove();
    }
    otherPlayers.clear();
}
