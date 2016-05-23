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
    const int cardDistance = 30;
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
    cardWidget->move(cardDistance * cardWidgets.length(), 0);
	cardWidget->show();
    cardWidgets.push_back(cardWidget);
}

void CardsWidget::removeCard(int index)
{
    cardWidgets.removeAt(index);
}
