#include "cardswidget.h"
#include "ui_cardswidget.h"
#include <QMessageBox>

/*do usunięcia. test sprawdza czy sygnał cardClicked działa*/
void showClickedCard(const Card& card)
{
	QMessageBox msg;
	msg.setIcon(QMessageBox::Information);
	msg.setText("Kliknięto kartę: " + card.getSuitAsText() + card.getPipAsText());
	msg.exec();
}

CardsWidget::CardsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardsWidget)
{
    ui->setupUi(this);

    Card card(Card::Suit::Heart, Card::Pip::Queen);
    Card card2(Card::Suit::Spade, Card::Pip::King);
    Card card3(Card::Suit::Diamond, Card::Pip::Card9);

    addCard(card);
    addCard(card2);
    addCard(card3);

	connect(this, &CardsWidget::cardClicked, showClickedCard);
}

CardsWidget::~CardsWidget()
{
    delete ui;
    for(SingleCardWidget* cardWidget : cardWidgets)
    {
        delete cardWidget;
    }
}

void CardsWidget::addCard(const Card& card)
{
    const int cardDistance = 30;
    SingleCardWidget* cardWidget = new SingleCardWidget(card, ui->cardsFrame);
    connect(cardWidget, &SingleCardWidget::cardClicked, this, &CardsWidget::cardClicked);
    cardWidget->move(cardDistance * cardWidgets.length(), 0);
	cardWidget->show();
    cardWidgets.push_back(cardWidget);
}
