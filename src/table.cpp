#include "table.h"
#include "time.h"
#include <qdebug.h>

Table::Table() : is4PlayedRecently(false), isWar(false), warCards(0)
{
    srand(time(NULL));
    //wrzucamy na stol karte poczatkowa niefunkcyjna 5-10
    if (tableCards.size() == 0) tableCards.push(getNewCard());
    while (static_cast<int>(tableCards.top().getPip()) < static_cast<int>(Card::Pip::Card5)
                            || static_cast<int>(tableCards.top().getPip()) > static_cast<int>(Card::Pip::Card10))
    {
        tableCards.push(getNewCard());
    }
}

bool Table::CanPlayCard(const Card& card) const
{
	if(tableCards.empty())
	{
		return true;
	}

	const Card& cardOnTop = tableCards.top();

    /* Wojna:
     * Król Pik (Spades) - poprzedni gracz bierze 5 kart (jak gracz da dowolnego króla do wojna się kończy i karty zerują)
     * Król Kier (Hearts) - następny gracz bierze 5 kart (jak gracz da dowolnego króla do wojna się kończy i karty zerują)
     * 2 - dodaje 2 karty
     * 3 - dodaje 3 karty
     * Na 2 lub 3 można dać króla: Król Pik (Spades) lub Król Kier (Hearts) dodaje 5 dodatkowych kart.
     * Na króla można dać tylko króla który kończy wojnę.
     */
    if (isWar)
    {
        if (cardOnTop.getPip() == Card::Pip::Card2 || cardOnTop.getPip() == Card::Pip::Card3)
        {
            if (card.getPip() == Card::Pip::Card2 || card.getPip() == Card::Pip::Card3 || card.getPip() == Card::Pip::King) return true;
            else return false;
        }
        if (cardOnTop.getPip() == Card::Pip::King)
        {
            if (card.getPip() == Card::Pip::King) return true;
            else return false;
        }
    }

    //na 4 można dawać tylko 4, kto nie da czeka tyle kolejek ile było 4
    if(is4PlayedRecently)
    {
        if (card.getPip() == Card::Pip::Card4) return true;
        else return false;
    }
	
    /* Dama na wszystko, Wszystko na dame */
    if(cardOnTop.getPip() == Card::Pip::Queen || card.getPip() == Card::Pip::Queen)
	{
		return true;
	}

	if(cardOnTop.getPip() == Card::Pip::Ace)
	{
		return CanPlayCardOnAce(card);
	}

	if(cardOnTop.getPip() == Card::Pip::Jack)
	{
		return CanPlayCardOnJack(card);
	}

	if (cardOnTop.getSuit() == card.getSuit() || cardOnTop.getPip() == card.getPip())
	{
		return true;
	}

	return false;
}

void Table::PlayCard(const Card& card)
{
	if (!CanPlayCard(card))
	{
		throw std::invalid_argument("This card can't be played now.");
	}

	tableCards.push(card);
    if (card.getPip() == Card::Pip::Card4) is4PlayedRecently = true;
    if (card.getPip() == Card::Pip::Card2 || card.getPip() == Card::Pip::Card3 || card.getPip() == Card::Pip::King)
    {
        if (!isWar)
        {
            isWar = true;
            if (card.getPip() == Card::Pip::Card2) warCards += 2;
            else if (card.getPip() == Card::Pip::Card3) warCards += 3;
            else if (card.getPip() == Card::Pip::King && (card.getSuit() == Card::Suit::Spade || card.getSuit() == Card::Suit::Heart)) warCards += 5;
            qDebug() << "Początek wojny. Kart: " << warCards;
        }
        else
        {
            if (card.getPip() == Card::Pip::Card2) { warCards += 2; qDebug() << "Wojna l. kart: " << warCards; }
            else if (card.getPip() == Card::Pip::Card3) { warCards += 3; qDebug() << "Wojna l. kart: " << warCards; }
            else if (card.getPip() == Card::Pip::King) { warCards = 0; isWar = false; qDebug() << "Koniec wojny"; }
        }
    }
}

void Table::PlayAce(const Card& aceCard, Card::Suit changedToSuit)
{
	aceChangedSuit = changedToSuit;
	PlayCard(aceCard);
}

void Table::PlayJack(const Card& jackCard, Card::Pip requestedPip)
{
	jackRequestedPip = requestedPip;
	PlayCard(jackCard);
}

bool Table::CanPlayCardOnAce(const Card& card) const
{
	return aceChangedSuit == card.getSuit() || card.getPip() == Card::Pip::Ace;
}

bool Table::CanPlayCardOnJack(const Card& card) const
{
	//TODO: walet zada od wszystkich graczy (trzeba zebrac karty od wszystkich)
	//TODO: mozna nie zadac niczego
	return jackRequestedPip == card.getPip() || card.getPip() == Card::Pip::Jack;
}

//ściaga kartę ze stosu kart
Card Table::getNewCard()
{
    if (cardsStack.size() == 0) generateRandomCards();
    Card card = cardsStack.back();
    cardsStack.pop_back();
    return card;
}

//czyści obecny stos kart do wzięcia generuje w jego miejsce nowy stos który jest polosowany
void Table::generateRandomCards()
{
    cardsStack.empty();
    for (int cardSuit = static_cast<int>(Card::Suit::Heart); cardSuit <= static_cast<int>(Card::Suit::Spade); cardSuit++)
    {
        for (int cardPip = static_cast<int>(Card::Pip::Card2); cardPip <= static_cast<int>(Card::Pip::Ace); cardPip++)
        {
            cardsStack.push_back(Card(static_cast<Card::Suit>(cardSuit), static_cast<Card::Pip>(cardPip)));
        }
    }
    std::random_shuffle(cardsStack.begin(), cardsStack.end());
}

Card Table::topCard()
{
    return tableCards.top();
}

void Table::reset4Flag()
{
    is4PlayedRecently = false;
}

void Table::setAceChangedSuit(Card::Suit suit)
{
	aceChangedSuit = suit;
}
