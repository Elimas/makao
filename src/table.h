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
    bool isWar;
    int warCards;

	bool CanPlayCardOnAce(const Card& card) const;
	bool CanPlayCardOnJack(const Card& card) const;
    void generateRandomCards();

public:
    bool getIsWar() const { return isWar; }
    int getWarCardsCount() const { return warCards; }
    void resetWarFlag() { isWar = false; warCards = 0; }
};

#endif // GAME_H
