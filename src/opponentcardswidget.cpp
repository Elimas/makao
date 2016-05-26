#include "opponentcardswidget.h"
#include "ui_opponentcardswidget.h"

OpponentCardsWidget::OpponentCardsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpponentCardsWidget)
{
    ui->setupUi(this);
}

OpponentCardsWidget::~OpponentCardsWidget()
{
    while (cards.length() > 0) {
        removeCard();
    }
    delete ui;
}

void OpponentCardsWidget::setCardsNumber(int cardsNumber)
{
    while(cards.length() < cardsNumber)
    {
        addCard();
    }

    while (cards.length() > cardsNumber) {
        removeCard();
    }
}

void OpponentCardsWidget::setOrientation(Orientation newOrientation)
{
    orientation = newOrientation;
}

void OpponentCardsWidget::addCard()
{
    const int cardDistance = 15;
    QWidget* cardWidget = new QWidget(ui->cardsFrame);
    cardWidget->setStyleSheet("background-image: url(:/game/Grafiki/card_back.png);");
    cardWidget->resize(120, 171);
    if(orientation == Orientation::Horizontal)
    {
        cardWidget->move(cardDistance * cards.length(), 0);
    }
    else
    {
        cardWidget->move(0, cardDistance * cards.length());
    }
    cardWidget->show();
    cards.push_back(cardWidget);
}

void OpponentCardsWidget::removeCard()
{
    if(cards.length() > 0)
    {
        delete cards.pop();
    }
}
