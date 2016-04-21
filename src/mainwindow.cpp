#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cardswidget.h"
#include "card.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addCardButton_clicked()
{
	Card card(Card::Suit::Club, Card::Pip::Ace);
	ui->cardsWidget->addCard(card);
}
