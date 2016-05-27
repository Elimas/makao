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
    Card getNewCard();
    Card topCard();
    void reset4Flag();

private:
    std::deque<Card> cardsStack;
	std::stack<Card> tableCards;
	Card::Suit aceChangedSuit;
	Card::Pip jackRequestedPip;
    bool is4PlayedRecently;

	bool CanPlayCardOnAce(const Card& card) const;
	bool CanPlayCardOnJack(const Card& card) const;
    void generateRandomCards();
};

#endif // GAME_H
