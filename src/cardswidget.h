#ifndef CARDSWIDGET_H
#define CARDSWIDGET_H

#include <QWidget>
#include <QList>

#include "card.h"
#include "singlecardwidget.h"

namespace Ui {
class CardsWidget;
}

class CardsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardsWidget(QWidget *parent = nullptr);
    ~CardsWidget();

    void addCard(const Card& card);
    void removeCard(int index);
	void rearrangeCards();
	static int calculateCardHorizontalPosition(int cardIndex);

signals:
    void cardClicked(const Card& card, int cardIndex);

private:
    Ui::CardsWidget *ui;
    QList<SingleCardWidget*> cardWidgets;

};

#endif // CARDSWIDGET_H
