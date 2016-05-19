#ifndef CARD_H
#define CARD_H

#include <QString>

class Card
{
public:
    enum class Suit
    {
        Heart,
        Diamond,
        Club,
        Spade
    };

    enum class Pip
    {
        Card2,
        Card3,
        Card4,
        Card5,
        Card6,
        Card7,
        Card8,
        Card9,
        Card10,
        Jack,
        Queen,
        King,
        Ace
    };

    enum class Color
    {
        Black,
        Red
    };

    Card(const Suit suit, const Pip pip);
    Card(const Card& card);
    Suit getSuit() const;

    Pip getPip() const;

    Color getColor() const;

    QString getSuitAsText() const;
    QString getPipAsText() const;

    int getSuitAsInt() const { return static_cast<int>(suit); }
    int getPipAsInt() const { return static_cast<int>(pip); }
    QString serialize() const { return QString("%1;%2").arg(getSuitAsInt()).arg(getPipAsInt()); }
    static Card deserialize(QString str);

private:
    Suit suit;
    Pip pip;
};

#endif // CARD_H
