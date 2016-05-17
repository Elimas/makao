#include "table.h"
#include "time.h"
#include <qdebug.h>

Table::Table()
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
	
	/* Wszystko na dame */
	if(cardOnTop.getPip() == Card::Pip::Queen)
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



