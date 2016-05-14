#ifndef GAME_H
#define GAME_H
#include <stack>
#include "card.h"

class Table
{
public:
    Table();
	bool CanPlayCard(const Card& card) const;
	void PlayCard(const Card& card);
	void PlayAce(const Card& aceCard, Card::Suit changedToSuit);
	void PlayJack(const Card& jackCard, Card::Pip requestedPip);

private:
	std::stack<Card> tableCards;
	Card::Suit aceChangedSuit;
	Card::Pip jackRequestedPip;

	bool CanPlayCardOnAce(const Card& card) const;
	bool CanPlayCardOnJack(const Card& card) const;
};

#endif // GAME_H