#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gphNazwaGry->setVisible(false);
    ui->gphHaslo->setVisible(false);
    ui->txtNazwaGry->setVisible(false);
    ui->txtHaslo->setVisible(false);
    ui->rbtnHaslo->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}






void MainWindow::on_btnWyjscie_clicked()
{
    if(ui->btnWyjscie->text()=="Powrót")
    {
        ui->btnDolacz->setVisible(true);
        ui->btnStworz->setVisible(true);
        ui->btnWyjscie->setText("Wyjście");
    }
    else
    {
      QApplication::exit();
    }
}

void MainWindow::on_btnDolacz_clicked()
{
    ui->btnDolacz->setVisible(false);
    ui->btnStworz->setVisible(false);
    ui->btnWyjscie->setText("Powrót");
}

void MainWindow::on_btnStworz_clicked()
{
    ui->btnDolacz->setVisible(false);
    ui->btnStworz->setVisible(false);
    ui->btnWyjscie->setText("Powrót");
}
