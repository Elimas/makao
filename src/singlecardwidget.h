#ifndef SINGLECARDWIDGET_H
#define SINGLECARDWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "card.h"

namespace Ui {
class SingleCardWidget;
}

class SingleCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SingleCardWidget(const Card& card, QWidget *parent = 0);
    ~SingleCardWidget();

    Card getCard() const;

private:
    Ui::SingleCardWidget *ui;
    Card card;

    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

signals:
	void cardClicked(const Card& card);
};

#endif // SINGLECARDWIDGET_H
