#ifndef OPPONENTCARDSWIDGET_H
#define OPPONENTCARDSWIDGET_H

#include <QWidget>
#include <QStack>

namespace Ui {
class OpponentCardsWidget;
}

class OpponentCardsWidget : public QWidget
{
    Q_OBJECT

public:
	enum class Orientation
	{
		Horizontal,
		Vertical
	};

    explicit OpponentCardsWidget(QWidget *parent = 0);
    ~OpponentCardsWidget();

    void setCardsNumber(int cardsNumber);
    void setOrientation(Orientation newOrientation);

private:
    Ui::OpponentCardsWidget *ui;
    QStack<QWidget*> cards;
    Orientation orientation = Orientation::Horizontal;

    void addCard();
    void removeCard();
};

#endif // OPPONENTCARDSWIDGET_H
