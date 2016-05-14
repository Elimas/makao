#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), player(NULL), server(NULL), client(NULL), lobbyWidget(NULL),

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
    delete lobbyWidget;
    //delete gameScreenWidget;
    /*delete player;
    delete server;
    delete client;*/
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

void MainWindow::hostGame(QString playerName, int port)
{
    //if (player != NULL) delete player;
    //if (server != NULL) delete server;
    player = new Player(this, playerName);
    server = new Server(this, player);
    lobbyWidget = LobbyWidget::createServerLobby(this, server, port);
    lobbyWidget->show();
    lobbyWidget->startServer();
}

void MainWindow::joinGame(QString playerName, QString ip, int port)
{
    //if (player != NULL) delete player;
    //if (server != NULL) delete server;
    player = new Player(this, playerName);
    client = new Client(this, player);
    lobbyWidget = LobbyWidget::createClientLobby(this, client, ip, port);
    lobbyWidget->show();
    lobbyWidget->startClient();
}

void MainWindow::on_pushButton_2_clicked()
{
    hostGame("hostPlayer", port);
}

void MainWindow::on_pushButton_clicked()
{
    joinGame("clientPlayer", "127.0.0.1", port);
}
