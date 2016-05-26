#include "cardswidget.h"
#include "ui_cardswidget.h"
#include <QMessageBox>

CardsWidget::CardsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardsWidget)
{
    ui->setupUi(this);
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
    SingleCardWidget* cardWidget = new SingleCardWidget(card, ui->cardsFrame);
	connect(cardWidget, &SingleCardWidget::cardClicked, [=](const Card& clickedCard, const SingleCardWidget* widget)
	{
		for(int widgetIndex = 0; widgetIndex < cardWidgets.length(); ++widgetIndex)
		{
			if(cardWidgets[widgetIndex] == widget)
			{
				emit cardClicked(clickedCard, widgetIndex);
			}
		}
	});
    cardWidget->move(calculateCardHorizontalPosition(cardWidgets.length()), 0);
	cardWidget->show();
    cardWidgets.push_back(cardWidget);
}

void CardsWidget::removeCard(int index)
{
    SingleCardWidget* card = cardWidgets[index];
    card->deleteLater();
    cardWidgets.removeAt(index);
	rearrangeCards();
}

void CardsWidget::rearrangeCards()
{
	for (int widgetIndex = 0; widgetIndex < cardWidgets.length(); ++widgetIndex)
	{
		cardWidgets[widgetIndex]->move(calculateCardHorizontalPosition(widgetIndex), 0);
	}
}

int CardsWidget::calculateCardHorizontalPosition(int cardIndex)
{
	const int cardDistance = 30;
	return cardDistance * cardIndex;
}
