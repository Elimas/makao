#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->txtNick->setVisible(false);
    ui->txtIP->setVisible(false);
    ui->gphNick->setVisible(false);
    ui->gphIP->setVisible(false);
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
        ui->txtNick->setVisible(false);
        ui->txtIP->setVisible(false);
        ui->gphNick->setVisible(false);
        ui->gphIP->setVisible(false);
        ui->btnWyjscie->setText("Wyjście");
    }
    else
    {
      QApplication::exit();
    }
}

void MainWindow::on_btnDolacz_clicked()
{
    ui->btnDolacz->setVisible(true);
    ui->btnStworz->setVisible(false);
    ui->btnWyjscie->setText("Powrót");
    ui->txtNick->setVisible(true);
    ui->txtIP->setVisible(true);
    ui->gphNick->setVisible(true);
    ui->gphIP->setVisible(true);

}

void MainWindow::on_btnStworz_clicked()
{
    ui->btnDolacz->setVisible(false);
    ui->btnStworz->setVisible(true);
    ui->btnWyjscie->setText("Powrót");
    ui->txtNick->setVisible(true);
    ui->gphNick->setVisible(true);

}
