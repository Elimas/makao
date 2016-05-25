#include "client.h"
#include "messagetype.h"
#include <QDataStream>

Client::Client(QObject *parent, Player *player) :
    QObject(parent), player(player), connectedB (false)
{
    connect(player->socket, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(player->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(player->socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(player->socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

Client::~Client()
{
    disconnectFromHost();
    if (player) player->deleteLater();
}

void Client::connectToHost(QString ip, quint16 port)
{
    player->socket->abort();
	player->socket->connectToHost(ip, port);
}

bool Client::isConnected()
{
	return player->socket->state() == QTcpSocket::ConnectedState;
}

void Client::sendMessage(int messageType, QString message)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << (quint16)0;
	out << messageType;
	out << message;
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));
	player->socket->write(block);
}

void Client::readData()
{
	QDataStream in(player->socket);
	in.setVersion(QDataStream::Qt_4_0);

    while (player->socket->bytesAvailable() > 0)
    {
        quint16 blockSize = 0;
        if (blockSize == 0) {
            if (player->socket->bytesAvailable() < (int)sizeof(quint16))
                return;
            in >> blockSize;
        }

        if (player->socket->bytesAvailable() < blockSize)
            return;

        int messageType;
        QString message;
        in >> messageType;
        in >> message;

        qDebug() << "New message from server";
        qDebug() << "Type: " << messageType << " Message: " << message;
        if (messageType == MessageType::WelcomeClientSetId)
        {
            player->setId(message.toInt());
            sendMessage(MessageType::SetName, player->name);
        }
        else if (messageType == MessageType::PlayerConnected)
        {
            QStringList args = message.split(";");
            if (args.length() == 2)
            {
                int id = args.value(0).toInt();
                QString name = args.value(1);
                Player *newPlayer = new Player(this, name);
                newPlayer->id = id;
                otherPlayers.append(newPlayer);
                qDebug() << "New player connected #" << newPlayer->id << " " << newPlayer->name;
            }
        }
        else if (messageType == MessageType::PlayerSetNewName)
        {
            QStringList args = message.split(";");
            if (args.length() == 2)
            {
                int id = args.value(0).toInt();
                QString name = args.value(1);
                QList<Player*>::const_iterator it;
                for (it = otherPlayers.begin(); it != otherPlayers.end(); ++it)
                {
                    if ((*it)->id == id)
                    {
                        (*it)->name = name;
                    }
                }
                qDebug() << "Player #" << id << " changed name to " << name;
            }
        }
        else if (messageType == MessageType::PlayerDisconnected)
        {
            int id = message.toInt();
            QMutableListIterator<Player*> it(otherPlayers);
            while (it.hasNext())
            {
                Player* p = it.next();
                if (id == p->id)
                {
                    qDebug() << "Player #" << id << " (" << p->name << ") disconnected";
                    it.remove();
                }
            }
        }
        emit onDataReceived(messageType, message);
    }
}

void Client::connected()
{
    connectedB = true;
	qDebug() << "Connected to server";
	emit onConnected();
}

void Client::disconnected()
{
    connectedB = false;
	qDebug() << "Disconnected from server";
	emit onDisconnected();
}

void Client::socketError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Client socket error: " << player->socket->errorString();
	emit onError(socketError);
}

void Client::disconnectFromHost()
{
    if (connectedB && player->socket->state() != QTcpSocket::UnconnectedState)
    {
        player->socket->disconnectFromHost();
        player->socket->waitForDisconnected();
    }
}
