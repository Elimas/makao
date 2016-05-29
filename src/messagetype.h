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
        PlayerSetNewName = 6,
        StartGame = 7,
        SetTableCard = 8, //[S] wysylane do wszystkich graczy ustawia obecna karta na stole
        AddCard = 9, //[S] dodaje karte do talii gracza: SUIT_ID;PIP_ID
        RemoveCard = 10, //[S] usuwa karte z talii gracza: NR_KARTY
        TakeNewCard = 11, //[C] żądanie gracza do pociągniecia dodatkowej karty / jeżeli rzucono 4 powoduje czekanie klienta tyle tur ile było czwórek
        PlayCard = 12, //[C] żądanie gracza do zagrania kartą: NR_KARTY
        PlayerTurn = 13, //[S] wysyłane do wszystkich graczy id gracza który ma teraz turę: PLAYER_ID
        CardsNumber = 14, //[S] ilość kart które ma dany gracz: PLAYER_ID;ILOSC_KART
        Ready = 15, //[C] wysyła gracz po załadowaniu ekranu gry
        CantPlayCard = 16, //[S] wysyła server gdy klient nie może zagrać wybranej karty: NR_KARTY
        ResetTurnWaitCounter = 17, //[S] wysyła gdy ktoś czeka na kartach 4, powoduje reset licznika czkeania u klientów

        RequestedCard = 19; //[C] wysyła gracz po zażądaniu waletem
};

#endif // MESSAGETYPE_H
