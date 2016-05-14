#include "table.h"

Table::Table()
{

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
