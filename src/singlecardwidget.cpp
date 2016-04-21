#include "singlecardwidget.h"
#include "ui_singlecardwidget.h"
#include "card.h"

SingleCardWidget::SingleCardWidget(const Card& card, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleCardWidget),
    card(card)
{
    ui->setupUi(this);
    ui->pip1->setText(card.getPipAsText());
    ui->pip2->setText(card.getPipAsText());
    ui->suit1->setText(card.getSuitAsText());
    ui->suit2->setText(card.getSuitAsText());
    if(card.getColor() == Card::Color::Red)
    {
        this->setStyleSheet("QLabel { color : red; }");
    }
    else
    {
        this->setStyleSheet("QLabel { color : black; }");
    }
}

SingleCardWidget::~SingleCardWidget()
{
    delete ui;
}

Card SingleCardWidget::getCard() const
{
    return card;
}

void SingleCardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit cardClicked(getCard());
    }
}
