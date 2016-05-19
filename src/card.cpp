#include "card.h"
#include <QStringList>

Card::Card(const Suit suit, const Pip pip)
    :   suit(suit)
    ,   pip(pip)
{

}

Card::Card(const Card &card)
    :   suit(card.suit)
    ,   pip(card.pip)
{

}

Card::Suit Card::getSuit() const
{
    return suit;
}

Card::Pip Card::getPip() const
{
    return pip;
}

Card::Color Card::getColor() const
{
    return (suit == Suit::Heart || suit == Suit::Diamond) ? Color::Red : Color::Black;
}

QString Card::getSuitAsText() const
{
    switch(suit)
    {
    case Suit::Heart:
        return QString::fromWCharArray(L"\u2665");
    case Suit::Diamond:
        return QString::fromWCharArray(L"\u2666");
    case Suit::Club:
        return QString::fromWCharArray(L"\u2663");
    case Suit::Spade:
        return QString::fromWCharArray(L"\u2660");
    default:
        return "X";
    }
}

QString Card::getPipAsText() const
{
    switch(pip) {
    case Pip::Card2:
        return "2";
    case Pip::Card3:
        return "3";
    case Pip::Card4:
        return "4";
    case Pip::Card5:
        return "5";
    case Pip::Card6:
        return "6";
    case Pip::Card7:
        return "7";
    case Pip::Card8:
        return "8";
    case Pip::Card9:
        return "9";
    case Pip::Card10:
        return "10";
    case Pip::Jack:
        return "J";
    case Pip::Queen:
        return "Q";
    case Pip::King:
        return "K";
    case Pip::Ace:
        return "A";
    default:
        return "X";
    }
}

Card Card::deserialize(QString str)
{
    QStringList a = str.split(";");
    return Card(static_cast<Card::Suit>(a.value(0).toInt()), static_cast<Card::Pip>(a.value(1).toInt()));
}
