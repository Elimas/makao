#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

class MessageType
{
public:
    static const int WelcomeClientSetId = 1,
		WelcomeServer = 2,
		SetName = 3,
		PlayerConnected = 4,
        PlayerDisconnected = 5,
        PlayerSetNewName = 6;
};

#endif // MESSAGETYPE_H
